#include <windows.h> // підключення бібліотеки з функціями API
#include "resource.h" // підключення файлу ресурсів
#include <windowsx.h>

// Глобальні змінні:
HINSTANCE hInst; 	//Дескриптор програми	
LPCTSTR szWindowClass = L"Rybkin";
LPCTSTR szTitle = L"TicTacToe";

const int CELL_SIZE = 100;
HBRUSH hbr1, hbr2;
HICON hIcon1, hIcon2;
HWND hEdit;
int playerTurn = 1;
int gameBoard[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0};
int winner = 0;
int wins[3];
int numPage = 1;

// Попередній опис функцій

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
// Основна програма 
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
	int nCmdShow)
{
	MSG msg;

	// Реєстрація класу вікна 
	MyRegisterClass(hInstance);

	// Створення вікна програми
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}
	// Цикл обробки повідомлень
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW; 		//стиль вікна
	wcex.lpfnWndProc = (WNDPROC)WndProc; 		//віконна процедура
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance; 			//дескриптор програми
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)); 		//визначення іконки
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW); 	//визначення курсору
	wcex.hbrBackground = GetSysColorBrush(COLOR_WINDOW); //установка фону
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1); 				//визначення меню
	wcex.lpszClassName = szWindowClass; 		//ім’я класу
	wcex.hIconSm = NULL;

	return RegisterClassEx(&wcex); 			//реєстрація класу вікна
}

// FUNCTION: InitInstance (HANDLE, int)
// Створює вікно програми і зберігає дескриптор програми в змінній hInst

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	hInst = hInstance; //зберігає дескриптор додатка в змінній hInst
	hWnd = CreateWindow(szWindowClass, 	// ім’я класу вікна
		szTitle, 				// назва програми
		WS_OVERLAPPEDWINDOW,			// стиль вікна
		CW_USEDEFAULT, 			// положення по Х	
		CW_USEDEFAULT,			// положення по Y	
		CW_USEDEFAULT, 			// розмір по Х
		CW_USEDEFAULT, 			// розмір по Y
		NULL, 					// дескриптор батьківського вікна	
		NULL, 					// дескриптор меню вікна
		hInstance, 				// дескриптор програми
		NULL); 				// параметри створення.

	if (!hWnd) 	//Якщо вікно не творилось, функція повертає FALSE
	{
		return FALSE;
	}
	ShowWindow(hWnd, nCmdShow); 		//Показати вікно
	UpdateWindow(hWnd); 				//Оновити вікно
	return TRUE;
}

//Малювання ігрової дошки
BOOL GetGameBoardRect(HWND hWnd, RECT* pRect)
{	
	RECT rc;

	if (GetClientRect(hWnd, &rc))
	{
		int width = rc.right - rc.left;
		int height = rc.bottom - rc.top;

		pRect->left = (width - CELL_SIZE * 3) / 2;
		pRect->top = (height - CELL_SIZE * 3) / 2;

		pRect->right = pRect->left + CELL_SIZE * 3;
		pRect->bottom = pRect->top + CELL_SIZE * 3;

		return TRUE;
	}

	SetRectEmpty(pRect);
	return FALSE;
}

//Малювання ліній для комірок дошки
void DrawLine(HDC hdc, int x1, int y1, int x2, int y2)
{
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
}

int GetCellNumberFromPoint(HWND hWnd, int x, int y)
{
	POINT pt = {x, y};
	RECT rc;

	if (GetGameBoardRect(hWnd, &rc))
	{
		if (PtInRect(&rc, pt))
		{
			//Клік всередині ігрової дошки
			x = pt.x - rc.left;
			y = pt.y - rc.top;

			int column = x / CELL_SIZE;
			int row = y / CELL_SIZE;
			
			//конвертація в індекс
			return column + row * 3;
		}
	}

	return -1; //Клік зовні ігрової дошки
}

BOOL GetCellRect(HWND hWnd, int index, RECT* pRect)
{
	RECT rcBoard;

	SetRectEmpty(pRect);

	if (index < 0 || index > 8)
		return FALSE;

	if (GetGameBoardRect(hWnd, &rcBoard))
	{
		//Конвертація індексу в x, y
		int y = index / 3; //Номер рядка
		int x = index % 3; //Номер стовбця

		pRect->left = rcBoard.left + x * CELL_SIZE + 1;
		pRect->top = rcBoard.top + y * CELL_SIZE + 1;
		pRect->right = pRect->left + CELL_SIZE - 1;
		pRect->bottom = pRect->top + CELL_SIZE - 1;

		return TRUE;
	}
	return FALSE;
}

/*
Повертає:
0 - Нема переможця
1 - Гравець переміг
2 - Гравець переміг
3 - Нічия
*/

int GetWinner(int wins[3])
{
	int cells[] = { 0,1,2, 3,4,5, 6,7,8, 0,3,6, 1,4,7 ,2,5,8, 0,4,8, 2,4,6 };

	//перевірка на переможця
	for (int i = 0; i < ARRAYSIZE(cells); i += 3)
	{
		if ((0 != gameBoard[cells[i]]) && gameBoard[cells[i]] == gameBoard[cells[i + 1]] && gameBoard[cells[i]] == gameBoard[cells[i + 2]])
		{
			wins[0] = cells[i];
			wins[1] = cells[i + 1];
			wins[2] = cells[i + 2];

			return gameBoard[cells[i]];
		}
	}

	//Первірка чи маємо ми ще пусту клітинку
	for (int i = 0; i < ARRAYSIZE(gameBoard); ++i)
		if (0 == gameBoard[i])
			return 0; //продовжуємо гру
	
	//Нічия
	return 3;
}

void ShowTurn(HWND hWnd, HDC hdc)
{
	RECT rc;

	static const WCHAR szTurn1[] = L"Черга: Гравця 1";
	static const WCHAR szTurn2[] = L"Черга: Гравця 2";

	const WCHAR* pszTurnText = (playerTurn == 1) ? szTurn1 : szTurn2;

	switch (winner)
	{
	case 0: //Продовження гри
		pszTurnText = (playerTurn == 1) ? szTurn1 : szTurn2;
		break;
	case 1: //Гравець 1 переміг
		pszTurnText = L"Гравець 1 переміг!";
		break;
	case 2: //Гравець 2 переміг
		pszTurnText = L"Гравець 2 переміг!";
		break;
	case 3: //Нічия
		pszTurnText = L"Нічия!";
		break;
	}

	if (pszTurnText != NULL && GetClientRect(hWnd, &rc))
	{
		rc.top = rc.bottom - 48;
		FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
		SetTextColor(hdc, RGB(0, 0, 0));
		SetBkMode(hdc, TRANSPARENT);
		DrawText(hdc, pszTurnText, lstrlen(pszTurnText), &rc, DT_CENTER);
	}
}

void DrawIconCentered(HDC hdc, RECT* pRect, HICON hIcon)
{
	const int ICON_WIDTH = GetSystemMetrics(SM_CXICON);
	const int ICON_HEIGHT = GetSystemMetrics(SM_CYICON);

	if (pRect != NULL)
	{
		int left = pRect->left + ((pRect->right - pRect->left) - ICON_WIDTH) / 2;
		int top = pRect->top + ((pRect->bottom - pRect->top) - ICON_HEIGHT) / 2;
		DrawIcon(hdc, left, top, hIcon);
	}
}

void ShowWinner(HWND hWnd, HDC hdc)
{
	RECT rcWin;

	for (int i = 0; i < 3; ++i)
	{
		if (GetCellRect(hWnd, wins[i], &rcWin))
		{
			FillRect(hdc, &rcWin, hbr1);
			DrawIconCentered(hdc, &rcWin, (winner == 1) ? hIcon1 : hIcon2);
		}
	}
}

// FUNCTION: WndProc (HWND, unsigned, WORD, LONG)
// Віконна процедура. Приймає і обробляє всі повідомлення, що приходять в додаток

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE: 				//Повідомлення приходить при створенні вікна
		{
			hbr1 = CreateSolidBrush(RGB(255, 0, 0));
			hbr2 = CreateSolidBrush(RGB(0, 0, 255));

			//Завантаження іконків гравців
			hIcon1 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_PLAYER1));
			hIcon2 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_PLAYER2));
		}
		break;
	case WM_COMMAND:
		{	
			switch (LOWORD(wParam))
			{
			case IDM_NEWGAME:
				{
					int ret = MessageBox(hWnd, L"Ви дійсно хочете почати нову гру?", L"New Game", MB_YESNO | MB_ICONQUESTION);
				
					if (IDYES == ret)
					{
						//Початок нової гри
						playerTurn = 1;
						winner = 0;
						ZeroMemory(gameBoard, sizeof(gameBoard));
						
						InvalidateRect(hWnd, NULL, TRUE); 
						UpdateWindow(hWnd);
					}
				}
				break;
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
	case WM_LBUTTONDOWN:
		{
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);

			
			if (playerTurn == 0) {
				break;
			}
			int index = GetCellNumberFromPoint(hWnd, xPos, yPos);

			hdc = GetDC(hWnd);
			if (NULL != hdc)
			{
				//WCHAR temp[100];

				//wsprintf(temp, L"Index = %d", index);
				//TextOut(hdc, xPos, yPos, temp, lstrlen(temp));

				//Отриманння клітинки за індексом
				if (index != -1)
				{
					RECT rcCell;
					if ((0 == gameBoard[index]) && GetCellRect(hWnd, index, &rcCell))
					{
						gameBoard[index] = playerTurn;

						//FillRect(hdc, &rcCell, (playerTurn == 2) ? hbr2 : hbr1); 
						//DrawIcon(hdc, rcCell.left, rcCell.top, (playerTurn == 1) ? hIcon1 : hIcon2);
						DrawIconCentered(hdc, &rcCell, (playerTurn == 1) ? hIcon1 : hIcon2);

						//Перевірка на переможця
						winner = GetWinner(wins);
						if (winner == 1 || winner == 2)
						{
							ShowWinner(hWnd, hdc);

							//Маємо переможця
							MessageBox(
								hWnd, 
								(winner == 1) ? L"Гравець 1 переміг!" : L"Гравець 2 переміг!", 
								L"Перемога!", 
								MB_OK | MB_ICONINFORMATION);
							playerTurn = 0;
						}
						else if (winner == 3)
						{
							MessageBox(
								hWnd,
								L"Переможця нема.",
								L"Нічия!",
								MB_OK | MB_ICONEXCLAMATION);
						}
						else if (winner == 0)
						{
							playerTurn = (playerTurn == 2) ? 1 : 2;
						}

						//Відображення черги на хід 
						ShowTurn(hWnd, hdc);
					}
				}
				ReleaseDC(hWnd, hdc);
			}
		
		}
		break;
	case WM_GETMINMAXINFO: // Мінімальний розмір вікна
		{
			MINMAXINFO* pMinMax = (MINMAXINFO*)lParam;

			pMinMax->ptMinTrackSize.x = CELL_SIZE * 5;
			pMinMax->ptMinTrackSize.y = CELL_SIZE * 5;
		}
		break;
	case WM_PAINT: 				//Перемалювати вікно
		hdc = BeginPaint(hWnd, &ps); 	//Почати графічний вивід	
		
		RECT rc;

		if (GetGameBoardRect(hWnd, &rc))
		{
			RECT rcClient;

			//Відображення гравця та черги
			if (GetClientRect(hWnd, &rcClient))
			{
				const WCHAR szPLayer1[] = L"Player 1";
				const WCHAR szPLayer2[] = L"Player 2";

				SetBkMode(hdc, TRANSPARENT);

				//Малювання тексту
				TextOut(hdc, 16, 16, szPLayer1, lstrlen(szPLayer1));
				DrawIcon(hdc, 24, 34, hIcon1);
				TextOut(hdc, rcClient.right - 82, 16, szPLayer2, lstrlen(szPLayer2));
				DrawIcon(hdc, rcClient.right - 74, 34, hIcon2);

				//Відображення черги на хід 
				ShowTurn(hWnd, hdc);
			}

			FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
			//Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);

			for (int i = 0; i < 4; i++)
			{
				//Малювання вертикальних ліній
				DrawLine(hdc, rc.left + CELL_SIZE * i, rc.top, rc.left + CELL_SIZE * i, rc.bottom);
				//Малювання горизонтальних ліній
				DrawLine(hdc, rc.left, rc.top + CELL_SIZE * i, rc.right, rc.top + CELL_SIZE * i);
			}

			//Малювання всіх зайнятих клітинок
			RECT rcCell;

			for (int i = 0; i < ARRAYSIZE(gameBoard); ++i)
			{
				if ((0 != gameBoard[i]) && GetCellRect(hWnd, i, &rcCell))
				{
					//FillRect(hdc, &rcCell, (gameBoard[i] == 2) ? hbr2 : hbr1);
					DrawIconCentered(hdc, &rcCell, (gameBoard[i] == 1) ? hIcon1 : hIcon2);
				}
			}

			if (winner == 1 || winner == 2)
			{
				//Відображення переможця
				ShowWinner(hWnd, hdc);
			}
		}

		EndPaint(hWnd, &ps); 		//Закінчити графічний вивід	
		break;

	case WM_DESTROY: 				//Завершення роботи
		DeleteObject(hbr1);
		DeleteObject(hbr2);
		DestroyIcon(hIcon1);
		DestroyIcon(hIcon2);
		PostQuitMessage(0);
		break;
	default:
		//Обробка повідомлень, які не оброблені користувачем
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}