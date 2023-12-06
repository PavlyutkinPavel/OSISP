#include <windows.h>
#include <string>


const int BUFFER_SIZE = 256;
const LPCWSTR PIPE_NAME = L"\\\\.\\pipe\\MyNamedPipe";

void ProcessTask(HANDLE hPipe) {
    char buffer[BUFFER_SIZE];
    DWORD bytesRead;

    if (ReadFile(hPipe, buffer, BUFFER_SIZE, &bytesRead, nullptr)) {
        // Печать прочитанных данных
        buffer[bytesRead] = '\0';
        MessageBoxA(nullptr, buffer, "Read Data", MB_ICONINFORMATION);

        // Можете выполнить здесь обработку данных по заданию

        if (!WriteFile(hPipe, buffer, bytesRead, nullptr, nullptr)) {
            MessageBox(nullptr, L"Error writing to pipe", L"Error", MB_ICONERROR);
        }
        else {
            MessageBoxA(nullptr, buffer, "Written Data", MB_ICONINFORMATION);
        }
    }
    else {
        MessageBox(nullptr, L"Error reading from pipe", L"Error", MB_ICONERROR);
    }

    CloseHandle(hPipe);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    HANDLE hNamedPipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);

    if (hNamedPipe == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        MessageBox(nullptr, (L"Error opening named pipe. Error code: " + std::to_wstring(error)).c_str(), L"Error", MB_ICONERROR);
        return 1;
    }
    else {
        MessageBox(nullptr, L"Named pipe opened successfully", L"Success", MB_ICONINFORMATION);
    }

    ProcessTask(hNamedPipe);

    CloseHandle(hNamedPipe);

    return 0;
}
