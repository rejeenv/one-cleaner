#include <windows.h>
#include <iostream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

// GUI Element IDs
#define BUTTON_CLEAN 101
#define TEXT_STATIC  102

// Function Prototype
void deleteTempFiles(const fs::path& directory);

// Global Variables
HFONT hFont, hFontSmall;
HBRUSH hBrushButton;
HWND hStaticText;

// Window Dimensions
const int WINDOW_WIDTH = 400;
const int WINDOW_HEIGHT = 250;
const int BUTTON_WIDTH = 200;
const int BUTTON_HEIGHT = 60;

// Window Procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    // Register Window Class
    WNDCLASSEX wc = { };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("OneCleanerWindowClass");
    wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(30, 30, 30));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, TEXT("Failed to register window class!"), TEXT("Error"), MB_ICONERROR);
        return -1;
    }

    // Center the window on the screen
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int posX = (screenWidth - WINDOW_WIDTH) / 2;
    int posY = (screenHeight - WINDOW_HEIGHT) / 2;

    // Create Main Window
    HWND hwnd = CreateWindowEx(
        0, wc.lpszClassName, TEXT("One Cleaner"),
        WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,
        posX, posY, WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) {
        MessageBox(NULL, TEXT("Failed to create window!"), TEXT("Error"), MB_ICONERROR);
        return -2;
    }

    // Create Fonts
    hFont = CreateFont(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial"));

    hFontSmall = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial"));

    // Create Button (centered)
    HWND hButton = CreateWindow(
        TEXT("BUTTON"), TEXT("Clean TEMP"),
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
        (WINDOW_WIDTH - BUTTON_WIDTH) / 2, (WINDOW_HEIGHT - BUTTON_HEIGHT) / 2,
        BUTTON_WIDTH, BUTTON_HEIGHT,
        hwnd, (HMENU)BUTTON_CLEAN, hInstance, NULL
    );

    SendMessage(hButton, WM_SETFONT, (WPARAM)hFont, TRUE);

    // ✅ CREATE STATIC TEXT ✅
    hStaticText = CreateWindowW(
        L"STATIC", L"dsc.gg/hardhvh",
        WS_CHILD | WS_VISIBLE | SS_RIGHT,
        WINDOW_WIDTH - 130, WINDOW_HEIGHT - 25, 120, 20,
        hwnd, (HMENU)TEXT_STATIC, hInstance, NULL
    );

    SendMessage(hStaticText, WM_SETFONT, (WPARAM)hFontSmall, TRUE);

    // Create Brushes
    hBrushButton = CreateSolidBrush(RGB(20, 50, 100)); // Dark Blue

    // Show Window
    ShowWindow(hwnd, nShowCmd);
    UpdateWindow(hwnd);

    // Message Loop
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

// Window Procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
    switch (message) {
    case WM_COMMAND:
        if (LOWORD(wparam) == BUTTON_CLEAN) {
            char tempDir[MAX_PATH];
            if (GetEnvironmentVariableA("TEMP", tempDir, MAX_PATH) > 0) {
                fs::path targetDir(tempDir);

                int confirm = MessageBox(hwnd, TEXT("Are you sure you want to clean the TEMP folder?"),
                    TEXT("Confirmation"), MB_YESNO | MB_ICONQUESTION);
                if (confirm == IDYES) {
                    deleteTempFiles(targetDir);
                    MessageBox(hwnd, TEXT("Cleaning completed!"), TEXT("Success"), MB_OK | MB_ICONINFORMATION);
                }
            }
            else {
                MessageBox(hwnd, TEXT("Failed to retrieve the TEMP folder path!"), TEXT("Error"), MB_ICONERROR);
            }
        }
        break;

    case WM_DRAWITEM: {
        LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lparam;
        if (pDIS->CtlID == BUTTON_CLEAN) {
            FillRect(pDIS->hDC, &pDIS->rcItem, hBrushButton);
            SetTextColor(pDIS->hDC, RGB(255, 255, 255));
            SetBkMode(pDIS->hDC, TRANSPARENT);

            DrawText(pDIS->hDC, TEXT("🧹 Clean TEMP"), -1, &pDIS->rcItem,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            return TRUE;
        }
    }
                    break;

    case WM_CTLCOLORSTATIC: {
        HDC hdcStatic = (HDC)wparam;
        if ((HWND)lparam == hStaticText) {
            SetTextColor(hdcStatic, RGB(180, 180, 180));
            SetBkMode(hdcStatic, TRANSPARENT);
            return (LRESULT)GetStockObject(NULL_BRUSH);
        }
    }
                          break;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        DeleteObject(hFont);
        DeleteObject(hFontSmall);
        DeleteObject(hBrushButton);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, message, wparam, lparam);
    }
    return 0;
}

// Function to delete TEMP files
void deleteTempFiles(const fs::path& directory) {
    try {
        if (!fs::exists(directory)) return;

        for (const auto& entry : fs::directory_iterator(directory)) {
            try {
                if (fs::is_regular_file(entry)) {
                    fs::remove(entry.path());
                }
                else if (fs::is_directory(entry)) {
                    deleteTempFiles(entry.path());
                    fs::remove(entry.path());
                }
            }
            catch (const std::exception&) {}
        }
    }
    catch (const std::exception&) {}
}
