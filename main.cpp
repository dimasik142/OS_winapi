#include<windows.h>
#include<string.h>
#include <ctime>

#define BLOCK_HEIGHT 100

using namespace std;

BOOL Line(HDC,int,int,int,int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void returnToMapRandAuto();

int positionAutos[3] = {-100,-1000,-1000};
const int yCordinates[3][2] = {{30,170},{230, 370},{430, 570} }; // масив кординат по осі y
char szProgName[] = "Егра";
int timeStatus = 0, playerPosition = 1 , saveAuto = 4;



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	HWND hWnd;
	MSG lpMsg;
	WNDCLASS w;

	w.lpszClassName = L"Егра"; //имя программы - объявлено выше
	w.hInstance = hInstance; //идентификатор текущего приложения
	w.lpfnWndProc = WndProc; //указатель на функцию окна
	w.hCursor = LoadCursor(NULL, IDC_ARROW); //загружаем курсор
	w.hIcon = 0;
	w.lpszMenuName = 0;
	w.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //цвет фона окна
	w.style = CS_HREDRAW | CS_VREDRAW;
	w.cbClsExtra = 0;
	w.cbWndExtra = 0;

	//Если не удалось зарегистрировать класс окна - выходим
	if (!RegisterClass(&w))
		return 0;

	//Создадим окно в памяти, заполнив аргументы CreateWindow
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

	return(lpMsg.wParam);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	
	HDC hdc; 
	PAINTSTRUCT ps; 
	RECT auto1 , auto2, auto3 , player;

	switch (messg) {
		case WM_CREATE:
			SetTimer(hWnd, 1, 50, NULL);
			SetForegroundWindow(hWnd);
			returnToMapRandAuto();
			returnToMapRandAuto();
			break;
		case WM_TIMER:
			srand(timeStatus);
			timeStatus++;
			if (positionAutos[playerPosition] + BLOCK_HEIGHT > 550 && positionAutos[playerPosition] < 650) {
				KillTimer(hWnd, 1);
				MessageBox(NULL, (LPCTSTR)L"канец", L"канец", MB_ICONASTERISK | MB_OK);
				PostQuitMessage(0);

			}
			for (int i = 0; i < 3; i++) {
				positionAutos[i] += 8 + (int)(timeStatus / 50);
			}
			if ((timeStatus %40 - +(int)(timeStatus / 50)) == 0) {
				returnToMapRandAuto();
				returnToMapRandAuto();
			}
			if (positionAutos[1] < 400 && positionAutos[0] < 400 && positionAutos[2] < 400) {
				positionAutos[rand() % 3] = 10000;
			}
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);

			auto1.top = positionAutos[0] + BLOCK_HEIGHT;
			auto1.left = yCordinates[0][1];
			auto1.right = yCordinates[0][0];
			auto1.bottom = positionAutos[0];
			FillRect(hdc, &auto1, HBRUSH(CreateSolidBrush(RGB(111, 111, 111))));

			auto2.top = positionAutos[1] + BLOCK_HEIGHT;
			auto2.left = yCordinates[1][1];
			auto2.right = yCordinates[1][0];
			auto2.bottom = positionAutos[1];
			FillRect(hdc, &auto2, HBRUSH(CreateSolidBrush(RGB(111, 111, 111))));
		
			auto3.top = positionAutos[2] + BLOCK_HEIGHT;
			auto3.left = yCordinates[2][1];
			auto3.right = yCordinates[2][0];
			auto3.bottom = positionAutos[2];
			FillRect(hdc, &auto3, HBRUSH(CreateSolidBrush(RGB(111, 111, 111))));

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

void returnToMapRandAuto() {
	int autoNumber = rand() % 3;
	if (positionAutos[autoNumber] > 700 ) {
		positionAutos[autoNumber] = -100;
	}
}
