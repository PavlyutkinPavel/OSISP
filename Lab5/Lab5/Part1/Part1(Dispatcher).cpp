#include <windows.h>
#include <string>
#include <iostream>

const int BUFFER_SIZE = 256;
const LPCWSTR PIPE_NAME = L"\\\\.\\pipe\\MyNamedPipe";

void DispatchTask(HANDLE hPipe, const char* taskData) {
    // Конвертация char[] в std::wstring
    std::wstring wTaskData = std::wstring(taskData, taskData + strlen(taskData));

    DWORD bytesWritten;
    if (!WriteFile(hPipe, wTaskData.c_str(), (wTaskData.size() + 1) * sizeof(wchar_t), &bytesWritten, nullptr)) {
        MessageBox(nullptr, L"Error writing to pipe", L"Error", MB_ICONERROR);
        return;
    }

    wchar_t resultData[BUFFER_SIZE];
    DWORD bytesRead;
    if (!ReadFile(hPipe, resultData, BUFFER_SIZE, &bytesRead, nullptr)) {
        MessageBox(nullptr, L"Error reading from pipe", L"Error", MB_ICONERROR);
        return;
    }

    // Конвертация wchar_t[] в std::wstring
    std::wstring wResultData(resultData, resultData + bytesRead / sizeof(wchar_t));

    // Формирование строки результата
    std::wstring resultString = L"Dispatcher received result: " + wResultData;

    MessageBox(nullptr, resultString.c_str(), L"Result", MB_ICONINFORMATION);

    CloseHandle(hPipe);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    while (true) {
        HANDLE hNamedPipe = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, BUFFER_SIZE, BUFFER_SIZE, 0, nullptr);

        if (hNamedPipe == INVALID_HANDLE_VALUE) {
            MessageBox(nullptr, L"Error creating named pipe", L"Error", MB_ICONERROR);
            return 1;
        }

        MessageBox(nullptr, L"Dispatcher is waiting for connections...", L"Info", MB_ICONINFORMATION);

        if (ConnectNamedPipe(hNamedPipe, nullptr)) {
            const char* taskData = "YourTaskDataHere";
            DispatchTask(hNamedPipe, taskData);
        }
        else {
            MessageBox(nullptr, L"Error connecting to named pipe", L"Error", MB_ICONERROR);
        }

        CloseHandle(hNamedPipe);
    }

    return 0;
}
