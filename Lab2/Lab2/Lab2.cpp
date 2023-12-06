#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <string>

/*
Этот код представляет собой Windows-приложение, которое демонстрирует создание самовосстанавливающегося процесса. Вот пошаговое объяснение, что делает данный код:

1. Сначала определена структура `AppContext`, которая используется для хранения "рабочих" данных. В данном случае, у нас есть два целочисленных поля: `data1` и `data2`.

2. Создается указатель `g_Context` для хранения экземпляра структуры `AppContext`. Этот указатель будет использоваться для сохранения и восстановления контекста приложения.

3. `SaveContext()` - это функция, которая сохраняет текущие рабочие данные в экземпляре `AppContext`. В данном случае, устанавливаются значения `data1` и `data2` равными 100 и 200.

4. `RestoreContext()` - это функция для восстановления ранее сохраненных данных и выполнения действий для продолжения работы. Если контекст был сохранен (т.е., `g_Context` не равен `nullptr`), то данные восстанавливаются, и сообщение `MessageBox` выводит восстановленные значения `data1` и `data2`. После этого указатель `g_Context` освобождается (удаляется).

5. В функции `WndProc` (обработчик оконных сообщений) обрабатывается сообщение `WM_CLOSE`, которое генерируется при закрытии окна. В этом случае, вызывается `SaveContext()` для сохранения текущего контекста, а затем создается новое окно с тем же оконным классом (`SelfRestoringProcess`) и названием "Self-Restoring Process". После создания нового окна, вызывается `RestoreContext()` для восстановления контекста, а текущее окно завершает свою работу с помощью `PostMessage(hwnd, WM_QUIT, 0, 0)`.

6. В функции `WinMain` создается окно с определенным оконным классом, и запускается цикл обработки сообщений с помощью `GetMessage` и `DispatchMessage`.

Таким образом, приложение позволяет сохранить контекст, создать новое окно и восстановить контекст в новом окне при закрытии старого окна.
*/

// Структура для хранения полей наших "рабочих" данных
struct AppContext {
    int data1;
    int data2;
};

AppContext* g_Context = nullptr;

// Функция для сохранения контекста
void SaveContext() {
    g_Context = new AppContext;
    // Здесь сохраняем рабочие данные
    g_Context->data1 = 100; 
    g_Context->data2 = 200;
}

// Функция для восстановления контекста
void RestoreContext() {
    // Восстанавливайте свои рабочие данные и выполняйте необходимые действия
    if (g_Context) {
        std::string message = "Restored data1: " + std::to_string(g_Context->data1) + "\n";
        message += "Restored data2: " + std::to_string(g_Context->data2);
        MessageBoxA(NULL, message.c_str(), "Restored Context", MB_OK);
        delete g_Context;
        g_Context = nullptr;
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CLOSE:
    {
        SaveContext();

        // Создайте новое окно для продолжения работы
        WNDCLASS wc = { 0 };
        wc.lpszClassName = _T("SelfRestoringProcess");
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpfnWndProc = WndProc;
        RegisterClass(&wc);

        HWND newHwnd = CreateWindow(wc.lpszClassName, _T("Self-Restoring Process"), WS_OVERLAPPEDWINDOW, 0, 0, 640, 480, 0, 0, GetModuleHandle(NULL), 0);

        ShowWindow(newHwnd, SW_SHOW);
        UpdateWindow(newHwnd);

        // Восстановите контекст в новом окне
        RestoreContext();

        // Завершите текущее окно
        PostMessage(hwnd, WM_QUIT, 0, 0);
    }
    break;
    case WM_QUIT:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = _T("SelfRestoringProcess");
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, _T("Failed to register the window class"), _T("Error"), MB_ICONERROR);
        return -1;
    }

    HWND hwnd = CreateWindow(_T("SelfRestoringProcess"), _T("Self-Restoring Process"), WS_OVERLAPPEDWINDOW, 0, 0, 640, 480, NULL, NULL, hInstance, NULL);

    if (!hwnd) {
        MessageBox(NULL, _T("Failed to create the window"), _T("Error"), MB_ICONERROR);
        return -1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
