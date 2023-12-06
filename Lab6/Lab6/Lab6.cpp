#include <windows.h>
#include <thread>
#include <vector>
#include <mutex>
#include <sstream>
#include <iomanip>

const int num_philosophers = 5;
std::vector<std::mutex> forks(num_philosophers);
std::vector<std::thread> philosopher_threads;
HWND hwndEdit;  // Дескриптор текстового окна для вывода информации

int num_meals = 0;
int max_meals = 10;  // Максимальное количество приемов пищи для каждого философа
std::vector<int> meals_eaten(num_philosophers, 0);

void think(int philosopher_id) {
    std::wstringstream ss;
    ss << L"Philosopher " << philosopher_id << L" is thinking.\r\n";
    SendMessageW(hwndEdit, EM_SETSEL, -1, -1);
    SendMessageW(hwndEdit, EM_REPLACESEL, 0, reinterpret_cast<LPARAM>(ss.str().c_str()));
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 1000));
}

void eat(int philosopher_id) {
    std::wstringstream ss;
    ss << L"Philosopher " << philosopher_id << L" is eating.\r\n";
    SendMessageW(hwndEdit, EM_SETSEL, -1, -1);
    SendMessageW(hwndEdit, EM_REPLACESEL, 0, reinterpret_cast<LPARAM>(ss.str().c_str()));

    // Счетчик приемов пищи
    meals_eaten[philosopher_id]++;
    num_meals++;

    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 1000));
}

void philosopher(int philosopher_id) {
    while (meals_eaten[philosopher_id] < max_meals) {
        think(philosopher_id);

        std::unique_lock<std::mutex> left_fork(forks[philosopher_id]);
        std::unique_lock<std::mutex> right_fork(forks[(philosopher_id + 1) % num_philosophers]);

        eat(philosopher_id);
    }
}

void printStatistics() {
    std::wstringstream ss;
    ss << L"\r\nStatistics:\r\n";
    for (int i = 0; i < num_philosophers; ++i) {
        ss << L"Philosopher " << i << L" ate " << meals_eaten[i] << L" meals.\r\n";
    }
    ss << L"Total meals eaten: " << num_meals << L"\r\n";

    SendMessageW(hwndEdit, EM_SETSEL, -1, -1);
    SendMessageW(hwndEdit, EM_REPLACESEL, 0, reinterpret_cast<LPARAM>(ss.str().c_str()));
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        hwndEdit = CreateWindowExW(
            WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
            10, 10, 400, 300,
            hwnd, nullptr, GetModuleHandle(nullptr), nullptr);

        for (int i = 0; i < num_philosophers; ++i) {
            philosopher_threads.emplace_back(philosopher, i);
        }

        // Добавление кнопки для вывода статистики
        CreateWindowW(
            L"BUTTON", L"Show Statistics", WS_VISIBLE | WS_CHILD,
            10, 320, 120, 30,
            hwnd, reinterpret_cast<HMENU>(1), GetModuleHandle(nullptr), nullptr);
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1) {
            // Обработка нажатия кнопки вывода статистики
            printStatistics();
        }
        break;

    case WM_DESTROY:
        for (auto& thread : philosopher_threads) {
            thread.join();
        }
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    srand(static_cast<unsigned>(time(nullptr)));

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"PhilosopherClass";

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0,
        L"PhilosopherClass",
        L"Philosopher Dining",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        600,
        400,
        nullptr,
        nullptr,
        hInstance,
        nullptr);

    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}
