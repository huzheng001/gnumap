#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#ifndef VxWorks
#include <mgutils/mgutils.h>
#endif

#include "meagle.h"

#ifdef VxWorks
#define DRIVE_PATH "/ata0a/"
#else
#define DRIVE_PATH
#endif

#include "meagle_app.h"
#include "meagle_search.h"
#include "meagle_state.h"
static const int ww=750;
static const int wh=630;

static HWND hMainWnd;
static HWND eyewin_hwnd;
static int eyeshow = 0;
static int isOpen = 0;

const char **pmain_menu = NULL;
const char **pfile_menu_item =NULL;
const char **pview_menu_item =NULL;
const char **pdemo_menu_item =NULL;
const char **phelp_menu_item =NULL;
const char *lang = "en_us";

static const char* main_menu_cn[] = {
    "文件",
    "查看",
    "示例",
    "帮助"
};

static const char* main_menu_tw[] = {
    "文件",
    "查看",
    "示例",
    "幫助"
};

static const char* main_menu_en[] = {
    "File",
    "View",
    "Demo",
    "Help"
};

/*simple chinese menu*/
static const char* file_menu_item_cn[] = {
            "打开地图",
            "关闭地图",
            "保存地图",
            "退出"
};

static const char* view_menu_item_cn[] = {
            "缩小",
            "放大",
            "移动",
            "旋转",
            "层管理"
};

static const char* demo_menu_item_cn[] = {
            "移动定位",
            "路径跟踪",
            "查询"
};

static const char* help_menu_item_cn[] = {
            "关于"
};

/*traditional chinese menu*/
static const char* file_menu_item_tw[] = {
            "打開地圖",
            "關閉地圖",
            "保存地圖",
            "退出"
};

static const char* view_menu_item_tw[] = {
            "縮小",
            "放大",
            "移動",
            "旋轉",
            "層管理"
};

static const char* demo_menu_item_tw[] = {
            "移動定位",
            "路徑跟踪",
            "查詢"
};

static const char* help_menu_item_tw[] = {
            "關於"
};


/*english menu*/
static const char* file_menu_item[] = {
            "Open Map...",
            "Close Map",
            "Save Map",
            "Exit"
};

static const char* view_menu_item[] = {
            "Zoom In",
            "Zoom Out",
            "Pan",
            "Rotate",
            "Layer..."
};

static const char* demo_menu_item[] = {
            "Moving Localize",
            "Path Tracing",
            "Search"
};

static const char* help_menu_item[] = {
            "About"
};

static void zoomin (HWND hwnd, BOOL zoomin)
{

    double pixel_length;

    SendMessage(hwnd, ME_GET_SCALE, 0, (DWORD)(&pixel_length));

    if (zoomin){
        pixel_length = pixel_length/2;
    } else {
        pixel_length = pixel_length*2;
    }

    SendMessage(hwnd, ME_SET_SCALE, 0, (DWORD)(&pixel_length));
}

static HMENU create_submenu(const char* menu_item[], int nr_item, int idm)
{
    HMENU menu;
    MENUITEMINFO mii;
    int i;

    memset (&mii, 0, sizeof(mii));
    mii.type = MFT_STRING;
    mii.typedata = (DWORD)"";
    menu = CreatePopupMenu(&mii);

    memset (&mii, 0, sizeof(mii));
    mii.type = MFT_STRING;
    for (i = 0; i < nr_item; ++i)
    {
        mii.id = idm + i;
        mii.typedata = (DWORD)menu_item[i];
        InsertMenuItem (menu, i, TRUE, &mii);
    }
    menu = StripPopupHead(menu);
    return menu;
}

#ifndef VxWorks
static PLOGFONT cap_font = NULL;
static PLOGFONT menu_font = NULL;
static PLOGFONT utf8_font = NULL;

static void InitFont()
{

    cap_font = g_SysLogFont[SYSLOGFONT_CAPTION];
    ctrl_font = g_SysLogFont[SYSLOGFONT_CONTROL];
    menu_font = g_SysLogFont[SYSLOGFONT_MENU];

    utf8_font = CreateLogFontByName("*-Arial-rrncnn-*-12-UTF-8");

    g_SysLogFont[SYSLOGFONT_CAPTION] = utf8_font;
    g_SysLogFont[SYSLOGFONT_CONTROL] = utf8_font;
    g_SysLogFont[SYSLOGFONT_MENU] = utf8_font;

    if(strcasecmp(lang,"zh_cn")==0){
        local_SysText = GetSysTextInUTF8("zh_CN");
    }
    else if(strcasecmp(lang,"zh_tw")==0){
        local_SysText = GetSysTextInUTF8("zh_TW");
    }
    else
        local_SysText = GetSysTextInUTF8("EN");

}

static void ReleaseFont()
{
    g_SysLogFont[SYSLOGFONT_CAPTION] = cap_font;
    g_SysLogFont[SYSLOGFONT_CONTROL] = ctrl_font;
    g_SysLogFont[SYSLOGFONT_MENU] = menu_font;

    DestroyLogFont(utf8_font);
}
#endif

static HMENU create_menu()
{
    MENUITEMINFO mii;

    main_menu = CreateMenu();

    if(strcasecmp(lang,"zh_cn")==0){
        pmain_menu = main_menu_cn;
        pfile_menu_item = file_menu_item_cn;
        pview_menu_item = view_menu_item_cn;
        pdemo_menu_item = demo_menu_item_cn;
        phelp_menu_item = help_menu_item_cn;
    }
    else if(strcasecmp(lang,"zh_tw")==0){
        pmain_menu = main_menu_tw;
        pfile_menu_item = file_menu_item_tw;
        pview_menu_item = view_menu_item_tw;
        pdemo_menu_item = demo_menu_item_tw;
        phelp_menu_item = help_menu_item_tw;
    }
    else{
        pmain_menu = main_menu_en;
        pfile_menu_item = file_menu_item;
        pview_menu_item = view_menu_item;
        pdemo_menu_item = demo_menu_item;
        phelp_menu_item = help_menu_item;
    }

    memset (&mii, 0, sizeof(mii));
    mii.type = MFT_STRING;
    mii.id = IDM_MAIN_FILE;
    mii.typedata = (DWORD)pmain_menu[0];
    mii.hsubmenu = create_submenu(pfile_menu_item, TABLESIZE(file_menu_item), IDM_FILE_OPENMAP);
    InsertMenuItem(main_menu, 0, TRUE, &mii);

    mii.type = MFT_STRING;
    mii.id = IDM_MAIN_VIEW;
    mii.typedata = (DWORD)pmain_menu[1];
    mii.hsubmenu = create_submenu(pview_menu_item, TABLESIZE(view_menu_item), IDM_VIEW_ZOOMIN);
    InsertMenuItem(main_menu, 1, TRUE, &mii);

    mii.type = MFT_STRING;
    mii.id = IDM_MAIN_DEMO;
    mii.typedata = (DWORD)pmain_menu[2];
    mii.hsubmenu = create_submenu(pdemo_menu_item, TABLESIZE(demo_menu_item), IDM_DEMO_MOVINGLOCALIZE);
    InsertMenuItem(main_menu, 2, TRUE, &mii);

    mii.type = MFT_STRING;
    mii.id = IDM_MAIN_HELP;
    mii.typedata = (DWORD)pmain_menu[3];
    mii.hsubmenu = create_submenu(phelp_menu_item, TABLESIZE(help_menu_item), IDM_HELP_ABOUT);
    InsertMenuItem(main_menu, 3, TRUE, &mii);
    return main_menu;
}

static void set_other_item_unchecked (int nc)
{
    int i;
    NTBITEMINFO ntbii;
    for (i = OP_ZOOMIN; i <= OP_DISTANCE; ++i)
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
    switch (nc) {
        case OP_OPEN:
            SendMessage (GetParent(hwnd), MSG_COMMAND, IDM_FILE_OPENMAP, 0);
            return;
        case OP_SAVE:
            SendMessage (GetParent(hwnd), MSG_COMMAND, IDM_FILE_SAVEMAP, 0);
            return;
        case OP_CLOSE:
            SendMessage (GetParent(hwnd), MSG_COMMAND, IDM_FILE_CLOSEMAP, 0);
            return;
        case OP_GLOBAL:
            SendMessage (meagle_hwnd, ME_GLOBAL, 0, 0);
            return;
        case OP_ZOOMOUT:
            change_state(STATE_ZOOMOUT);
            SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_ZOOMOUT, 0);
            break;
        case OP_ZOOMIN:
            change_state(STATE_ZOOMIN);
            SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_ZOOMIN, 0);
            break;
        case OP_PAN:
            change_state(STATE_PAN);
            SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_DRAG, 0);
            break;
        case OP_ROTATE:
            change_state(STATE_ROTATE);
            SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_ROTATE, 0);
            break;
        case OP_DISTANCE:
            if (state == STATE_DISTANCE) {
                SendMessage (meagle_hwnd, ME_CLEAR, 0, 0);
            }
            change_state(STATE_DISTANCE);
            SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_DISTANCE, 0);
            break;
        case OP_FIND:
            if (state == STATE_DISTANCE) 
            {
                change_state(STATE_PAN);
                SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_DRAG, 0);
                set_other_item_unchecked (OP_PAN);
            }
            open_search_window(meagle_hwnd, lang);
            return;
        case OP_LAYER:
            if (state == STATE_DISTANCE) 
            {
                change_state(STATE_PAN);
                SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_DRAG, 0);
                set_other_item_unchecked (OP_PAN);
            }
            change_layer_window(meagle_hwnd, lang);
            return;
        case OP_THROW:
            if (state == STATE_DISTANCE) 
            {
                change_state(STATE_PAN);
                SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_DRAG, 0);
                set_other_item_unchecked (OP_PAN);
            }
            SendMessage (meagle_hwnd, ME_CLEAR, 0, 0);
            return;
        case OP_ZOOMIN2:
            if (state == STATE_DISTANCE) 
            {
                change_state(STATE_ZOOMIN);
                SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_ZOOMIN, 0);
                set_other_item_unchecked (OP_ZOOMIN);
            }
            zoomin (meagle_hwnd, TRUE);
            return;
        case OP_ZOOMOUT2:
            if (state == STATE_DISTANCE) 
            {
                change_state(STATE_ZOOMOUT);
                SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_ZOOMOUT, 0);
                set_other_item_unchecked (OP_ZOOMOUT);
            }
            zoomin (meagle_hwnd, FALSE);
            return;
        case OP_MOVE_N:
            if (state == STATE_DISTANCE) 
            {
                change_state(STATE_PAN);
                SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_DRAG, 0);
                set_other_item_unchecked (OP_PAN);
            }
            SendMessage (meagle_hwnd, ME_MOVE_Y, ME_COORDINATE_WIN, (LPARAM)-300);
            return;
        case OP_MOVE_S:
            if (state == STATE_DISTANCE)
            {
                change_state(STATE_PAN);
                SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_DRAG, 0);
                set_other_item_unchecked (OP_PAN);
            }
            SendMessage (meagle_hwnd, ME_MOVE_Y, ME_COORDINATE_WIN, (LPARAM)300);
            return;
        case OP_MOVE_W:
            if (state == STATE_DISTANCE)
            {
                change_state(STATE_PAN);
                SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_DRAG, 0);
                set_other_item_unchecked (OP_PAN);
            }
            SendMessage (meagle_hwnd, ME_MOVE_X, ME_COORDINATE_WIN, (LPARAM)-300);
            return;
        case OP_MOVE_E:
            if (state == STATE_DISTANCE)
            {
                change_state(STATE_PAN);
                SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_DRAG, 0);
                set_other_item_unchecked (OP_PAN);
            }
            SendMessage (meagle_hwnd, ME_MOVE_X, ME_COORDINATE_WIN, (LPARAM)300);
            return;
        case OP_ROTATE_R:
            {
                const double alpha = -G_PI/2;
                if (state == STATE_DISTANCE) 
                {
                    change_state(STATE_ROTATE);
                    SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_ROTATE, 0);
                    set_other_item_unchecked (OP_ROTATE);
                }

                SendMessage (meagle_hwnd, ME_ROTATE, 0, (LPARAM)&alpha);
            }
            return;
        case OP_ROTATE_L:
            {
                const double alpha = G_PI/2;
                if (state == STATE_DISTANCE) 
                {
                    change_state(STATE_ROTATE);
                    SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_ROTATE, 0);
                    set_other_item_unchecked (OP_ROTATE);
                }

                SendMessage (meagle_hwnd, ME_ROTATE, 0, (LPARAM)&alpha);
            }
            return;
        case OP_NORTH:
            if (state == STATE_DISTANCE) {
                change_state(STATE_ROTATE);
                SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_ROTATE, 0);
                set_other_item_unchecked (OP_ROTATE);
            }
            SendMessage (meagle_hwnd, ME_NORTH, 0, 0);
            return;
        case OP_EYEVIEW:
            if (eyeshow){
                eyeshow = 0;
                ShowWindow(eyewin_hwnd, SW_HIDE);
            }else{
                eyeshow = 1;
                ShowWindow(eyewin_hwnd, SW_SHOW);
            }
            return;
    }
    set_other_item_unchecked (nc);
}

static void create_tool_bar(HWND hWnd)
{
    int i;
    NTBINFO ntb_info;
    NTBITEMINFO ntbii;

    memset (&ntb_bitmap, 0, sizeof(ntb_bitmap));
    memset (&ntbii, 0, sizeof(ntbii));

    if (LoadBitmap (HDC_SCREEN, &ntb_bitmap, DRIVE_PATH"res/tool_bar.gif"))
    {
        fprintf (stderr, "Can not load the image for toolbar!\n");
    }

    ntb_info.image = &ntb_bitmap;
    ntb_info.nr_cells = TB_ITEMS;
    ntb_info.nr_cols = 4;
    ntb_info.w_cell = 0;
    ntb_info.h_cell = 0;

    ntb_hwnd = CreateWindow (CTRL_NEWTOOLBAR,
                            "",
                            WS_CHILD | WS_VISIBLE | NTBS_HORIZONTAL,
                            IDC_NTL,
                            0, 0, 750, 36,
                            hWnd,
                            (DWORD) &ntb_info);
    InvalidateRect (ntb_hwnd, NULL, TRUE);

    for (i = 0; i < TB_ITEMS; ++i)
    {
        if ((i>3 && i <9) || i == TB_ITEMS-1)
            ntbii.flags = NTBIF_CHECKBUTTON;
        else
            ntbii.flags = NTBIF_PUSHBUTTON;
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
                      0, 550, 150, 80, hWnd, 0);
    status_dis_hwnd = CreateWindow (CTRL_STATIC,
                      "",
                      WS_CHILD | WS_VISIBLE | SS_LEFT,
                      IDC_STATUS,
                      150, 550, 150, 80, hWnd, 0);
    CreateWindow (CTRL_STATIC,
            "Limit",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            IDC_STATUS,
            300, 550, 50, 80, hWnd, 0);
    status_limit_hwnd = CreateWindow (CTRL_STATIC,
                      "",
                      WS_CHILD | WS_VISIBLE | SS_LEFT,
                      IDC_STATUS,
                      350, 550, 150, 80, hWnd, 0);
    CreateWindow (CTRL_STATIC,
            "Speed",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            IDC_STATUS,
            500, 550, 50, 80, hWnd, 0);
    status_speed_hwnd = CreateWindow (CTRL_STATIC,
                      "",
                      WS_CHILD | WS_VISIBLE | SS_LEFT,
                      IDC_STATUS,
                      550, 550, 250, 80, hWnd, 0);
}

static void prepare_for_loadmap()
{ 
    if (LoadBitmap (HDC_SCREEN, &load_bitmap, DRIVE_PATH"res/load.gif"))
    {
        fprintf (stderr, "Can not load the image for load!\n");
    }

    if (LoadBitmap (HDC_SCREEN, &moving_bitmap, DRIVE_PATH"res/car.gif"))
    {
        fprintf (stderr, "Can not load the image for button!\n");

    }
    if (LoadBitmap (HDC_SCREEN, &reverse_bitmap, DRIVE_PATH"res/car_reverse.gif"))
    {
        fprintf (stderr, "Can not load the image for button!\n");

    }
}

static DLGTEMPLATE EyeWinData =
{
     WS_CAPTION | WS_BORDER,
#ifdef __TARGET_MGDESKTOP__
     WS_EX_TOPMOST | WS_EX_NOCLOSEBOX | WS_EX_DLGHIDE,
#else
     WS_EX_TOPMOST | WS_EX_NOCLOSEBOX,
#endif
     530, 425, 218, 175,
     "EyeView",
     0, 0,
     1, NULL,
     0
};
static CTRLDATA EyeCtrlInitProgress [] =
{
    {
        MEAGLEEYE_CTRL,
        WS_VISIBLE,
        0, 0, 218, 150,
        IDC_MEEYEVIEW,
        "",
        0
    }
};

#ifdef __TARGET_MGDESKTOP__
#define DEFAULT_MAP "./res/meagle.meg"
#define MSG_OPENMAP MSG_USER+123
#endif

int MEagleProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    switch (message)
    {
        case MSG_CREATE:
            create_tool_bar(hWnd);
            create_status_bar(hWnd);

#ifdef __TARGET_MGDESKTOP__
            meagle_hwnd = CreateWindow (MEAGLE_CTRL,
                                      "",
                                      WS_NONE,//WS_VISIBLE,
                                      IDC_MEVIEW,
                                      0, 36, 750, 514, hWnd, 0);
            EyeWinData.controls = EyeCtrlInitProgress;
            eyewin_hwnd = CreateMainWindowIndirect (&EyeWinData, hWnd, 0);
            //ShowWindow(eyewin_hwnd, SW_HIDE);
#else
            meagle_hwnd = CreateWindow (MEAGLE_CTRL,
                                      "",
                                      WS_VISIBLE,
                                      IDC_MEVIEW,
                                      0, 36, 750, 514, hWnd, 0);
            EyeWinData.controls = EyeCtrlInitProgress;
            eyewin_hwnd = CreateMainWindowIndirect (&EyeWinData, hWnd, 0);
            ShowWindow(eyewin_hwnd, SW_HIDE);
#endif
            meagleeye_hwnd = GetDlgItem(eyewin_hwnd, IDC_MEEYEVIEW);
            prepare_for_loadmap();

#ifdef __TARGET_MGDESKTOP__
            PostMessage(hWnd, MSG_OPENMAP, 0, 0);
#endif
            break;
#ifdef __TARGET_MGDESKTOP__
        case MSG_OPENMAP:
            set_other_item_unchecked (OP_PAN);
            SendMessage(meagle_hwnd, ME_LOAD_MEAGLE_MAP, 0, (LPARAM)DEFAULT_MAP);
            zoomin(meagle_hwnd, TRUE);
            SendMessage (meagle_hwnd, ME_MOVE_X, ME_COORDINATE_WIN, (LPARAM)390);
            ShowWindow(meagle_hwnd, SW_SHOWNORMAL);

            SendMessage(meagleeye_hwnd, ME_ADD_EYE, 0, meagle_hwnd);
            SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_DRAG, 0);
            change_state(STATE_PAN);
            isOpen = 1;
            return 0;
#endif
        case MSG_COMMAND:
            {
                switch (LOWORD(wParam))
                {
                    case IDM_FILE_OPENMAP:
                        {
                            char **layers_name = NULL;
                            SetIMEStatus (IME_STATUS_ENABLED, FALSE);
                            if (state == STATE_DISTANCE) 
                            {
                                change_state(STATE_PAN);
                                SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_DRAG, 0);
                                set_other_item_unchecked (OP_PAN);
                            }
#if !defined(VxWorks)
                            NEWFILEDLGDATA newfiledlg =
                            {
                                FALSE,
                                FALSE,
                                "",
                                "",
                                "./",
                                "All file(*.*)|MapInfo file(*.WOR;*.wor)|Gst file(*.GST;*.gst)|Meagle file(*.meg;*.MEG)",
                                1
                            };
                            int ret = ShowOpenDialog (hWnd, 100, 100, 350, 250, &newfiledlg);
                            if(ret == IDOK)
                            {
                                if (strcasecmp(newfiledlg.filefullname + strlen(newfiledlg.filefullname) -3, "MEG") == 0)
                                {
                                    set_other_item_unchecked (OP_PAN);
                                    SendMessage(meagle_hwnd, ME_LOAD_MEAGLE_MAP, 0, (LPARAM)newfiledlg.filefullname);
                                    SendMessage(meagleeye_hwnd, ME_ADD_EYE, 0, meagle_hwnd);
                                    SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_DRAG, 0);
                                    change_state(STATE_PAN);
                                    isOpen = 1;
                                }
                                else
                                {
                                    char* pos = strrchr(newfiledlg.filefullname, '/');
                                    if(pos != NULL)
                                        *pos = '\0';
                                    SendMessage(meagle_hwnd, ME_OPEN_MAP, 0, (LPARAM)newfiledlg.filefullname);
                                    set_other_item_unchecked (OP_PAN);

                                    SendMessage(meagle_hwnd, ME_GET_MAPINFO, 0, (LPARAM)&layers_name);

                                    if (layers_name != NULL){
                                        open_layer_window(hWnd, &layers_name);

                                        if (layers_name != NULL){
                                            hdc = GetClientDC(hWnd);
                                            FillBoxWithBitmap(hdc, (ww-106)/2, (wh-27)/2, 106, 27, &load_bitmap);
                                            ReleaseDC(hdc);

                                            SendMessage(meagle_hwnd, ME_LOAD_MAP, 0, (LPARAM)layers_name);
                                            SendMessage(meagleeye_hwnd, ME_ADD_EYE, 0, meagle_hwnd);
                                            SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_DRAG, 0);
                                            change_state(STATE_PAN);

                                            isOpen = 1;
                                        }
                                    }
                                    if(layers_name)
                                        free(layers_name);
                                }
                            }
#else
                SendMessage(meagle_hwnd, ME_OPEN_MAP, 0, (LPARAM)DRIVE_PATH"map");
                            set_other_item_unchecked (OP_PAN);
                SendMessage(meagle_hwnd, ME_GET_MAPINFO, 0, (LPARAM)&layers_name);

                            if (layers_name != NULL){
                                open_layer_window(hWnd, &layers_name);

                                if (layers_name != NULL){
                                   hdc = GetClientDC(hWnd);
                                   FillBoxWithBitmap(hdc, (ww-106)/2, (wh-27)/2, 106, 27, &load_bitmap);
                                   ReleaseDC(hdc);

                                   SendMessage(meagle_hwnd, ME_LOAD_MAP, 0, (LPARAM)layers_name);
                                   SendMessage(meagleeye_hwnd, ME_ADD_EYE, 0, meagle_hwnd);
                                   SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_DRAG, 0);
                                   change_state(STATE_PAN);

                                   isOpen = 1;
                                 }
                            }
                            if(layers_name)
                               free(layers_name);
#endif
                            break;
                        }
                    case IDM_FILE_CLOSEMAP:
                        if (state == STATE_DISTANCE) 
                        {
                            change_state(STATE_PAN);
                            SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_DRAG, 0);
                            set_other_item_unchecked (OP_PAN);
                        }
                        SendMessage(meagle_hwnd, ME_CLOSE_MAP, 0, 0);
                        isOpen = 0;
                        break;
                    case IDM_FILE_SAVEMAP:
                        {
                            SetIMEStatus (IME_STATUS_ENABLED, FALSE);
                            if (state == STATE_DISTANCE) 
                            {
                                change_state(STATE_PAN);
                                SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_DRAG, 0);
                                set_other_item_unchecked (OP_PAN);
                            }
#if !defined(VxWorks)
                            if (isOpen) {
                                NEWFILEDLGDATA newfiledlg =
                                {
                                    TRUE,
                                    "",
                                    "",
                                    "./",
                                    "Meagle file(*.meg;*.MEG)",
                                    1
                                };
                                int ret = ShowOpenDialog (hWnd, 100, 100, 350, 250, &newfiledlg);
                                if(ret == IDOK)
                                {
                                    char *full_name = NULL;
                                    int len;
                                    int equal = 1;
                                    len = strlen(newfiledlg.filefullname);
                                    if (len >4)
                                        equal = strcasecmp (newfiledlg.filefullname + len-4, ".meg");
                                    if (equal != 0) 
                                    {
                                        full_name = (char *)malloc ((len+5)*sizeof(char));
                                        full_name = strcpy(full_name, newfiledlg.filefullname);
                                        full_name = strcat(full_name, ".meg");
                                    }else
                                        full_name = strdup(newfiledlg.filefullname);

                                    SendMessage(meagle_hwnd, ME_SAVE_MEAGLE_MAP, 0, (LPARAM)full_name);
                                    if (full_name)
                                        free(full_name);
                                }
                            }
#endif
                            break;
                        }
                    case IDM_FILE_EXIT:
                        DestroyMainWindow (hWnd);
                        PostQuitMessage (hWnd);
                        return 0;
                    case IDM_VIEW_ZOOMIN:
                        change_state(STATE_ZOOMIN);
                        set_other_item_unchecked (OP_ZOOMIN);
                        SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_ZOOMIN, 0);
                        break;
                    case IDM_VIEW_ZOOMOUT:
                        change_state(STATE_ZOOMOUT);
                        set_other_item_unchecked (OP_ZOOMOUT);
                        SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_ZOOMOUT, 0);
                        break;
                    case IDM_VIEW_PAN:
                        change_state(STATE_PAN);
                        set_other_item_unchecked (OP_PAN);
                        SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_DRAG, 0);
                        break;
                    case IDM_VIEW_ROTATE:
                        change_state(STATE_ROTATE);
                        set_other_item_unchecked (OP_ROTATE);
                        SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_ROTATE, 0);
                        break;
                    case IDM_VIEW_LAYER:
                        if (state == STATE_DISTANCE) 
                        {
                            change_state(STATE_PAN);
                            SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_DRAG, 0);
                            set_other_item_unchecked (OP_PAN);
                        }
                        change_layer_window(hWnd, lang);
                        break;
                    case IDM_DEMO_SEARCH:
                        if (state == STATE_DISTANCE) 
                        {
                            change_state(STATE_PAN);
                            SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_DRAG, 0);
                            set_other_item_unchecked (OP_PAN);
                        }
                        open_search_window(hWnd, lang);
                        break;
                    case IDM_DEMO_MOVINGLOCALIZE:
                        if (state == STATE_DISTANCE) 
                        {
                            change_state(STATE_PAN);
                            SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_DRAG, 0);
                            set_other_item_unchecked (OP_PAN);
                        }
                        change_state(STATE_MOVINGLOCALIZE);
                        break;
                    case IDM_DEMO_PATHTRACING:
                        if (state == STATE_DISTANCE) 
                        {
                            change_state(STATE_PAN);
                            SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_DRAG, 0);
                            set_other_item_unchecked (OP_PAN);
                        }
                        set_car_pos(375,257);
                        open_path_window(hWnd, lang);
                        break;
                    case IDM_HELP_ABOUT:
                        if (state == STATE_DISTANCE) 
                        {
                            change_state(STATE_PAN);
                            SendMessage (meagle_hwnd, ME_CHANGE_TOOL, ME_STATUS_DRAG, 0);
                            set_other_item_unchecked (OP_PAN);
                        }
                        if (strcasecmp(lang, "zh_cn") == 0)
                        {
                            MessageBox(hWnd, "mEagle 1.0.0\n" 
                                    "Copyright (C) 2007, 北京飞漫软件技术有限公司。\n"
                                    "保留所有权利", "关于 mEagle", MB_OK);
                        }
                        else if (strcasecmp(lang, "zh_tw") == 0)
                        {
                            MessageBox(hWnd, "mEagle 1.0.0\n" 
                                    "Copyright (C) 2007, 北京飛漫軟件技術有限公司。\n"
                                    "保留所有權利", "關於 mEagle", MB_OK);
                        }
                        else
                        {
                            MessageBox(hWnd, "mEagle 1.0.0\n"
                                    "Copyright (C) 2007, Beijing Feynman Software Technology Co., Ltd.\n"
                                    "All Rights Reserved.", "About mEagle", MB_OK);
                        }
                        break;
                }
            }
            break;
        case MSG_KEYDOWN:
            if (wParam == SCANCODE_PAGEUP)
            {
                zoomin (meagle_hwnd, TRUE);
                return 0;
            }
            if (wParam == SCANCODE_PAGEDOWN)
            {
                zoomin (meagle_hwnd, FALSE);
                return 0;
            }
            if (wParam == SCANCODE_CURSORBLOCKLEFT)
            {
        double alpha = 0;
                if (state == STATE_MOVINGLOCALIZE)
                {
                    alpha = G_PI/36;
                    SendMessage (meagle_hwnd, ME_ROTATE, 0, (LPARAM)&alpha);
                    set_status_ll_info();
                }
                else if (state == STATE_MOVINGREVERSE)
                {
                    change_state(STATE_MOVINGLOCALIZE);
                    alpha = G_PI/36;
                    SendMessage (meagle_hwnd, ME_ROTATE, 0, (LPARAM)&alpha);
                    set_status_ll_info();
                }
                else if (state == STATE_PAN)
                {
                    SendMessage (meagle_hwnd, ME_MOVE_X, ME_COORDINATE_WIN, (LPARAM)-10);
                }
                return 0;
            }
            if (wParam == SCANCODE_CURSORBLOCKRIGHT)
            {
        double alpha = 0;
                if (state == STATE_MOVINGLOCALIZE)
                {
                    alpha = -G_PI/36;
                    SendMessage (meagle_hwnd, ME_ROTATE, 0, (LPARAM)&alpha);
                    set_status_ll_info();
                }
                else if (state == STATE_MOVINGREVERSE)
                {
                    change_state(STATE_MOVINGLOCALIZE);
                    alpha = -G_PI/36;
                    SendMessage (meagle_hwnd, ME_ROTATE, 0, (LPARAM)&alpha);
                    set_status_ll_info();
                }
                else if (state == STATE_PAN)
                {
                    SendMessage (meagle_hwnd, ME_MOVE_X, ME_COORDINATE_WIN, (LPARAM)10);
                }
                return 0;
            }
            if (wParam == SCANCODE_CURSORBLOCKUP)
            {
                if (state == STATE_MOVINGLOCALIZE)
                {
                    SendMessage (meagle_hwnd, ME_MOVE_Y, ME_COORDINATE_WIN, (LPARAM)-10);
                    set_status_ll_info();
                }
                else if (state == STATE_MOVINGREVERSE)
                {
                    change_state(STATE_MOVINGLOCALIZE);
                    SendMessage (meagle_hwnd, ME_MOVE_Y, ME_COORDINATE_WIN, (LPARAM)-10);
                    set_status_ll_info();
                }

                else if (state == STATE_PAN)
                {
                    SendMessage (meagle_hwnd, ME_MOVE_Y, ME_COORDINATE_WIN, (LPARAM)-10);
                }
                return 0;
            }
            if (wParam == SCANCODE_CURSORBLOCKDOWN)
            {
                if (state == STATE_MOVINGLOCALIZE)
                {
                    change_state(STATE_MOVINGREVERSE);
                    SendMessage (meagle_hwnd, ME_MOVE_Y, ME_COORDINATE_WIN, (LPARAM)10);
                    set_status_ll_info();
                }
                else if (state == STATE_MOVINGREVERSE)
                {
                    SendMessage (meagle_hwnd, ME_MOVE_Y, ME_COORDINATE_WIN, (LPARAM)10);
                    set_status_ll_info();
                }
                else if (state == STATE_PAN)
                {
                    SendMessage (meagle_hwnd, ME_MOVE_Y, ME_COORDINATE_WIN, (LPARAM)10);
                }
                return 0;
            }
            break;
        case MSG_LBUTTONDOWN:
            if (state == STATE_PATHTRACING) 
                return 0;
            break;
        case MSG_LBUTTONUP:
            if (state == STATE_PATHTRACING) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                if (x>0 && x<750 && y>36 && y<(wh-80))
                    set_path_info(x, y-36);
                return 0;
                /*
                if (x< 600 && y<600) {
                    return 0;
                }
                    */
            }
            break;
        case MSG_TIMER:
            switch(wParam){
                case IDT_PATH:
                    path_tracing();
                    break;
            }
            break;
        case MSG_DESTROY:
            SendMessage(meagleeye_hwnd, ME_REMOVE_EYE, 0, 0);
            UnloadBitmap (&load_bitmap);
            UnloadBitmap (&moving_bitmap);
            UnloadBitmap (&reverse_bitmap);
            UnloadBitmap (&ntb_bitmap);
            UnloadBitmap (&qtb_bitmap);
            DestroyMainWindowIndirect(eyewin_hwnd);
            DestroyAllControls (hWnd);
            break;
        case MSG_CLOSE:
            PostQuitMessage (hWnd);
            DestroyMainWindow (hWnd);
            return 0;
    }
    return DefaultMainWinProc (hWnd, message, wParam, lParam);
}

#ifdef __TARGET_MGDESKTOP__
extern int __mg_mainwin_offset_x;
extern int __mg_mainwin_offset_y;
#endif

int MiniGUIMain (int args, const char* argv[])
{
    MSG Msg;
    MAINWINCREATE CreateInfo;

#if _MINIGUI_VERSION_CODE >= _VERSION_CODE (2, 0, 2)
#if defined (_MGRM_PROCESSES) 
    int i;
    const char* layer = NULL;

#ifdef __TARGET_MGDESKTOP__
    lang = getenv("MGDESKTOP_LANG");
    const char *winoff = NULL;
    for (i = 1; i < args; i++) {
        if (strcmp (argv[i], "-layer") == 0)
            layer = argv[i + 1];
        else if (strcmp(argv[i], "-winoff") == 0)
            winoff = argv[i + 1];
        else if (strcmp(argv[i], "-lang") == 0)
            lang = argv[i + 1];
    }
    if (winoff) {
        __mg_mainwin_offset_x = atoi(winoff);
        winoff = strchr( winoff, ',');
        if (winoff) {
            __mg_mainwin_offset_y = atoi(winoff + 1);
        }
    }
#else
    for (i = 1; i < args; i++) {
        if (strcmp (argv[i], "-layer") == 0) {
            layer = argv[i + 1];
            break;
        }
    }
#endif
#ifndef __TARGET_MGDESKTOP__
    GetLayerInfo (layer, NULL, NULL, NULL);
#endif
    if (JoinLayer (layer, argv[0], 0, 0) == INV_LAYER_HANDLE) {
        printf ("JoinLayer: invalid layer handle.\n");
        exit (1);
    }
#endif

#elif _MINIGUI_VERSION_CODE >= _VERSION_CODE (1, 6, 8)
#ifdef _LITE_VERSION
    SetDesktopRect(0, 0, 800, 600);
#endif
#endif

#ifndef VxWorks
    InitFont();
    //InitNewOpenFileBox();
#endif

    RegisterMEagleControl ();
    RegisterMEagleEyeControl ();

#ifdef __TARGET_MGDESKTOP__
    CreateInfo.dwStyle = WS_VISIBLE|WS_CAPTION|WS_BORDER|WS_DLGFRAME;
#else
    CreateInfo.dwStyle = WS_VISIBLE|WS_CAPTION|WS_BORDER;
#endif
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "mEagle";
    CreateInfo.hMenu = create_menu();
    CreateInfo.hCursor = GetSystemCursor(IDC_ARROW);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = MEagleProc;
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
 
#if !defined(_LITE_VERSION) && !defined(_MGRM_PROCESSES) && !defined(_STAND_ALONE) && !defined(VxWorks)
#ifdef _IME_GB2312
    GBIMEWindow (hMainWnd);
#endif
#endif

    ShowWindow(hMainWnd, SW_SHOWNORMAL);
    SetWindowBkColor (hMainWnd, RGB2Pixel (HDC_SCREEN, 230, 246, 255));
    InvalidateRect (hMainWnd, NULL, TRUE);

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    UnregisterMEagleControl ();
    UnregisterMEagleEyeControl ();
#ifndef VxWorks
    //NewOpenFileBoxCleanup();
    ReleaseFont();    
#endif    
    MainWindowThreadCleanup (hMainWnd);

    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

