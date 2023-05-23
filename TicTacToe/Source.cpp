#include <windows.h> // ���������� �������� � ��������� API
#include "resource.h" // ���������� ����� �������
#include <windowsx.h>
#include <vector>

#include <string>

// �������� ����:
HINSTANCE hInst; 	//���������� ��������	
LPCTSTR szWindowClass = L"Rybkin";
LPCTSTR szTitle = L"TicTacToe";

const int CELL_SIZE = 50;
int numCells = 3;
HBRUSH hbr1, hbr2;
HICON hIcon1, hIcon2;
int playerTurn = 1;
//int gameBoard[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0};
std::vector<int> gameBoard((numCells * numCells), 0);

int winner = 0;
int wins[3];

// ��������� ���� �������

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK DlgProcSBG(HWND, UINT, WPARAM, LPARAM);
// ������� �������� 
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
	int nCmdShow)
{
	MSG msg;

	// ��������� ����� ���� 
	MyRegisterClass(hInstance);

	// ��������� ���� ��������
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}
	// ���� ������� ����������
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
	wcex.style = CS_HREDRAW | CS_VREDRAW; 		//����� ����
	wcex.lpfnWndProc = (WNDPROC)WndProc; 		//������ ���������
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance; 			//���������� ��������
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)); 		//���������� ������
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW); 	//���������� �������
	wcex.hbrBackground = GetSysColorBrush(COLOR_WINDOW); //��������� ����
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1); 				//���������� ����
	wcex.lpszClassName = szWindowClass; 		//��� �����
	wcex.hIconSm = NULL;

	return RegisterClassEx(&wcex); 			//��������� ����� ����
}

// FUNCTION: InitInstance (HANDLE, int)
// ������� ���� �������� � ������ ���������� �������� � ����� hInst

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	hInst = hInstance; //������ ���������� ������� � ����� hInst
	hWnd = CreateWindow(szWindowClass, 	// ��� ����� ����
		szTitle, 				// ����� ��������
		WS_OVERLAPPEDWINDOW,			// ����� ����
		CW_USEDEFAULT, 			// ��������� �� �	
		CW_USEDEFAULT,			// ��������� �� Y	
		CW_USEDEFAULT, 			// ����� �� �
		CW_USEDEFAULT, 			// ����� �� Y
		NULL, 					// ���������� ������������ ����	
		NULL, 					// ���������� ���� ����
		hInstance, 				// ���������� ��������
		NULL); 				// ��������� ���������.

	if (!hWnd) 	//���� ���� �� ���������, ������� ������� FALSE
	{
		return FALSE;
	}
	ShowWindow(hWnd, nCmdShow); 		//�������� ����
	UpdateWindow(hWnd); 				//������� ����
	return TRUE;
}

//��������� ������ �����
BOOL GetGameBoardRect(HWND hWnd, RECT* pRect)
{	
	RECT rc;

	if (GetClientRect(hWnd, &rc))
	{
		int width = rc.right - rc.left;
		int height = rc.bottom - rc.top;

		pRect->left = (width - CELL_SIZE * numCells) / 2;
		pRect->top = (height - CELL_SIZE * numCells) / 2;

		pRect->right = pRect->left + CELL_SIZE * numCells;
		pRect->bottom = pRect->top + CELL_SIZE * numCells;		

		return TRUE;
	}

	SetRectEmpty(pRect);
	return FALSE;
}

//��������� ��� ��� ������ �����
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
			//��� �������� ������ �����
			x = pt.x - rc.left;
			y = pt.y - rc.top;

			int column = x / CELL_SIZE;
			int row = y / CELL_SIZE;
			
			//����������� � ������
			return column + row * numCells;
		}
	}

	return -1; //��� ���� ������ �����
}

BOOL GetCellRect(HWND hWnd, int index, RECT* pRect)
{
	RECT rcBoard;

	SetRectEmpty(pRect);

	if (index < 0 || index > (gameBoard.size() - 1))
		return FALSE;

	if (GetGameBoardRect(hWnd, &rcBoard))
	{
		//����������� ������� � x, y
		int y = index / numCells; //����� �����
		int x = index % numCells; //����� �������

		pRect->left = rcBoard.left + x * CELL_SIZE + 1;
		pRect->top = rcBoard.top + y * CELL_SIZE + 1;
		pRect->right = pRect->left + CELL_SIZE - 1;
		pRect->bottom = pRect->top + CELL_SIZE - 1;

		return TRUE;
	}
	return FALSE;
}

/*
�������:
0 - ���� ���������
1 - 1 ������� ������
2 - 2 ������� ������
3 - ͳ���
*/

/*int GetWinner(int wins[3])
{
	int cells[] = { 0,1,2, 3,4,5, 6,7,8, 0,3,6, 1,4,7 ,2,5,8, 0,4,8, 2,4,6 };

	//�������� �� ���������
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

	//������� �� ���� �� �� ����� �������
	for (int i = 0; i < gameBoard.size(); ++i)
		if (gameBoard[i] == 0)
			return 0; //���������� ���
	
	//ͳ���
	return 3;

	
	for (int row = 0; row < numCells; row++) {
		for (int col = 0; col <= numCells - 5; col++) {
			int startCell = row * numCells + col;
			int player = gameBoard[startCell];
			if (player == 0) continue; // ���������� ������ ������
			bool win = true;
			for (int i = 1; i < 3; i++) {
				if (gameBoard[startCell + i] != player || (startCell + i) % numCells < col + i) {
					win = false;
					break;
				}
			}
			if (win) return player;
		}
	}

	// �������� ����������
	for (int col = 0; col < numCells; col++) {
		for (int row = 0; row <= numCells - 5; row++) {
			int startCell = row * numCells + col;
			int player = gameBoard[startCell];
			if (player == 0) continue; // ���������� ������ ������
			bool win = true;
			for (int i = 1; i < 5; i++) {
				if (gameBoard[startCell + i * numCells] != player) {
					win = false;
					break;
				}
			}
			if (win) return player;
		}
	}

	// Checking diagonals (right top to left bottom)
	for (int row = 0; row <= numCells - 5; row++) {
		for (int col = numCells - 1; col >= 4; col--) {
			int startCell = row * numCells + col;
			int player = gameBoard[startCell];
			if (player == 0) continue; // Skip empty cells
			bool win = true;
			for (int i = 1; i < 5; i++) {
				if (gameBoard[startCell + i * (numCells - 1)] != player) {
					win = false;
					break;
				}
			}
			if (win) return player;
		}
	}

	// Check for empty cells
	for (int cell : gameBoard) {
		if (cell == 0) return 0; // Game is still ongoing
	}

	// Draw
	return 3;
}
*/

int GetWinner(int size, int winLength)
{
	// �������� �� ���������� ���������� �� �����������
	for (int row = 0; row < size; ++row)
	{
		for (int col = 0; col <= size - winLength; ++col)
		{
			int startIndex = row * size + col;
			int symbol = gameBoard[startIndex];
			if (symbol != 0)
			{
				bool win = true;
				for (int i = 1; i < winLength; ++i)
				{
					if (gameBoard[startIndex + i] != symbol)
					{
						win = false;
						break;
					}
				}
				if (win)
					return symbol;
			}
		}
	}

	// �������� �� ���������� ���������� �� ���������
	for (int col = 0; col < size; ++col)
	{
		for (int row = 0; row <= size - winLength; ++row)
		{
			int startIndex = row * size + col;
			int symbol = gameBoard[startIndex];
			if (symbol != 0)
			{
				bool win = true;
				for (int i = 1; i < winLength; ++i)
				{
					if (gameBoard[startIndex + i * size] != symbol)
					{
						win = false;
						break;
					}
				}
				if (win)
					return symbol;
			}
		}
	}

	// �������� �� ���������� ���������� �� ����������
	for (int row = 0; row <= size - winLength; ++row)
	{
		for (int col = 0; col <= size - winLength; ++col)
		{
			int startIndex = row * size + col;
			int symbol = gameBoard[startIndex];
			if (symbol != 0)
			{
				// �������� �� ��������� ����� ������� (\)
				bool win = true;
				for (int i = 1; i < winLength; ++i)
				{
					if (gameBoard[startIndex + i * (size + 1)] != symbol)
					{
						win = false;
						break;
					}
				}
				if (win)
					return symbol;

				// �������� �� ��������� ������ ������ (/)
				win = true;
				for (int i = 1; i < winLength; ++i)
				{
					if (gameBoard[startIndex + i * (size - 1)] != symbol)
					{
						win = false;
						break;
					}
				}
				if (win)
					return symbol;
			}
		}
	}

	// ����� (���� ��� ������ ���������)
	bool draw = true;
	for (int i = 0; i < size * size; ++i)
	{
		if (gameBoard[i] == 0)
		{
			draw = false;
			break;
		}
	}
	if (draw)
		return 3;

	// ��� ����������
	return 0;
}


void ShowTurn(HWND hWnd, HDC hdc)
{
	RECT rc;

	static const WCHAR szTurn1[] = L"�����: ������ 1";
	static const WCHAR szTurn2[] = L"�����: ������ 2";

	const WCHAR* pszTurnText = (playerTurn == 1) ? szTurn1 : szTurn2;

	switch (winner)
	{
	case 0: //����������� ���
		pszTurnText = (playerTurn == 1) ? szTurn1 : szTurn2;
		break;
	case 1: //������� 1 ������
		pszTurnText = L"������� 1 ������!";
		break;
	case 2: //������� 2 ������
		pszTurnText = L"������� 2 ������!";
		break;
	case 3: //ͳ���
		pszTurnText = L"ͳ���!";
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

	/*for (int i = 0; i < (gameBoard.size() - 1); ++i)
	{
		if (GetCellRect(hWnd, wins[i], &rcWin)) //���� ������ �������� � � ����� ��������� �� �� ��� ������� ����������� 
		{
			FillRect(hdc, &rcWin, hbr1);
			DrawIconCentered(hdc, &rcWin, (winner == 1) ? hIcon1 : hIcon2);
		}
	}*/
}

// FUNCTION: WndProc (HWND, unsigned, WORD, LONG)
// ³����� ���������. ������ � �������� �� �����������, �� ��������� � �������

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE: 				//����������� ��������� ��� �������� ����
		{
			hbr1 = CreateSolidBrush(RGB(255, 0, 0));
			hbr2 = CreateSolidBrush(RGB(0, 0, 255));

			//������������ ������ �������
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
					int ret = MessageBox(hWnd, L"�� ����� ������ ������ ���� ���?", L"New Game", MB_YESNO | MB_ICONQUESTION);
				
					if (ret == IDYES)
					{
						//������� ���� ���
						playerTurn = 1;
						winner = 0;
						gameBoard.assign(gameBoard.size(), 0);
						
						InvalidateRect(hWnd, NULL, TRUE); 
						UpdateWindow(hWnd);
					}
				}
				break;
			case IDM_SIZEGAMEBOARD:
				{
					DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOGSIZEGAMEBOARD), hWnd, DlgProcSBG, 0);
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

				//���������� ������� �� ��������
				if (index != -1)
				{
					RECT rcCell;
					if ((0 == gameBoard[index]) && GetCellRect(hWnd, index, &rcCell))
					{
						gameBoard[index] = playerTurn;

						//FillRect(hdc, &rcCell, (playerTurn == 2) ? hbr2 : hbr1); 
						//DrawIcon(hdc, rcCell.left, rcCell.top, (playerTurn == 1) ? hIcon1 : hIcon2);
						DrawIconCentered(hdc, &rcCell, (playerTurn == 1) ? hIcon1 : hIcon2);

						//�������� �� ���������
						int minLength = numCells > 5 ? 5 : 3;
						winner = GetWinner(numCells, minLength);
						//winner = 0;
						if (winner == 1 || winner == 2)
						{
							ShowWinner(hWnd, hdc);

							//���� ���������
							MessageBox(
								hWnd, 
								(winner == 1) ? L"������� 1 ������!" : L"������� 2 ������!", 
								L"��������!", 
								MB_OK | MB_ICONINFORMATION);
							playerTurn = 0;
						}
						else if (winner == 3)
						{
							MessageBox(
								hWnd,
								L"��������� ����.",
								L"ͳ���!",
								MB_OK | MB_ICONEXCLAMATION);
						}
						else if (winner == 0)
						{
							playerTurn = (playerTurn == 2) ? 1 : 2;
						}

						//³���������� ����� �� ��� 
						ShowTurn(hWnd, hdc);
					}
				}
				ReleaseDC(hWnd, hdc);
			}
		
		}
		break;
	case WM_GETMINMAXINFO: // ̳�������� ����� ����
		{
			MINMAXINFO* pMinMax = (MINMAXINFO*)lParam;

			pMinMax->ptMinTrackSize.x = CELL_SIZE * (numCells + 2);
			pMinMax->ptMinTrackSize.y = CELL_SIZE * (numCells + 4);
		}
		break;
	case WM_PAINT: 				//������������ ����
		hdc = BeginPaint(hWnd, &ps); 	//������ ��������� ����	
		
		RECT rc;

		if (GetGameBoardRect(hWnd, &rc))
		{
			RECT rcClient;

			//³���������� ������ �� �����
			if (GetClientRect(hWnd, &rcClient))
			{
				const WCHAR szPLayer1[] = L"Player 1";
				const WCHAR szPLayer2[] = L"Player 2";

				SetBkMode(hdc, TRANSPARENT);

				//��������� ������
				TextOut(hdc, 16, 16, szPLayer1, lstrlen(szPLayer1));
				DrawIcon(hdc, 24, 34, hIcon1);
				TextOut(hdc, rcClient.right - 82, 16, szPLayer2, lstrlen(szPLayer2));
				DrawIcon(hdc, rcClient.right - 74, 34, hIcon2);

				//³���������� ����� �� ��� 
				ShowTurn(hWnd, hdc);
			}

			FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
			//Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);

			for (int i = 0; i < numCells + 1; i++)
			{
				//��������� ������������ ���
				DrawLine(hdc, rc.left + CELL_SIZE * i, rc.top, rc.left + CELL_SIZE * i, rc.bottom);
				//��������� �������������� ���
				DrawLine(hdc, rc.left, rc.top + CELL_SIZE * i, rc.right, rc.top + CELL_SIZE * i);
			}

			//��������� ��� �������� �������
			RECT rcCell;

			for (int i = 0; i < gameBoard.size(); ++i)
			{
				if ((0 != gameBoard[i]) && GetCellRect(hWnd, i, &rcCell))
				{
					//FillRect(hdc, &rcCell, (gameBoard[i] == 2) ? hbr2 : hbr1);
					DrawIconCentered(hdc, &rcCell, (gameBoard[i] == 1) ? hIcon1 : hIcon2);
				}
			}

			if (winner == 1 || winner == 2)
			{
				//³���������� ���������
				ShowWinner(hWnd, hdc);
			}
		}

		EndPaint(hWnd, &ps); 		//�������� ��������� ����	
		break;

	case WM_DESTROY: 				//���������� ������
		DeleteObject(hbr1);
		DeleteObject(hbr2);
		DestroyIcon(hIcon1);
		DestroyIcon(hIcon2);
		PostQuitMessage(0);
		break;
	default:
		//������� ����������, �� �� �������� ������������
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK DlgProcSBG(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) //ĳ������� ���� ��� ������� ������ �������� ����
{
	switch (message)
	{
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case ID_OK:
			{
				HWND hEdit = GetDlgItem(hWnd, IDC_EDIT);

				//��������� ������ � �������� "edit"
				wchar_t text[256];
				GetWindowText(hEdit, text, sizeof(text) / sizeof(text[0]));

				//������������ ������ � �����
				int number = _wtoi(text);

				//�������� �������� ����� (3-25)
				if (number < 3 || number > 25)
				{
					MessageBox(hWnd, L"����� ������� ���� � ������� �� 3 �� 25!", L"�������", MB_OK | MB_ICONERROR);
				}
				else
				{
					numCells = number; //������� ����� ����
					gameBoard.resize((numCells * numCells)); //���� ������ ������ ��� �������� ����
					playerTurn = 1; //������� ������ �� ������� 1
					winner = 0; //��������� ���� �� ����
					gameBoard.assign(gameBoard.size(), 0); //������� ����� �������� ����

					EndDialog(hWnd, 0);

					//��������� ���� ���� ���� ������ �������� ����
					InvalidateRect(GetParent(hWnd), NULL, TRUE);
					UpdateWindow(GetParent(hWnd));
				}
			}
			break;
		case ID_CANCEL:
			EndDialog(hWnd, 0);
			break;
		}
	}
		break;
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		break;
	default:
		return FALSE;
	}
	return FALSE;
}