#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <meagle.h>

#define IDC_MEVIEW 101
#define IDC_MEEYEVIEW 102

HWND meagle_hwnd;
HWND meagleeye_hwnd;

static int mEagleProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_CREATE:
            meagle_hwnd = CreateWindow (MEAGLE_CTRL,
                    "",
                    WS_VISIBLE,
                    IDC_MEVIEW,
                    0, 0, 600, 480, hWnd, 0);  
            meagleeye_hwnd = CreateWindow (MEAGLEEYE_CTRL,
                                      "",
                                      WS_VISIBLE,
                                      IDC_MEEYEVIEW,
                                      600, 480, 150, 150, hWnd, 0);

            char **layers_name = NULL;
            SendMessage(meagle_hwnd, ME_OPEN_MAP, 0, (LPARAM)"./res/map");
            SendMessage(meagle_hwnd, ME_GET_MAPINFO, 0, (LPARAM)&layers_name);
            SendMessage(meagle_hwnd, ME_LOAD_MAP, 0, (LPARAM)layers_name);
            SendMessage(meagleeye_hwnd, ME_ADD_EYE, 0, meagle_hwnd);
            SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_DRAG, 0);
            if(layers_name)
                free(layers_name);
            break;
        case MSG_DESTROY:
            DestroyAllControls (hWnd);
            return 0;

        case MSG_CLOSE:
            DestroyMainWindow (hWnd);
            PostQuitMessage (hWnd);
            return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;  

    MAINWINCREATE CreateInfo;


    /*Register map control*/
    RegisterMEagleControl();
    RegisterMEagleEyeControl ();

    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "mEagle";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = mEagleProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = 750;
    CreateInfo.by = 630;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

    hMainWnd = CreateMainWindow (&CreateInfo);

    if (hMainWnd == HWND_INVALID)
        return -1;

    ShowWindow(hMainWnd, SW_SHOWNORMAL);
    SetWindowBkColor (hMainWnd, RGB2Pixel (HDC_SCREEN, 230, 246, 255));
    InvalidateRect (hMainWnd, NULL, TRUE);

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    UnregisterMEagleControl ();
    UnregisterMEagleEyeControl ();

    MainWindowThreadCleanup (hMainWnd);

    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

