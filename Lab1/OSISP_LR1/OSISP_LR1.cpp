#include <windows.h>

// Глобальные переменные
HINSTANCE hInst;
LPCTSTR szWindowClass = L"ПримерОкна";
LPCTSTR szTitle = L"Простое оконное приложение";

// Объявление функции обработки сообщений
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Тип для определения фигуры для рисования
enum ShapeType {
    CIRCLE,
    SQUARE
};

// Текущая выбранная фигура
ShapeType currentShape = CIRCLE;

// Текущий выбранный цвет и размер фигуры
COLORREF currentColor = RGB(0, 0, 255); // Синий цвет по умолчанию
int shapeSize = 100; // Размер фигур по умолчанию

// Текущая кисть для рисования
HBRUSH brush;

// Новые идентификаторы для кнопок
#define IDC_BLUE 101
#define IDC_RED 102
#define IDC_SMALL 103
#define IDC_LARGE 104

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    hInst = hInstance;

    // Регистрация класса окна
    WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex)) {
        MessageBox(NULL, L"Не удалось зарегистрировать класс окна.", L"Ошибка", MB_ICONERROR);
        return 1;
    }

    // Создание окна
    HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

    if (!hWnd) {
        MessageBox(NULL, L"Не удалось создать окно.", L"Ошибка", MB_ICONERROR);
        return 1;
    }

    // Создание кнопок для выбора фигуры
    CreateWindow(L"BUTTON", L"Круг", WS_CHILD | WS_VISIBLE, 10, 10, 80, 30, hWnd, (HMENU)CIRCLE, hInstance, NULL);
    CreateWindow(L"BUTTON", L"Квадрат", WS_CHILD | WS_VISIBLE, 100, 10, 80, 30, hWnd, (HMENU)SQUARE, hInstance, NULL);

    // Создание кнопок для изменения цвета и размера фигуры
    CreateWindow(L"BUTTON", L"Синий", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 10, 50, 80, 30, hWnd, (HMENU)IDC_BLUE, hInstance, NULL);
    CreateWindow(L"BUTTON", L"Красный", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 100, 50, 80, 30, hWnd, (HMENU)IDC_RED, hInstance, NULL);
    CreateWindow(L"BUTTON", L"Маленький", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 10, 90, 80, 30, hWnd, (HMENU)IDC_SMALL, hInstance, NULL);
    CreateWindow(L"BUTTON", L"Большой", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 100, 90, 80, 30, hWnd, (HMENU)IDC_LARGE, hInstance, NULL);

    // Инициализация кисти
    brush = CreateSolidBrush(currentColor);

    // Отображение окна
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Основной цикл сообщений
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

// Функция обработки сообщений
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    HDC hdc;

    hdc = BeginPaint(hWnd, &ps);
    RECT rect;
    GetClientRect(hWnd, &rect);
    int centerX = (rect.right - rect.left) / 2;
    int centerY = (rect.bottom - rect.top) / 2;

    switch (message) {
    case WM_PAINT:

        if (currentShape == CIRCLE) {
            SelectObject(hdc, brush); // Выбор текущей кисти
            Ellipse(hdc, centerX - shapeSize, centerY - shapeSize, centerX + shapeSize, centerY + shapeSize);
        }
        else if (currentShape == SQUARE) {
            SelectObject(hdc, brush); // Выбор текущей кисти
            Rectangle(hdc, centerX - shapeSize, centerY - shapeSize, centerX + shapeSize, centerY + shapeSize);
        }
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        DeleteObject(brush); // Освобождение кисти
        PostQuitMessage(0);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case CIRCLE:
            currentShape = CIRCLE;
            InvalidateRect(hWnd, NULL, TRUE);
            break;

        case SQUARE:
            currentShape = SQUARE;
            InvalidateRect(hWnd, NULL, TRUE);
            break;

        case IDC_BLUE:
            currentColor = RGB(0, 0, 255); // Синий
            DeleteObject(brush); // Освобождение предыдущей кисти
            brush = CreateSolidBrush(currentColor); // Создание новой кисти
            InvalidateRect(hWnd, NULL, TRUE);
            break;

        case IDC_RED:
            currentColor = RGB(255, 0, 0); // Красный
            DeleteObject(brush); // Освобождение предыдущей кисти
            brush = CreateSolidBrush(currentColor); // Создание новой кисти
            InvalidateRect(hWnd, NULL, TRUE);
            break;

        case IDC_SMALL:
            shapeSize = 50; // Установка маленького размера
            InvalidateRect(hWnd, NULL, TRUE);
            break;

        case IDC_LARGE:
            shapeSize = 100; // Установка большого размера
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        }
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
