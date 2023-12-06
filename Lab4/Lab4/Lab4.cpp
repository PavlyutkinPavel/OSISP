#include <windows.h>
#include <tchar.h>
#include <string>
#include <fstream>
#include <thread>

#define ID_FILE_OPEN 1001
#define ID_PROCESS_FILE 1002

/*
Этот код представляет собой простое Windows-приложение с использованием WinAPI, которое выполняет следующие функции:

1. Открывает окно приложения с текстовым полем (многострочным редактором) и двумя кнопками: "Open File" и "Process File".

2. При нажатии на "Open File" открывается стандартное диалоговое окно для выбора текстового файла. После выбора файла его содержимое читается с использованием `std::wifstream` и выводится в текстовом поле в окне приложения.

3. При нажатии на "Process File" также открывается диалоговое окно для выбора текстового файла. Однако, в этом случае обработка файла выполняется асинхронно в новом потоке. Для этого используется `std::thread`, который запускает функцию `ProcessFileAsync`, которая читает содержимое файла и выводит его в текстовое поле.

4. Окно приложения содержит обработчик сообщений `WndProc`, который обрабатывает различные события, такие как нажатие кнопок, закрытие окна и другие.

Этот код просто демонстрирует базовую функциональность Windows-приложения для открытия и обработки текстовых файлов, как с использованием синхронного, так и с асинхронного подхода.
*/

HWND g_hEdit = NULL;

bool OpenFile(std::wstring& filePath) {
    OPENFILENAME ofn;
    TCHAR szFile[MAX_PATH] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = _T("Text Files\0*.txt\0All Files\0*.*\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn)) {
        filePath = ofn.lpstrFile;
        return true;
    }
    return false;
}

void ProcessFileAsync(const std::wstring& filePath) {
    std::wifstream file(filePath);
    if (file.is_open()) {
        std::wstring content;
        std::wstring line;
        while (std::getline(file, line)) {
            content += line + L"\r\n";
        }
        file.close();

        SetWindowText(g_hEdit, content.c_str());
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        g_hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), _T(""), WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL, 10, 10, 600, 400, hWnd, NULL, GetModuleHandle(NULL), NULL);

        CreateWindow(_T("BUTTON"), _T("Open File"), WS_CHILD | WS_VISIBLE, 10, 420, 100, 30, hWnd, (HMENU)ID_FILE_OPEN, GetModuleHandle(NULL), NULL);
        CreateWindow(_T("BUTTON"), _T("Process File"), WS_CHILD | WS_VISIBLE, 120, 420, 100, 30, hWnd, (HMENU)ID_PROCESS_FILE, GetModuleHandle(NULL), NULL);

        return 0;
    case WM_COMMAND:
        if (LOWORD(wParam) == ID_FILE_OPEN) {
            std::wstring filePath;
            if (OpenFile(filePath)) {
                SetWindowText(g_hEdit, _T("")); // Очистим текстовое поле
                ProcessFileAsync(filePath);
            }
        }
        else if (LOWORD(wParam) == ID_PROCESS_FILE) {
            // Вызываем асинхронную функцию для обработки файла
            std::wstring filePath;
            if (OpenFile(filePath)) {
                std::thread(ProcessFileAsync, filePath).detach();
            }
        }
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
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
    wc.lpszClassName = _T("TextFileReader");
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, _T("Failed to register the window class"), _T("Error"), MB_ICONERROR);
        return -1;
    }

    HWND hWnd = CreateWindow(_T("TextFileReader"), _T("Text File Reader"), WS_OVERLAPPEDWINDOW, 100, 100, 640, 480, NULL, NULL, hInstance, NULL);

    if (!hWnd) {
        MessageBox(NULL, _T("Failed to create the window"), _T("Error"), MB_ICONERROR);
        return -1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
