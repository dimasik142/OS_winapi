#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <strsafe.h>
#include <iostream>
#include <fstream>
#include<string.h>
#include <tchar.h>

using namespace std;


#define BUFSIZE 512
LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\game");
DWORD WINAPI messaging(LPVOID), cbWrittenBytes;
VOID GetAnswerToRequest(LPTSTR, LPTSTR, LPDWORD);
int index = 0;

int main(int argc, TCHAR *argv[])
{
	setlocale(LC_ALL, "Russian");
	BOOL fConnected = FALSE;
	DWORD dwThreadId = 0;
	HANDLE hPipe = INVALID_HANDLE_VALUE;
	HANDLE hThread = NULL;

	for (;;)
	{
		hPipe = CreateNamedPipe(lpszPipename, PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 
			PIPE_UNLIMITED_INSTANCES, BUFSIZE, BUFSIZE, 0, NULL
		);

		if (hPipe == INVALID_HANDLE_VALUE)
		{
			cout << "CreateNamedPipe failed, error code is = "
				<< GetLastError() << "." << endl;
			return -1;
		}
		fConnected = ConnectNamedPipe(hPipe, NULL);
		if (fConnected == 0)
		{
			if (GetLastError() == ERROR_PIPE_CONNECTED)
			{
				fConnected = TRUE;
			}
		}

		if (fConnected)
		{
			hThread = CreateThread( NULL, 0, messaging,	(LPVOID)hPipe,0,NULL);

			if (hThread == NULL)
			{
				cout << "CreateThread failed, error code is = "
					<< GetLastError() << "." << endl;
				return -1;
			}
			else {
				CloseHandle(hThread);
			}
		}
		else {
			CloseHandle(hPipe);
		}
	}
	return 0;
}

DWORD WINAPI messaging(LPVOID lpvParam)
{
	HANDLE hHeap = GetProcessHeap();
	TCHAR* pchRequest = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE * sizeof(TCHAR));
	TCHAR* pchReply = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE * sizeof(TCHAR));
	DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0;
	BOOL fSuccess = FALSE;
	HANDLE hPipe = NULL;

	char buff[5];
	int resultsArray[100];

	if (lpvParam == NULL)
	{
		cout << "ERROR - Server:" << endl;
		if (pchReply != NULL)
		{
			HeapFree(hHeap, 0, pchReply);
		}
		if (pchRequest != NULL)
		{
			HeapFree(hHeap, 0, pchRequest);
		}
		return 1;
	}

	if (pchRequest == NULL)
	{
		cout << "ERROR - Server:" << endl;
		CloseHandle(hPipe);
		if (pchReply != NULL)
		{
			HeapFree(hHeap, 0, pchReply);
		}
		return 1;
	}
	if (pchReply == NULL)
	{
		cout << "ERROR - Server:" << endl;
		CloseHandle(hPipe);
		if (pchRequest != NULL)
		{
			HeapFree(hHeap, 0, pchRequest);
		}
		return 1;
	}

	std::cout << "Server: Client thread successfully created." << endl;
	hPipe = (HANDLE)lpvParam;

	while (1) {
		fSuccess = ReadFile( hPipe, pchRequest, BUFSIZE * sizeof(TCHAR), &cbBytesRead, NULL); 

		ifstream fin("gameResults.txt");

		int item = 0;
		while (fin >> buff) {
			resultsArray[item] = atoi(buff);
			item++;
		}

		fin.close();
		int userTime = _wtoi(pchRequest);

		resultsArray[item] = userTime;

		// сортировка масива
		int temp = 0;
		bool exit = false; 
		while (!exit) {
			exit = true;
			for (int int_counter = 0; int_counter < item+1; int_counter++) 
				if (resultsArray[int_counter] < resultsArray[int_counter + 1]) {

					temp = resultsArray[int_counter];
					resultsArray[int_counter] = resultsArray[int_counter + 1];
					resultsArray[int_counter + 1] = temp;
					exit = false; 
				}
		}
		if (index == 0) {
			for (int i = 0; i < item + 1; i++) {
				cout << resultsArray[i] << " __ ";
			}
			cout << endl;
		}
		if (userTime > (int)resultsArray[4]) {
			TCHAR bufasdasds[BUFSIZE] = L"Ви в топ 5";
			DWORD cbToWriteBytes = (lstrlen(bufasdasds) + 1) * sizeof(TCHAR);
			fSuccess = WriteFile(hPipe, bufasdasds, cbToWriteBytes, &cbWrittenBytes, NULL);
		}
		else {
			TCHAR bufasdasds[BUFSIZE] = L"Грай далі, тобі ще далеко до топ 5";
			DWORD cbToWriteBytes = (lstrlen(bufasdasds) + 1) * sizeof(TCHAR);
			fSuccess = WriteFile(hPipe, bufasdasds, cbToWriteBytes, &cbWrittenBytes, NULL);
		}
		if (index == 0) {
			ofstream fout;
			fout.open("gameResults.txt");
			for (int i = 0; i < item + 1; i++) {
				fout << resultsArray[i] << endl;
			}
			fout.close();
			index = 100;
		}

		if (!fSuccess || cbReplyBytes != cbWritten) {
				break;
		}
	}
	FlushFileBuffers(hPipe);
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);
	HeapFree(hHeap, 0, pchRequest);
	HeapFree(hHeap, 0, pchReply);
	cout << "Server: Client thread exiting." << endl;
	return 0;
}


