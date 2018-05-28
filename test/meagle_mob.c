#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <meagle.h>
#include "meagle_search.h"
#include "meagle_state.h"


#define IDT_PATH 100
extern PLOGFONT ctrl_font;
enum {
    IDC_MEVIEW = 101,       /* button */
    IDC_MEEYEVIEW,
    IDC_STATUS,
    IDC_NTL,
    IDC_NORTH,
    IDC_ZOOM_IN,
    IDC_ZOOM_OUT,
    IDC_ROTATE_LEFT,
    IDC_ROTATE_RIGHT,
    IDM_MAIN_FILE,          /* menu item */
    IDM_MAIN_VIEW,
    IDM_MAIN_DEMO,
    IDM_MAIN_HELP,
    IDM_FILE_OPENMAP,
    IDM_FILE_CLOSEMAP,
    IDM_FILE_EXIT,
    IDM_VIEW_ZOOMIN,
    IDM_VIEW_ZOOMOUT,
    IDM_VIEW_PAN,
    IDM_VIEW_ROTATE,
    IDM_VIEW_LAYER,
    IDM_VIEW_EYE,
    IDM_DEMO_MOVINGLOCALIZE,
    IDM_DEMO_PATHTRACING,
    IDM_DEMO_SEARCH,
    IDM_HELP_ABOUT,
    IDC_MBN
};

/* tool bar*/

#define TB_ITEMS  8            // bitmap count of toolbar every line  

enum {
    OP_BASE = 10,
    OP_GLOBAL,
    OP_ZOOMIN,
    OP_ZOOMOUT,
    OP_PAN,
    OP_RL,
    OP_RR,                            
    OP_PT,    //Path tracing
    OP_NORTH                            
};

#define G_PI    3.14159265358979323846E0

static const int ww=480;       //main window width
static const int wh=272;       //main window height

static const int mectlw = 390; //meagle control width
static const int mectlh = 272; //meagle control height

static const int tbx = 0;     //tool bar top left x coordinate
static const int tby = 0;    //tool bar top left y coordinate
static const int tbw = 35;    //tool bar width
static const int tbh = 272;     //tool bar height


HWND hMainWnd;  
HWND meagle_hwnd;
HWND ntb_hwnd;
HWND status_corner_hwnd;
HWND status_dis_hwnd;
HWND status_limit_hwnd;
HWND status_speed_hwnd;

BITMAP ntb_bitmap;
BITMAP load_bitmap;

static void set_other_item_unchecked (int nc)
{
    int i;
    NTBITEMINFO ntbii;
    for (i = OP_ZOOMIN; i <= OP_PAN; ++i)
    {
        memset (&ntbii, 0, sizeof(ntbii));
        ntbii.which = MTB_WHICH_FLAGS;
        if (i != nc)
            ntbii.flags &= ~NTBIF_CHECKED;
        else
            ntbii.flags |= NTBIF_CHECKED;
        SendMessage (ntb_hwnd, NTBM_SETITEM, i, (LPARAM) &ntbii);
    }
}

static void map_operating_proc (HWND hwnd, int id, int nc, DWORD add_data)
{
  double alpha = 0;
    switch (nc) {
        case OP_GLOBAL:
            SendMessage (meagle_hwnd, ME_GLOBAL, 0, 0);
            return;
        case OP_ZOOMOUT:
            SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_ZOOMOUT, 0);
            break;
        case OP_ZOOMIN:
            SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_ZOOMIN, 0);
            break;
        case OP_PAN:
            SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_DRAG, 0);
            break;
        case OP_RL:
            alpha = -G_PI/8;
            SendMessage (meagle_hwnd, ME_ROTATE, 0, (LPARAM)&alpha);
            break;

        case OP_RR:
            alpha = G_PI/8;
            SendMessage (meagle_hwnd, ME_ROTATE, 0, (LPARAM)&alpha);
            break;
        case OP_NORTH:
            SendMessage (meagle_hwnd, ME_NORTH, 0, 0);
            break;
	    
        case OP_PT:
            set_car_pos(mectlw/2,mectlh/2);
            tracing();
            break;            
    }

    if(nc>=OP_ZOOMIN && nc<=OP_PAN)
    {
        set_other_item_unchecked (nc);
    }
}

static void create_toolbar (HWND hWnd)
{
	NTBINFO ntb_info;
	NTBITEMINFO ntbii;
	int i;

	memset (&ntb_bitmap, 0, sizeof(ntb_bitmap));


	if (LoadBitmap (HDC_SCREEN, &ntb_bitmap, "res/toolbar.gif"))
	{
		fprintf (stderr, "Can not load the image for toolbar!\n");
	}


	ntb_info.nr_cells = TB_ITEMS;
	ntb_info.w_cell = 0;
	ntb_info.h_cell = 0;
	ntb_info.nr_cols = 0;
	ntb_info.image = &ntb_bitmap;

	ntb_hwnd = CreateWindow (CTRL_NEWTOOLBAR,
				 "",
				 WS_CHILD | WS_VISIBLE | NTBS_VERTICAL,
				 IDC_NTL,
				 tbx, tby, tbw, tbh,
				 hWnd,
				 (DWORD) &ntb_info);

	InvalidateRect (ntb_hwnd, NULL, TRUE);

	memset (&ntbii, 0, sizeof(ntbii));

	for (i = 0; i < TB_ITEMS; ++i)
	{
	  if (i == 0 || (i>3 &&i <= TB_ITEMS - 1) )		
			ntbii.flags = NTBIF_PUSHBUTTON;
		else
			ntbii.flags = NTBIF_CHECKBUTTON;
		ntbii.id = OP_BASE + i + 1;
		if (ntbii.id == OP_PAN)
			ntbii.flags |= NTBIF_CHECKED;
		ntbii.bmp_cell = i;
		SendMessage(ntb_hwnd, NTBM_ADDITEM, 0, (LPARAM)&ntbii);
	}

	SetNotificationCallback (ntb_hwnd, map_operating_proc); 
}

static void create_status_bar(HWND hWnd)
{
  	status_corner_hwnd = CreateWindow (CTRL_STATIC,
					  "",
					  WS_CHILD | WS_VISIBLE | SS_LEFT,
					  IDC_STATUS,
					  423, 30, 50, 20, hWnd, 0);

  	status_dis_hwnd = CreateWindow (CTRL_STATIC,
					  "",
					  WS_CHILD | WS_VISIBLE | SS_LEFT,
					  IDC_STATUS,
					  423, 80, 50, 20, hWnd, 0);

    status_limit_hwnd = CreateWindow (CTRL_STATIC,
					  "",
					  WS_CHILD | WS_VISIBLE | SS_LEFT,
					  IDC_STATUS,
					  423, 130, 50, 20, hWnd, 0);
    status_speed_hwnd = CreateWindow (CTRL_STATIC,
					  "",
					  WS_CHILD | WS_VISIBLE | SS_LEFT,
					  IDC_STATUS,
					  423, 180, 50, 20, hWnd, 0);
}

#define LINESIZE 50

static int mEagleProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
  HDC hdc;
    switch (message) {
        case MSG_CREATE:
	    create_toolbar(hWnd);
        create_status_bar(hWnd);
        meagle_hwnd = CreateWindow (MEAGLE_CTRL,
                                    "",
                                    WS_VISIBLE,
                                    IDC_MEVIEW,
                                    30, 0, mectlw, mectlh, hWnd, 0);  

        if (LoadBitmap (HDC_SCREEN, &load_bitmap, "res/load.gif"))
        {
            fprintf (stderr, "Can not load the image for load!\n");
            break;
        } 
 

	    SendMessage(meagle_hwnd, ME_LOAD_MEAGLE_MAP, 0, (LPARAM)"res/meagle.meg");
        break;

        case MSG_PAINT:
            hdc = BeginPaint (hWnd);
            SetBkMode (hdc, BM_TRANSPARENT);
            SetTextColor (hdc, RGB2Pixel(hdc, 0x17, 0x44, 0xC2));
            TextOut (hdc, 422, 10, "corner:");
            TextOut (hdc, 422, 60, "dis:");
            TextOut (hdc, 422, 110, "limit:");
            TextOut (hdc, 422, 160, "speed:");
            EndPaint (hWnd, hdc);
            break;      

        case MSG_LBUTTONUP:
            break;

        case MSG_TIMER:
            switch(wParam)
            {
                case IDT_PATH:
                    path_tracing();
                    break;
            }
            break;

        case MSG_DESTROY:
            UnloadBitmap (&load_bitmap);
            UnloadBitmap (&ntb_bitmap);
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

    MAINWINCREATE CreateInfo;


    /*Register map control*/
    RegisterMEagleControl();


    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "mEagle";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = mEagleProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = ww;
    CreateInfo.by = wh;
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

    MainWindowThreadCleanup (hMainWnd);

    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

