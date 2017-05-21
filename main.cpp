#include<windows.h>
#include<string.h>
#include <ctime>
#include <fstream>


#define BLOCK_HEIGHT 100

using namespace std;

BOOL Line(HDC,int,int,int,int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HANDLE handeForChangeColor, handeForAddNewAuto, handeForCheckThreeAuto;

CRITICAL_SECTION critic_section;

DWORD WINAPI addAutoToMap(LPVOID);
DWORD WINAPI changeAutosAndMapColor(LPVOID);
DWORD WINAPI checkToTheThreeAutos(LPVOID);

int autosColor[3] = {111,111,111};
int positionAutos[5][2] = { {1000,3},{1000,3},{1000,3},{1000,3},{1000,3} };
const int yCordinates[3][2] = {{30,170},{230, 370},{430, 570} };

const int priorityProcessArray[3] = { 
	2 ,  // addAutoToMap
	1 ,  // changeAutosColor
	3    // chackAutos
};

char szProgName[] = "Егра";
int timeStatus = 0, playerPosition = 1 , saveAuto = 100, saveAutoTime = 0 , AS_mode = 0;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	HWND hWnd;
	MSG lpMsg;
	WNDCLASS w;

	w.lpszClassName = L"Егра"; 
	w.hInstance = hInstance;
	w.lpfnWndProc = WndProc;
	w.hCursor = LoadCursor(NULL, IDC_ARROW); 
	w.hIcon = 0;
	w.lpszMenuName = 0;
	w.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	w.style = CS_HREDRAW | CS_VREDRAW;
	w.cbClsExtra = 0;
	w.cbWndExtra = 0;

	if (!RegisterClass(&w))
		return 0;

	hWnd = CreateWindow(L"Егра", L"Тіпа егра", WS_OVERLAPPED | WS_BORDER | WS_SYSMENU, 
		300,0,600,700, 
		(HWND)NULL, 
		(HMENU)NULL,
		(HINSTANCE)hInstance, 
		(HINSTANCE)NULL); 
	  
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	while (GetMessage(&lpMsg, NULL, 0, 0)) {
		TranslateMessage(&lpMsg);
		DispatchMessage(&lpMsg);
	}
	if (AS_mode == 0) {
		DeleteCriticalSection(&critic_section);
	}
	return(lpMsg.wParam);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	
	HDC hdc; 
	PAINTSTRUCT ps; 
	RECT autos[5], player;
	HANDLE thread;
	HBRUSH Brush;

	int delta;


	switch (messg) {


		case WM_CREATE:
			if (AS_mode == 0) {
				InitializeCriticalSection(&critic_section);
			}
			SetTimer(hWnd, 1, 50, NULL);
			handeForAddNewAuto = CreateThread(NULL, 0, addAutoToMap, NULL, 0, NULL);
			SetForegroundWindow(hWnd);
			break;


		case WM_TIMER:

			srand(timeStatus);
			timeStatus++;
			
			for (int item = 0; item < 5; item++) {
				if (positionAutos[item][0] + BLOCK_HEIGHT > 550 && positionAutos[item][0] < 650 && positionAutos[item][1] == playerPosition) {
					KillTimer(hWnd, 1);
					MessageBox(NULL, (LPCTSTR)L"канец", L"канец", MB_ICONASTERISK | MB_OK);
					PostQuitMessage(0);

				}
				positionAutos[item][0] += 8 + (int)(timeStatus / 50);
			}

			delta = 30 - (int)(timeStatus / 50);
			if (delta < 10)
				delta = 10;

			if ((timeStatus % delta) == 0) {
				if (saveAutoTime != 0) {
					saveAutoTime--;
				}

				handeForAddNewAuto = CreateThread(NULL, 0, addAutoToMap, NULL, 0, NULL);
				SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
				SetThreadPriority(handeForAddNewAuto, priorityProcessArray[0]);
				//CloseHandle(handeForAddNewAuto);

				handeForCheckThreeAuto = CreateThread(NULL, 0, checkToTheThreeAutos, NULL, 0, NULL);
				SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
				SetThreadPriority(handeForCheckThreeAuto, priorityProcessArray[2]);
				//CloseHandle(handeForCheckThreeAuto);
			}

			InvalidateRect(hWnd, NULL, TRUE);
			break;


		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			for (int i = 0; i < 5; i++) {
				switch (positionAutos[i][1])
				{
					case 0:
						autos[i].left = yCordinates[0][1];
						autos[i].right = yCordinates[0][0];
						break;
					case 1:
						autos[i].left = yCordinates[1][1];
						autos[i].right = yCordinates[1][0];
						break;
					case 2:
						autos[i].left = yCordinates[2][1];
						autos[i].right = yCordinates[2][0];
						break;
					default :
						break;
				}

				autos[i].top = positionAutos[i][0] + BLOCK_HEIGHT;
				autos[i].bottom = positionAutos[i][0];
				if (saveAuto != 100) {
					if (saveAuto == positionAutos[i][1] && saveAutoTime > 0) {
						Brush = CreateSolidBrush(RGB(0, 255, 0));
						FillRect(hdc, &autos[i], Brush);
						DeleteObject(Brush);
					}
				}
				else {
					Brush = CreateSolidBrush(RGB(autosColor[0], autosColor[1], autosColor[2]));
					FillRect(hdc, &autos[i], Brush);
					DeleteObject(Brush);
				}
			}

			player.top = 650;
			player.left = yCordinates[playerPosition][1];
			player.right = yCordinates[playerPosition][0];
			player.bottom = 550;
			FillRect(hdc, &player, HBRUSH(CreateSolidBrush(RGB(55, 0, 100))));

			Line(hdc, 200, 0, 200, 800);
			Line(hdc, 400, 0, 400, 800);
		
			EndPaint(hWnd, &ps);
			break;


		case WM_KEYDOWN:
			handeForChangeColor = CreateThread(NULL, 0, changeAutosAndMapColor, NULL, 0, NULL);
			SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
			SetThreadPriority(handeForAddNewAuto, priorityProcessArray[1]);
			//CloseHandle(handeForChangeColor);

			switch (wParam) {
				case VK_LEFT:
					if (playerPosition != 0)
						playerPosition -= 1;
					InvalidateRect(hWnd, NULL, TRUE);
					break;
				case VK_RIGHT:
					if (playerPosition != 2)
						playerPosition += 1;
					InvalidateRect(hWnd, NULL, TRUE);
					break;
				default:
					break;
			}
			break;


		case WM_DESTROY:
			PostQuitMessage(0); 
			break;


		default:
			return(DefWindowProc(hWnd, messg, wParam, lParam)); 
	}
	return 0;
}

BOOL Line(HDC hdc, int x1, int y1, int x2, int y2)
{
	MoveToEx(hdc, x1, y1, NULL); 
	return LineTo(hdc, x2, y2);
}

DWORD WINAPI addAutoToMap(LPVOID t1)
{
	if (AS_mode == 0)
	{
		while (!TryEnterCriticalSection(&critic_section));
	}
	srand(time(0));
	int autoNumber = rand() % 5;
	bool addAutoStatus = false;

	while (!addAutoStatus) {
		if (positionAutos[autoNumber][0] > 700 || positionAutos[autoNumber][0] < -150) {
			positionAutos[autoNumber][1] = rand() % 3;
			positionAutos[autoNumber][0] = -100;
			addAutoStatus = true;
		}
		srand((int)t1);
		int autoNumber = rand() % 5;
	}
	if (AS_mode == 0) {
		LeaveCriticalSection(&critic_section);
	}

	return 0;
}

DWORD WINAPI changeAutosAndMapColor(LPVOID t)
{
	if (AS_mode == 0)
	{
		while (!TryEnterCriticalSection(&critic_section));
	}
	srand(time(0));

	for (int i = 0; i < 3; i++) {
		autosColor[i] = rand() % 255;
	}
	if (AS_mode == 0) {
		LeaveCriticalSection(&critic_section);
	}
	return 0;
}

DWORD WINAPI checkToTheThreeAutos(LPVOID t)
{
	int checkAutosPosition[5][2];
	int checkPoint = 0;

	for (int i = 0; i < 5; i++) {
		if ( positionAutos[i][0] < 600  && positionAutos[i][0] > 300 ) {
			checkAutosPosition[checkPoint][0] = positionAutos[i][0];
			checkAutosPosition[checkPoint][1] = positionAutos[i][1];
			checkPoint++;
		}
	}

	bool checkLinesByAutos[3] = { FALSE , FALSE , FALSE };

	for (int i = 0; i < checkPoint; i++) {
		checkLinesByAutos[checkAutosPosition[i][1]] = TRUE;
	}

	if (checkLinesByAutos[0] == FALSE || checkLinesByAutos[1] == FALSE || checkLinesByAutos[2] == FALSE)
		return 0;

	srand(time(0));
	saveAuto = rand() % 3;
	saveAutoTime = 5;
	return 0;
}