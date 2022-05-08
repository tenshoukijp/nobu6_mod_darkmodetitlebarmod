// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include <windows.h>
#include "win32_darkmode.h"

HWND hTenshouWnd = NULL;
bool isDarkModeUable = false;

extern void TryDarkMode();

HMODULE hSelfDllModule = NULL;
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        hSelfDllModule = hModule;

        TryDarkMode();
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

bool isDarkModeUsableWindowsVersion() {
    auto RtlGetNtVersionNumbers = reinterpret_cast<fnRtlGetNtVersionNumbers>(GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlGetNtVersionNumbers"));
    if (RtlGetNtVersionNumbers)
    {
        DWORD major, minor;
        RtlGetNtVersionNumbers(&major, &minor, &g_buildNumber);
        g_buildNumber &= ~0xF0000000;
        if (major >= 10 && CheckBuildNumber(g_buildNumber)) {
            return true;
        }
    }

    return false;
}

void TryDarkMode() {
    if (!isDarkModeUable) {
        isDarkModeUable = isDarkModeUsableWindowsVersion();
    }

    if (isDarkModeUable) {
        if (!hTenshouWnd) {
            InitDarkMode();

            // 天翔記のウィンドウハンドル
            hTenshouWnd = FindWindow(L"Tenshouki95", NULL);
        }

        if (hTenshouWnd) {
            // _AllowDarkModeForWindow(hTenshouWnd, true);
            RefreshTitleBarThemeColor(hTenshouWnd);
        }
    }
}

void ReplaceDarkModeIcon() {
    HICON hIcon = LoadIcon(hSelfDllModule, MAKEINTRESOURCE(IDI_ICON1));

    if (hTenshouWnd) {
        //Change both icons to the same icon handle.
        SendMessage(hTenshouWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
        SendMessage(hTenshouWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

        //This will ensure that the application icon gets changed too.
        SendMessage(GetWindow(hTenshouWnd, GW_OWNER), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
        SendMessage(GetWindow(hTenshouWnd, GW_OWNER), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    }
}

void WINAPI OnInitialize(HANDLE hCallDLLHandle) {
    TryDarkMode();

    // アイコンもダークモードにしたいという人は、このコメントアウトを外してコンパイルすること。
    // ReplaceDarkModeIcon();
}

void WINAPI OnWmSize() {
    TryDarkMode();
}

void WINAPI OnAfterChangeWindowSize() {
    TryDarkMode();
}

void WINAPI OnFinalize() {

}