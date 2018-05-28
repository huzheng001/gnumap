#include <string.h>

#include <minigui/mgconfig.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "meagle.h"
#include "meagle_app.h"
#include "meagle_search.h"

extern HWND meagle_hwnd;
const char* searchlang = NULL;
#ifndef VxWorks
extern PLOGFONT ctrl_font;
#endif
#define IDC_KEY   601
#define IDC_AREA  602
#define IDC_KEYWORDS  603
#define IDC_DIA  604
#define IDC_SLAYERS  605
#define IDC_KEY_LIST  606
#define IDC_SEL_LIST  607
static int area_distance = 0;
static char *layer_name = NULL;
static int result_count = 0;
static KEYWORD_SEARCH result;
static KEYWORD_SEARCH **all_result = NULL;

static int ispath = 0;
static HWND hEdit = 0;


static DLGTEMPLATE KeyPopWinData =
{
     WS_CAPTION | WS_BORDER,
     WS_EX_TOPMOST,
     100, 100, 250, 230,
     "Search Result",
     0, 0,
     3, NULL,
     0
};

static CTRLDATA KeyCtrlInitProgress [] =
{
    {
        CTRL_LISTBOX,
        WS_VISIBLE | WS_VSCROLL | WS_BORDER | LBS_NOTIFY,
        5, 10, 220, 140,
        IDC_KEY_LIST,
        "",
        0
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON,
        30, 170, 60, 25,
        IDOK,
        "OK",
        0
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON,
        160, 170, 60, 25,
        IDCANCEL,
        "Cancel",
        0
    }
};


static void InitDisplayFont(HWND hDlg)
{
    if (strcasecmp(searchlang, "zh_cn") == 0)
    {
        SetWindowCaption(hDlg, "查找结果");
        SetWindowText(GetDlgItem(hDlg, IDOK), "确定");
        SetWindowText(GetDlgItem(hDlg, IDCANCEL), "取消");
    }
    else if (strcasecmp(searchlang, "zh_tw") == 0)
    {
        SetWindowCaption(hDlg, "查找結果");
        SetWindowText(GetDlgItem(hDlg, IDOK), "確定");
        SetWindowText(GetDlgItem(hDlg, IDCANCEL), "取消");
    }
    else
    {
        SetWindowCaption(hDlg, "Search Result");
        SetWindowText(GetDlgItem(hDlg, IDOK), "OK");
        SetWindowText(GetDlgItem(hDlg, IDCANCEL), "CANCEL");
   }
}

static void free_all_result()
{
    int i;
    for (i = 0; i < result_count; i++){
        if(all_result[i]->feature_name)
            free(all_result[i]->feature_name);
        if (all_result[i])
            free(all_result[i]);
    }
    if (all_result)
        free(all_result);
    
    all_result = NULL;
}

static void display_search_resultlist(HWND hDlg)
{
    int i;
    SendMessage(GetDlgItem(hDlg, IDC_KEY_LIST), LB_RESETCONTENT, 0, 0);
    for (i = 0; i < result_count; i++){
        SendMessage(GetDlgItem(hDlg, IDC_KEY_LIST), LB_ADDSTRING, 0,
                    (LPARAM) all_result[i]->feature_name);
    }
    SendMessage(GetDlgItem(hDlg, IDC_KEY_LIST), LB_SETCURSEL, 0, 0);
}

static void draw_result(HWND hDlg)
{
    int cursel;
    RESULT result;
    double latitude, longitude;
    if (!SendMessage(GetDlgItem(hDlg, IDC_KEY_LIST), LB_GETSELCOUNT, 0, 0))
        return;
    cursel = SendMessage(GetDlgItem(hDlg, IDC_KEY_LIST), LB_GETCURSEL, 0, 0);
#if 0
    if (ispath){
        SendMessage(meagle_hwnd, ME_CLEAR, 0, 0);
        SendMessage(meagle_hwnd, ME_REFRESH, 0, 0);
    }
#endif
    result.GEOMDATA.CIRCLE.ll.latitude = all_result[cursel]->latitude;
    result.GEOMDATA.CIRCLE.ll.longitude = all_result[cursel]->longitude;
    result.GEOMDATA.CIRCLE.color = "#FF000000";
    result.GEOMDATA.CIRCLE.size = 10;
    result.feature_name = all_result[cursel]->feature_name;
    result.geom_type = CIRCLEFILLED;
    
    SendMessage(meagle_hwnd, ME_ADD_RESULT, 0, (LPARAM) &result);
    if (!ispath){
        latitude = result.GEOMDATA.CIRCLE.ll.latitude;
        longitude = result.GEOMDATA.CIRCLE.ll.longitude;
        SendMessage(meagle_hwnd, ME_SETCENTER_LAT,(DWORD)&latitude, (LPARAM)&longitude );
        SendMessage(meagle_hwnd, ME_REFRESH, 0, 0);
    }else{
        SendMessage(meagle_hwnd, ME_REFRESH, 0, 0);
        SetParamToPath(hEdit, (char*)result.feature_name, result.GEOMDATA.CIRCLE.ll.latitude,result.GEOMDATA.CIRCLE.ll.longitude);
    }
}

static int KeyDisplayProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    int i;
    switch (message) {
        case MSG_INITDIALOG:
            printf ("KeyDisplayProc searchlang = %s \n", searchlang);
            InitDisplayFont(hDlg);
#ifndef VxWorks
            SetWindowFont(GetDlgItem(hDlg, IDC_KEY_LIST), ctrl_font);
#endif
            display_search_resultlist(hDlg);
            return 1;
        case MSG_COMMAND:
            switch (wParam)
            {
                case IDOK:
                    draw_result(hDlg);
                case IDCANCEL:
                    free_all_result();
                    ispath = 0;
                    hEdit = 0;
                    EndDialog(hDlg, wParam);
                    break;
            }
           break;
        }
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

void display_key_searchresult(HWND hParent, const char* lang)
{
    searchlang = lang;
    KeyPopWinData.controls = KeyCtrlInitProgress;
    DialogBoxIndirectParam (&KeyPopWinData, hParent, KeyDisplayProc, 0L);
}

static DLGTEMPLATE ChooseDlg=
{
    WS_BORDER | WS_CAPTION,
    WS_EX_TOPMOST,
    200, 200, 300, 240,
    "Select location",
    0, 0,
    4, NULL,
    0 
};

static CTRLDATA CtrlChooseDlg [] =
{
    {
        "static",
        WS_VISIBLE,
        20, 20, 270, 25,
        500,
        "Please select the right location:",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_LISTBOX,
        WS_VISIBLE | WS_VSCROLL | WS_BORDER | WS_TABSTOP,
        30, 45, 240, 120,
        IDC_SEL_LIST,
        "",
        0
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON,
        50, 180, 80, 25,
        IDOK,
        "OK",
        0
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON,
        160, 180, 80, 25,
        IDCANCEL,
        "CANCEL",
        0
    },
};

static void InitChooseFont(HWND hDlg)
{
    if (strcasecmp(searchlang, "zh_cn")==0)
    {
        SetWindowCaption (hDlg, "选择位置");
        SetWindowText(GetDlgItem(hDlg, 500), "请选择正确的位置:");
        SetWindowText(GetDlgItem(hDlg, IDOK), "确定");
        SetWindowText(GetDlgItem(hDlg, IDCANCEL), "取消");
    }
    else if (strcasecmp(searchlang, "zh_tw")==0)
    {
        SetWindowCaption (hDlg, "選擇位置");
        SetWindowText(GetDlgItem(hDlg, 500), "請選擇正確的位置:");
        SetWindowText(GetDlgItem(hDlg, IDOK), "確定");
        SetWindowText(GetDlgItem(hDlg, IDCANCEL), "取消");
    }
    else
    {
        SetWindowCaption (hDlg, "Search Location");
        SetWindowText(GetDlgItem(hDlg, 500), "Please select the right location:");
        SetWindowText(GetDlgItem(hDlg, IDOK), "OK");
        SetWindowText(GetDlgItem(hDlg, IDCANCEL), "CANCEL");
    }
}

void end_area_search(HWND HwndEdit, int IsPath, HWND hParent, const char* lang)
{
    int i;
    if (IsPath){
        hEdit = HwndEdit;
        ispath = IsPath;
    }
    result_count = SendMessage(meagle_hwnd, ME_GET_AREARESULTCOUNT, 0, 0);
    
    all_result = (KEYWORD_SEARCH **)malloc(result_count*sizeof(KEYWORD_SEARCH));
    memset(all_result, '0', result_count*sizeof(KEYWORD_SEARCH));

    for(i = 0; i < result_count; i++)
    {
        SendMessage(meagle_hwnd, ME_GET_AREARESULTITEM, i, (LPARAM)&result);
        all_result[i] = (KEYWORD_SEARCH *)malloc(sizeof(KEYWORD_SEARCH));
        all_result[i]->latitude = result.latitude;
        all_result[i]->longitude = result.longitude;
        all_result[i]->feature_name = result.feature_name;
    }
    display_key_searchresult(hParent, lang);
    //display_key_searchresult(HWND_DESKTOP);
}

static int ChooseProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    int i, cursel;
    AREA_SEARCH area_search;

    switch( message ) {
        case MSG_INITDIALOG:
            InitChooseFont(hDlg);
            printf ("ChooseProc searchlang = %s \n", searchlang);
            for ( i = 0; i < result_count; i++){
                SendMessage(GetDlgItem(hDlg, IDC_SEL_LIST), LB_ADDSTRING, 0,
                            (LPARAM) all_result[i]->feature_name);
                SendMessage(GetDlgItem(hDlg, IDC_SEL_LIST), LB_SETCURSEL, 0, 0);
            }
#ifndef VxWorks
            SetWindowFont(GetDlgItem(hDlg, IDC_SEL_LIST), ctrl_font);
#endif
            return 1;
        case MSG_COMMAND:
            switch (wParam)
            {
                case IDOK:
                    cursel = SendMessage(GetDlgItem(hDlg, IDC_SEL_LIST),LB_GETCURSEL, 0, 0);
                    if(cursel == -1)
                        break;
                    area_search.latitude = all_result[cursel]->latitude;
                    area_search.longitude = all_result[cursel]->longitude;
                    area_search.distance = area_distance;
                    if(!layer_name)
                        SendMessage(meagle_hwnd, ME_AREA_SEARCH, 0, (LPARAM)&area_search);
                    else
                        SendMessage(meagle_hwnd, ME_AREA_SEARCH, (WPARAM)layer_name, (LPARAM)&area_search);
                    free_all_result();
                    end_area_search(0, 0, hDlg, searchlang);
                    EndDialog(hDlg, wParam);
                    break;
                case IDCANCEL:
                    EndDialog(hDlg, wParam);
                    break;
            }
           break;
    }
    return DefaultDialogProc (hDlg, message, wParam, lParam);
};

HWND create_choosedlg(HWND hDlg, const char* lang)
{
    searchlang = lang;
    ChooseDlg.controls = CtrlChooseDlg;
    return DialogBoxIndirectParam(&ChooseDlg, hDlg, ChooseProc, 0);
}

static DLGTEMPLATE PopWinData =
{
     WS_CAPTION | WS_BORDER,
     WS_EX_NONE,
     100, 100, 250, 230,
     "Search",
     0, 0,
     11, NULL,
     0
};

static CTRLDATA CtrlInitProgress [] =
{
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_GROUPBOX,
        5, 5, 235, 50,
        100,
        "Mode:",
        0, WS_EX_TRANSPARENT
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_CHECKED | WS_GROUP,
        10, 30, 80, 20,
        IDC_KEY,
        "Location",
        0, WS_EX_TRANSPARENT
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | BS_AUTORADIOBUTTON,
        140, 30, 80, 20,
        IDC_AREA,
        "Area",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
        5, 70, 160, 25,
        200,
        "Name:",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_SLEDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        70, 70, 160, 25,
        IDC_KEYWORDS,
        NULL,
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
        5, 100, 60, 25,
        300,
        "Range(m):",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_SLEDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        70, 100, 160, 25,
        IDC_DIA,
        NULL,
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
        5, 130, 60, 25,
        400,
        "Layer:",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_COMBOBOX,
        WS_VISIBLE | CBS_DROPDOWNLIST | WS_TABSTOP,
        70, 130, 160, 25,
        IDC_SLAYERS,
        NULL,
        0, WS_EX_TRANSPARENT
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON,
        30, 170, 60, 25,
        IDOK,
        "Search",
        0
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON,
        160, 170, 60, 25,
        IDCANCEL,
        "Cancel",
        0
    }
};
static void display_all_clayers(HWND hDlg)
{
    int layer_count = 0;
    int index;
    SendMessage(meagle_hwnd, ME_GET_LAYERCOUNT, 0, (LPARAM)&layer_count);
    if (layer_count == 0)
        return;

    SendDlgItemMessage (hDlg, IDC_SLAYERS, CB_ADDSTRING, 0, (LPARAM)"All Layers");
    
    for (index =0; index < layer_count; index++){
        SendMessage(meagle_hwnd, ME_GET_LAYERNAME, index, (LPARAM)&layer_name);
        SendDlgItemMessage (hDlg, IDC_SLAYERS, CB_ADDSTRING, 0, (LPARAM)layer_name);
    }
    SendDlgItemMessage (hDlg, IDC_SLAYERS, CB_SETCURSEL, 0, 0);
}

static void select_notif_proc (HWND hwnd, int id, int nc, DWORD add_data )
{
    if (nc == BN_CLICKED){
        if (id == IDC_KEY)
            SendMessage(GetDlgItem (GetParent(hwnd), IDC_DIA), MSG_ENABLE, 0, 0);
        else if (id == IDC_AREA)
            SendMessage(GetDlgItem (GetParent(hwnd), IDC_DIA), MSG_ENABLE, 1, 0);
    }
}

static int begin_key_search(HWND hDlg)
{
    int layer_index;
    char keyinfo[256] = {0};
    GetWindowText(GetDlgItem(hDlg, IDC_KEYWORDS), keyinfo, 255);

    if (keyinfo[0] == '\0'){
        if(strcasecmp(searchlang,"zh_cn")==0){
            MessageBox(hDlg, "请重新输入查找信息。", "查找", MB_OK );
        }else if (strcasecmp(searchlang, "zh_tw")==0){
            MessageBox(hDlg, "請重新輸入查找信息。", "查找", MB_OK );
        }else{
            MessageBox(hDlg, "Please input searching information.", "Search", MB_OK );
        }
        return 0;
    }
    
    layer_index = SendMessage(GetDlgItem(hDlg, IDC_SLAYERS), CB_GETCURSEL, 0, 0);
    if (layer_index == 0)
        layer_name = NULL;
    else
        SendMessage(meagle_hwnd, ME_GET_LAYERNAME, (layer_index-1), (LPARAM)&layer_name);
    
    SendMessage (meagle_hwnd, ME_KEY_SEARCH, (WPARAM)layer_name, (LPARAM)keyinfo);
    
    return 1;
}

void end_key_search(HWND HwndEdit, int IsPath, HWND hDlg, const char* lang)
{
    int i, state;
    if (IsPath){
        hEdit = HwndEdit;
        ispath = IsPath;
    }
    result_count = SendMessage(meagle_hwnd, ME_GET_KEYRESULTCOUNT, 0, 0);
    if (result_count == 0){
        if (strcasecmp(lang, "zh_cn")==0){
            MessageBox( hDlg, "没有找到任何结果。", "查找结果", MB_OK );
        }else if (strcasecmp(lang, "zh_tw")==0){
            MessageBox( hDlg, "沒有找到任何結果。", "查找結果", MB_OK );
        }else{
            MessageBox( hDlg, "Not find any results.", "Search Result", MB_OK );
        }
        ispath = 0;
        return;
    }

    all_result = (KEYWORD_SEARCH**)malloc(result_count * sizeof(KEYWORD_SEARCH));

    for (i = 0; i < result_count; i++){
        SendMessage(meagle_hwnd, ME_GET_KEYRESULTITEM, i, (LPARAM)&result);
        all_result[i] =(KEYWORD_SEARCH*) malloc(sizeof(KEYWORD_SEARCH));
        all_result[i]->latitude = result.latitude;
        all_result[i]->longitude = result.longitude;
        all_result[i]->feature_name = result.feature_name;
    }
    if (ispath)
        state = BST_CHECKED;
    else 
        state = SendMessage(GetDlgItem(hDlg, IDC_KEY), BM_GETCHECK, 0, 0);

    if (state == BST_CHECKED)
        display_key_searchresult(hDlg, lang);
    else{
        int ret;
        char dist[80];
        
        ret = SendMessage(GetDlgItem(hDlg, IDC_DIA), MSG_GETTEXT, 80, (LPARAM)dist);
        if( ret == 0 ){
            if (strcasecmp(lang, "zh_cn")==0){
                MessageBox( hDlg, "请输入查找区域的范围(m)。", "范围", MB_OK );
            }else if (strcasecmp(lang, "zh_tw")==0){
                MessageBox( hDlg, "請輸入查找區域的範圍(m)。", "範圍", MB_OK );
            }else{
                MessageBox( hDlg, "Please input the range of the area(m).", "Range", MB_OK );
            }
            return;
        }
        area_distance = atoi(dist);
        create_choosedlg(hDlg, lang);
    }
}

static void InitSearchFont(HWND hDlg)
{
    if (strcasecmp(searchlang, "zh_cn")==0)
    {
        SetWindowCaption (hDlg, "查找");
        SetWindowText(GetDlgItem(hDlg, 100), "模式:");
        SetWindowText(GetDlgItem(hDlg, IDC_KEY), "关键字查找");
        SetWindowText(GetDlgItem(hDlg, IDC_AREA), "区域查找");
        SetWindowText(GetDlgItem(hDlg, 200), "名称:");
        SetWindowText(GetDlgItem(hDlg, 300), "范围(m):");
        SetWindowText(GetDlgItem(hDlg, 400), "层:");
        SetWindowText(GetDlgItem(hDlg, IDOK), "查找");
        SetWindowText(GetDlgItem(hDlg, IDCANCEL), "取消");
    }
    else if (strcasecmp(searchlang, "zh_tw")==0)
    {
        SetWindowCaption (hDlg, "查找");
        SetWindowText(GetDlgItem(hDlg, 100), "模式:");
        SetWindowText(GetDlgItem(hDlg, IDC_KEY), "關鍵字查找");
        SetWindowText(GetDlgItem(hDlg, IDC_AREA), "區域查找");
        SetWindowText(GetDlgItem(hDlg, 200), "名稱:");
        SetWindowText(GetDlgItem(hDlg, 300), "範圍(m):");
        SetWindowText(GetDlgItem(hDlg, 400), "層:");
        SetWindowText(GetDlgItem(hDlg, IDOK), "查找");
        SetWindowText(GetDlgItem(hDlg, IDCANCEL), "取消");
   }
    else
    {
        SetWindowCaption (hDlg, "Search");
        SetWindowText(GetDlgItem(hDlg, 100), "Mode:");
        SetWindowText(GetDlgItem(hDlg, IDC_KEY), "Location");
        SetWindowText(GetDlgItem(hDlg, IDC_AREA), "Area");
        SetWindowText(GetDlgItem(hDlg, 200), "Name:");
        SetWindowText(GetDlgItem(hDlg, 300), "Range(m):");
        SetWindowText(GetDlgItem(hDlg, 400), "Layer:");
        SetWindowText(GetDlgItem(hDlg, IDOK), "Search");
        SetWindowText(GetDlgItem(hDlg, IDCANCEL), "Cancel");
   }
}

static int SearchProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_INITDIALOG:
            InitSearchFont(hDlg);
#ifndef VxWorks
            SetWindowFont (GetDlgItem(hDlg, IDC_DIA), ctrl_font);
            SetWindowFont (GetDlgItem(hDlg, IDC_KEYWORDS), ctrl_font);
#endif
            display_all_clayers(hDlg);
            SetNotificationCallback(GetDlgItem (hDlg, IDC_KEY), select_notif_proc);
            SetNotificationCallback(GetDlgItem (hDlg, IDC_AREA), select_notif_proc);
            SendMessage(GetDlgItem (hDlg, IDC_DIA), MSG_ENABLE, 0, 0);
            return 1;
        case MSG_COMMAND:
            switch (wParam)
            {
                case IDOK:
                    if (begin_key_search(hDlg)){
                        end_key_search(0, 0, hDlg, (char*)searchlang);
                        EndDialog(hDlg, wParam);
                    }
                    break;
                case IDCANCEL:
                    EndDialog(hDlg, wParam);
                    break;
            }
           break;
    }
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

void open_search_window(HWND hParent, const char* lang)
{
    searchlang = lang;
    PopWinData.controls = CtrlInitProgress;
    DialogBoxIndirectParam (&PopWinData, hParent, SearchProc, 0);
}
