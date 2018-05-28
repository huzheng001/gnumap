#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <minigui/mgconfig.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "meagle_app.h"

extern HWND meagle_hwnd;
const char* dlglang=NULL;

#define IDC_CLAYER_INFO  701
#define IDL_CLAYER_LIST  702
#define IDC_CLAYER_UP  703
#define IDC_CLAYER_DOWN  704

static DLGTEMPLATE PopWinData =
{
     WS_CAPTION | WS_BORDER,
     WS_EX_NONE,
     100, 100, 280, 280,
     "",
     0, 0,
     6, NULL,
     0
};

static CTRLDATA CtrlInitProgress [] =
{
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
        20, 15, 200, 20,
        IDC_CLAYER_INFO,
        "",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_LISTBOX,
        WS_VISIBLE | WS_VSCROLL | WS_BORDER | LBS_NOTIFY | LBS_CHECKBOX | LBS_AUTOCHECK,
        20, 40, 180, 160,
        IDL_CLAYER_LIST,
        "",
        0
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON,
        210, 70, 60, 25,
        IDC_CLAYER_UP,
        "",
        0
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON,
        210, 150, 60, 25,
        IDC_CLAYER_DOWN,
        "",
        0
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON,
        20, 220, 60, 25,
        IDOK,
        "",
        0
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON,
        130, 220, 60, 25,
        IDCANCEL,
        "",
        0
    }
};

static void InitDlgFont(HWND hDlg)
{
    if(strcasecmp(dlglang,"zh_cn")==0){
        SetWindowCaption (hDlg, "切换层");
        SetWindowText(GetDlgItem(hDlg, IDC_CLAYER_INFO), "查看一个层或者多个层");
        SetWindowText(GetDlgItem(hDlg, IDC_CLAYER_DOWN), "向下");
        SetWindowText(GetDlgItem(hDlg, IDC_CLAYER_UP), "向上");
        SetWindowText(GetDlgItem(hDlg, IDOK), "确定");
        SetWindowText(GetDlgItem(hDlg, IDCANCEL), "取消");
    }
    else if(strcasecmp(dlglang,"zh_tw")==0){
        SetWindowCaption (hDlg, "切換層");
        SetWindowText(GetDlgItem(hDlg, IDC_CLAYER_INFO), "查看一個層或者多個層");
        SetWindowText(GetDlgItem(hDlg, IDC_CLAYER_DOWN), "向下");
        SetWindowText(GetDlgItem(hDlg, IDC_CLAYER_UP), "向上");
        SetWindowText(GetDlgItem(hDlg, IDOK), "確定");
        SetWindowText(GetDlgItem(hDlg, IDCANCEL), "取消");
   }
    else{
        SetWindowCaption (hDlg, "Change Layer");
        SetWindowText(GetDlgItem(hDlg, IDC_CLAYER_INFO), "Choose a Layer or Layers to view");
        SetWindowText(GetDlgItem(hDlg, IDC_CLAYER_DOWN), "Down");
        SetWindowText(GetDlgItem(hDlg, IDC_CLAYER_UP), "Up");
        SetWindowText(GetDlgItem(hDlg, IDOK), "OK");
        SetWindowText(GetDlgItem(hDlg, IDCANCEL), "CANCEL");
    }
}

static void display_all_clayers(HWND hDlg)
{
    int index;
    char *layer_name;
    int layer_visible;
    LISTBOXITEMINFO lbii;
    int layer_count = 0;

    SendMessage(meagle_hwnd, ME_GET_LAYERCOUNT, 0, (LPARAM)&layer_count);
    if (layer_count == 0)
        return;

    lbii.hIcon = 0;
    for (index =0;  index < layer_count; index++){
        SendMessage(meagle_hwnd, ME_GET_LAYERNAME, index, (LPARAM)&layer_name);
        lbii.string = layer_name;
        SendMessage(meagle_hwnd, ME_GET_LAYERVISIBLE, index, (LPARAM)&layer_visible);
        if (layer_visible == 1)
            lbii.cmFlag = CMFLAG_CHECKED;
        else
            lbii.cmFlag = CMFLAG_BLANK;
        SendDlgItemMessage (hDlg, IDL_CLAYER_LIST, LB_INSERTSTRING, 0, (LPARAM)&lbii);
    }
}

static void move_clayer_up(HWND hDlg)
{
    int cur_sel;
    LISTBOXITEMINFO lbii;
    lbii.hIcon = 0;
    lbii.cmFlag = CMFLAG_CHECKED;
 
    cur_sel = SendDlgItemMessage(hDlg, IDL_CLAYER_LIST, LB_GETCURSEL, 0, 0);
    if (cur_sel >0){
        int length = SendDlgItemMessage(hDlg, IDL_CLAYER_LIST, LB_GETTEXTLEN, cur_sel, 0);
        char layer[length+1];
        SendDlgItemMessage (hDlg, IDL_CLAYER_LIST, LB_GETTEXT, cur_sel, (LPARAM)layer);
        SendDlgItemMessage (hDlg, IDL_CLAYER_LIST, LB_DELETESTRING, cur_sel, 0);
        lbii.string = layer;
        SendDlgItemMessage (hDlg, IDL_CLAYER_LIST, LB_INSERTSTRING, cur_sel-1, (LPARAM)&lbii);
        SendDlgItemMessage (hDlg, IDL_CLAYER_LIST, LB_SETCURSEL, cur_sel-1, 0);
    }
}

static void move_clayer_down(HWND hDlg)
{
    int cur_sel;
    int count;
    LISTBOXITEMINFO lbii;
    lbii.hIcon = 0;
    lbii.cmFlag = CMFLAG_CHECKED;
 
    cur_sel = SendDlgItemMessage(hDlg, IDL_CLAYER_LIST, LB_GETCURSEL, 0, 0);
    count = SendDlgItemMessage(hDlg, IDL_CLAYER_LIST, LB_GETCOUNT, 0, 0);
    if (cur_sel >=0 && cur_sel < (count-1)){
        int length = SendDlgItemMessage(hDlg, IDL_CLAYER_LIST, LB_GETTEXTLEN, cur_sel, 0);
        char layer[length+1];
        SendDlgItemMessage (hDlg, IDL_CLAYER_LIST, LB_GETTEXT, cur_sel, (LPARAM)layer);
        SendDlgItemMessage (hDlg, IDL_CLAYER_LIST, LB_DELETESTRING, cur_sel, 0);
        lbii.string = layer;
        SendDlgItemMessage (hDlg, IDL_CLAYER_LIST, LB_INSERTSTRING, cur_sel+1, (LPARAM)&lbii);
        SendDlgItemMessage (hDlg, IDL_CLAYER_LIST, LB_SETCURSEL, cur_sel+1, 0);
    }
}

static int get_clayer_list(HWND hDlg)
{
    int index;
    int layer_visible;
    char string[MAX_NAME+1];

    int count = SendDlgItemMessage(hDlg, IDL_CLAYER_LIST, LB_GETCOUNT, 0, 0);
    if(count==0)
        return 0;

    for (index = 0; index<count; index++){
        SendDlgItemMessage (hDlg, IDL_CLAYER_LIST, LB_GETTEXT, count-1-index, (LPARAM)string);
        SendMessage(meagle_hwnd, ME_CHANGE_LAYERINDEX, index, (LPARAM)string);
        if(SendDlgItemMessage(hDlg, IDL_CLAYER_LIST, LB_GETCHECKMARK, count-1-index, 0) == CMFLAG_CHECKED)
            layer_visible = 1;
        else
            layer_visible= 0;
        SendMessage(meagle_hwnd, ME_CHANGE_LAYERVISIBLE, index, layer_visible);
    }
    SendMessage(meagle_hwnd, ME_REFRESH, 0, 0);
    SendMessage(meagle_hwnd, ME_UPDATE_EYE, 0, 0);
    return 1;
}

static int OpenLayerProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_INITDIALOG:
            InitDlgFont(hDlg);
            display_all_clayers(hDlg);
            return 1;
        case MSG_COMMAND:
            switch (wParam)
            {
                case IDC_CLAYER_UP:
                    move_clayer_up(hDlg);
                    break;
                case IDC_CLAYER_DOWN:
                    move_clayer_down(hDlg);
                    break;
                case IDOK:
                    get_clayer_list(hDlg);
                case IDCANCEL:
                    EndDialog(hDlg, wParam);
                    break;
            }
           break;
        }
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

void change_layer_window(HWND hParent, const char* lang)
{
    dlglang = lang;
    printf ("layer lang = %s \n", dlglang);
    PopWinData.controls = CtrlInitProgress;
    DialogBoxIndirectParam (&PopWinData, hParent, OpenLayerProc, 0);
}

