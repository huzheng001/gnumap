

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

#include "meagle.h"
#include "meagle_app.h"

#define IDC_LAYER_LINFO  601
#define IDL_LAYER_ALIST  602
#define IDC_LAYER_RINFO  603
#define IDL_LAYER_OLIST  604
#define IDC_LAYER_OPEN  605
#define IDC_LAYER_CLOSE  606
#define IDC_LAYER_UP  607
#define IDC_LAYER_DOWN  608

static DLGTEMPLATE PopWinData =
{
     WS_CAPTION | WS_BORDER,
     WS_EX_NONE,
     100, 100, 360, 320,
     "Open Layer",
     0, 0,
     10, NULL,
     0
};

static CTRLDATA CtrlInitProgress [] =
{
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
        30, 15, 120, 20,
        IDC_LAYER_LINFO,
        "All Layers:",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_LISTBOX,
        WS_VISIBLE | WS_VSCROLL | WS_BORDER | LBS_NOTIFY| LBS_MULTIPLESEL,
        20, 40, 120, 200,
        IDL_LAYER_ALIST,
        "",
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
        220, 15, 120, 20,
        IDC_LAYER_RINFO,
        "OPEN Layers:",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_LISTBOX,
        WS_VISIBLE | WS_VSCROLL | WS_BORDER | LBS_NOTIFY,
        220, 40, 120, 200,
        IDL_LAYER_OLIST,
        "",
        0
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON,
        150, 55, 60, 25,
        IDC_LAYER_OPEN,
        "---->",
        0
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON,
        150, 105, 60, 25,
        IDC_LAYER_CLOSE,
        "<----",
        0
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON,
        150, 155, 60, 25,
        IDC_LAYER_UP,
        "UP",
        0
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON,
        150, 205, 60, 25,
        IDC_LAYER_DOWN,
        "DOWN",
        0
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON,
        70, 255, 60, 25,
        IDOK,
        "OK",
        0
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON,
        240, 255, 60, 25,
        IDCANCEL,
        "CANCEL",
        0
    }
};

static void display_all_layers(HWND hDlg, char ***layers_name)
{

    char **displaylayername;
    int i, count;

    if (*layers_name == NULL)
        return;
    
    displaylayername = *layers_name;
    
    while (*displaylayername){
        SendDlgItemMessage (hDlg, IDL_LAYER_ALIST, LB_INSERTSTRING, 0, (LPARAM)(*displaylayername));
        //SendDlgItemMessage (hDlg, IDL_LAYER_ALIST, LB_ADDSTRING, 0, (LPARAM)(*displaylayername));
        *displaylayername++;
    }

    count = SendDlgItemMessage(hDlg, IDL_LAYER_ALIST, LB_GETCOUNT, 0, 0);
    if (count == 0)
        return;
   
    for (i=0; i<count; i++)
        SendDlgItemMessage (hDlg, IDL_LAYER_ALIST, LB_SETSEL, 1, (LPARAM)i);

}

static void do_open_layer(HWND hDlg, int A, int B, int sel_count)
{
    int i;
    int* sel_items;
    char layer_name[sel_count][MAX_NAME+1];

    sel_items = malloc(sizeof(int)*sel_count);
    memset(sel_items, 0,sizeof(int)*sel_count);
    
    SendDlgItemMessage(hDlg, A, LB_GETSELITEMS, sel_count, (LPARAM)sel_items);
 
    for (i =0; i< sel_count; i++){
        SendDlgItemMessage (hDlg, A, LB_GETTEXT, sel_items[i]-i, (LPARAM)layer_name[i]);
        SendDlgItemMessage (hDlg, A, LB_DELETESTRING, sel_items[i]-i, 0);
        SendDlgItemMessage (hDlg, B, LB_ADDSTRING, 0, (LPARAM)layer_name[i]);
    }
    
    SendDlgItemMessage (hDlg, B, LB_SETCURSEL, 0, 0);
    free(sel_items);
}

static void open_layer(HWND hDlg, int A, int B)
{
    int count;
    count = SendDlgItemMessage(hDlg, A, LB_GETSELCOUNT, 0, 0L);
    if (count == 0)
        return;
    else 
        do_open_layer(hDlg, A, B, count);
}


static void close_layer(HWND hDlg, int A, int B)
{
    int cur_sel = SendDlgItemMessage(hDlg, A, LB_GETCURSEL, 0, 0);
    if (cur_sel >=0){
        int length = SendDlgItemMessage(hDlg, A, LB_GETTEXTLEN, cur_sel, 0);
        char layer[length+1];
        SendDlgItemMessage (hDlg, A, LB_GETTEXT, cur_sel, (LPARAM)layer);
        SendDlgItemMessage (hDlg, A, LB_DELETESTRING, cur_sel, 0);
        SendDlgItemMessage (hDlg, B, LB_ADDSTRING, 0, (LPARAM)layer);
    }
}

static void move_layer_up(HWND hDlg)
{
    int cur_sel = SendDlgItemMessage(hDlg, IDL_LAYER_OLIST, LB_GETCURSEL, 0, 0);
    if (cur_sel >0){
        int length = SendDlgItemMessage(hDlg, IDL_LAYER_OLIST, LB_GETTEXTLEN, cur_sel, 0);
        char layer[length+1];
        SendDlgItemMessage (hDlg, IDL_LAYER_OLIST, LB_GETTEXT, cur_sel, (LPARAM)layer);
        SendDlgItemMessage (hDlg, IDL_LAYER_OLIST, LB_DELETESTRING, cur_sel, 0);
        SendDlgItemMessage (hDlg, IDL_LAYER_OLIST, LB_INSERTSTRING, cur_sel-1, (LPARAM)layer);
        SendDlgItemMessage (hDlg, IDL_LAYER_OLIST, LB_SETCURSEL, cur_sel-1, 0);
    }
}

static void move_layer_down(HWND hDlg)
{
    int cur_sel = SendDlgItemMessage(hDlg, IDL_LAYER_OLIST, LB_GETCURSEL, 0, 0);
    int count = SendDlgItemMessage(hDlg, IDL_LAYER_OLIST, LB_GETCOUNT, 0, 0);
    if (cur_sel >=0 && cur_sel < (count-1)){
        int length = SendDlgItemMessage(hDlg, IDL_LAYER_OLIST, LB_GETTEXTLEN, cur_sel, 0);
        char layer[length+1];
        SendDlgItemMessage (hDlg, IDL_LAYER_OLIST, LB_GETTEXT, cur_sel, (LPARAM)layer);
        SendDlgItemMessage (hDlg, IDL_LAYER_OLIST, LB_DELETESTRING, cur_sel, 0);
        SendDlgItemMessage (hDlg, IDL_LAYER_OLIST, LB_INSERTSTRING, cur_sel+1, (LPARAM)layer);
        SendDlgItemMessage (hDlg, IDL_LAYER_OLIST, LB_SETCURSEL, cur_sel+1, 0);
    }
}

static int do_get_layer_list(HWND hDlg, char ***layers_name, int count)
{
    char **avail_layers_name;
    int i;
    char string[count][MAX_NAME+1];
    int index=0;

    avail_layers_name = (char **)malloc((count+1) * sizeof(char *));
    memset(avail_layers_name,0, (count+1) * sizeof(char *));
 
    if(avail_layers_name == NULL)
        return 0;

    for (i= 0; i < count; i++){
        SendDlgItemMessage (hDlg, IDL_LAYER_OLIST, LB_GETTEXT, count-1-i, (LPARAM)string[i]);
        while((*layers_name)[index]){
            if (strcmp(string[i], (*layers_name)[index])==0)
                break;
            index++;
        }
        avail_layers_name[i] = (*layers_name)[index];
        index=0;
    }
    avail_layers_name[i] = NULL;
    if (*layers_name)
        free(*layers_name);
    *layers_name = avail_layers_name;
    return 1;
}

static int get_layer_list(HWND hDlg, char ***layers_name)
{
    int count;

    count = SendDlgItemMessage(hDlg, IDL_LAYER_OLIST, LB_GETCOUNT, 0, 0);
    if (count == 0){
        if (*layers_name)
            free(*layers_name);
        *layers_name = NULL;
        return 0;
    }else
        do_get_layer_list(hDlg, layers_name, count);
}

static int cancel_layer_list(HWND hDlg, char ***layers_name)
{
    if (*layers_name)
        free(*layers_name);
    *layers_name = NULL;
    return 1;
}
static int OpenLayerProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_INITDIALOG:
            SetWindowAdditionalData(hDlg, (DWORD)lParam);
            display_all_layers(hDlg, (char ***)lParam);
            return 1;
        case MSG_COMMAND:
            switch (wParam)
            {
                case IDC_LAYER_OPEN:
                    open_layer(hDlg, IDL_LAYER_ALIST, IDL_LAYER_OLIST);
                    break;
                case IDC_LAYER_CLOSE:
                    close_layer(hDlg, IDL_LAYER_OLIST, IDL_LAYER_ALIST);
                    break;
                case IDC_LAYER_UP:
                    move_layer_up(hDlg);
                    break;
                case IDC_LAYER_DOWN:
                    move_layer_down(hDlg);
                    break;
                case IDOK:
                    get_layer_list(hDlg, (char ***)GetWindowAdditionalData(hDlg));
                    EndDialog(hDlg, wParam);
                    break;
                case IDCANCEL:
                    cancel_layer_list(hDlg, (char ***)GetWindowAdditionalData(hDlg));
                    EndDialog(hDlg, wParam);
                    break;
            }
           break;
        }
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

void open_layer_window(HWND hParent, char *** layers_name)
{
    PopWinData.controls = CtrlInitProgress;
    DialogBoxIndirectParam (&PopWinData, hParent, OpenLayerProc, (DWORD)layers_name);
}

