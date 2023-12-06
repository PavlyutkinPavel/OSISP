#include <windows.h>
#include <string>
#include <fstream>

HHOOK g_hook;
HWND g_targetWindow;
std::wofstream g_logFile;

LRESULT CALLBACK HookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        CWPSTRUCT* msg = reinterpret_cast<CWPSTRUCT*>(lParam);

        if (msg->hwnd == g_targetWindow) {
            std::wstring message = L"Message: " + std::to_wstring(msg->message) +
                L", wParam: " + std::to_wstring(msg->wParam) +
                L", lParam: " + std::to_wstring(msg->lParam) + L"\n";

            g_logFile << message;
        }
    }

    return CallNextHookEx(g_hook, nCode, wParam, lParam);
}

HWND FindNotepadWindow() {
    return FindWindowW(L"Notepad", nullptr);
}

void SetHook(HWND targetWindow) {
    g_targetWindow = targetWindow;
    g_hook = SetWindowsHookEx(WH_CALLWNDPROC, HookCallback, nullptr, GetCurrentThreadId());

    if (g_hook == nullptr) {
        MessageBoxW(nullptr, L"Failed to set hook.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    g_logFile.open(L"WindowMessages.log", std::ios::out | std::ios::app);
    if (!g_logFile.is_open()) {
        MessageBoxW(nullptr, L"Failed to open log file.", L"Error", MB_OK | MB_ICONERROR);
        UnhookWindowsHookEx(g_hook);
        return;
    }
}

void Unhook() {
    if (g_logFile.is_open()) {
        g_logFile.close();
    }

    UnhookWindowsHookEx(g_hook);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    if (CreateProcessW(L"C:\\Windows\\System32\\notepad.exe", nullptr, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
        Sleep(2000);

        HWND targetWindow = FindNotepadWindow();

        if (targetWindow != nullptr) {
            SetHook(targetWindow);

            MSG msg;
            while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }

            Unhook();
        }
        else {
            MessageBoxW(nullptr, L"Failed to find Notepad window.", L"Error", MB_OK | MB_ICONERROR);
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else {
        MessageBoxW(nullptr, L"Failed to create process.", L"Error", MB_OK | MB_ICONERROR);
    }

    return 0;
}
