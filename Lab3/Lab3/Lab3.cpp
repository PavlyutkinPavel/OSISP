#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <vector>
#include <thread>
#include <ctime>
#include <iomanip>
#include <sstream>

const int numThreads = 4;
const int matrixSize = 200;

struct ThreadInfo {
    std::thread thread;
    int priority;
    clock_t start_time;
    clock_t end_time;
    long long work_done;
    HWND hwnd;
};

std::vector<ThreadInfo> threads;
RECT columns[numThreads];
int columnWidth;

void MultiplyMatrices(int threadIndex) {
    int matrixA[matrixSize][matrixSize];
    int matrixB[matrixSize][matrixSize];
    int resultMatrix[matrixSize][matrixSize];

    for (int i = 0; i < matrixSize; ++i) {
        for (int j = 0; j < matrixSize; ++j) {
            matrixA[i][j] = rand() % 100;
            matrixB[i][j] = rand() % 100;
        }
    }

    clock_t start_time = clock();

    for (int i = 0; i < matrixSize; ++i) {
        for (int j = 0; j < matrixSize; ++j) {
            resultMatrix[i][j] = 0;
            for (int k = 0; k < matrixSize; ++k) {
                resultMatrix[i][j] += matrixA[i][k] * matrixB[k][j];
            }
        }
    }

    clock_t end_time = clock();
    long long work_done = (end_time - start_time);

    threads[threadIndex].start_time = start_time;
    threads[threadIndex].end_time = end_time;
    threads[threadIndex].work_done = work_done;

    PostMessage(threads[threadIndex].hwnd, WM_USER, 0, threadIndex);
}

void DisplayInfo(HWND hwnd, int threadIndex) {
    std::wstringstream wss;
    wss << L"Thread " << threadIndex << L" (" << threads[threadIndex].priority << L")\n";
    wss << L"Start time: " << threads[threadIndex].start_time << L" ms\n";
    wss << L"End time: " << threads[threadIndex].end_time << L" ms\n";
    wss << L"Work done: " << threads[threadIndex].work_done << L" ms";

    RECT columnRect = columns[threadIndex];
    columnRect.top += 20;
    columnRect.left += 10;
    columnRect.right -= 10;

    HDC hdc = GetDC(hwnd);
    SetBkMode(hdc, TRANSPARENT);

    DrawText(hdc, wss.str().c_str(), -1, &columnRect, DT_LEFT | DT_TOP);

    ReleaseDC(hwnd, hdc);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_CREATE) {
        srand(static_cast<unsigned>(time(nullptr)));
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        columnWidth = (clientRect.right - clientRect.left) / numThreads;

        for (int i = 0; i < numThreads; ++i) {
            columns[i] = { i * columnWidth, 0, (i + 1) * columnWidth, clientRect.bottom };
            threads.push_back({ std::thread(MultiplyMatrices, i), THREAD_PRIORITY_NORMAL, 0, 0, 0, hwnd });
        }

        threads[0].priority = THREAD_PRIORITY_LOWEST;
        threads[1].priority = THREAD_PRIORITY_BELOW_NORMAL;
        threads[2].priority = THREAD_PRIORITY_NORMAL;
        threads[3].priority = THREAD_PRIORITY_HIGHEST;

        for (int i = 0; i < numThreads; ++i) {
            SetThreadPriority(threads[i].thread.native_handle(), threads[i].priority);
        }

        return 0;
    }

    if (msg == WM_CLOSE) {
        for (int i = 0; i < numThreads; ++i) {
            threads[i].thread.join();
        }

        PostQuitMessage(0);
        return 0;
    }

    if (msg == WM_USER) {
        int threadIndex = lParam;
        DisplayInfo(hwnd, threadIndex);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
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
    wc.lpszClassName = _T("ThreadPriorityDemo");
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, _T("Failed to register the window class"), _T("Error"), MB_ICONERROR);
        return -1;
    }

    HWND hwnd = CreateWindow(_T("ThreadPriorityDemo"), _T("Thread Priority Demo"), WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, NULL, hInstance, NULL);

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
