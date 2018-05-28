#ifndef _MEAGLEAPP_H_
#define _MEAGLEAPP_H_

#include "meagle.h"
#define TB_ITEMS           22 

#define IDT_PATH 100

#define G_PI    3.14159265358979323846E0

HWND meagle_hwnd;
HWND meagleeye_hwnd;
HWND ntb_hwnd;
HWND qtb_hwnd;
HWND status_corner_hwnd;
HWND status_dis_hwnd;
HWND status_limit_hwnd;
HWND status_speed_hwnd;

HMENU main_menu;

PLOGFONT ctrl_font;
BITMAP ntb_bitmap;
BITMAP qtb_bitmap;

BITMAP load_bitmap;
BITMAP moving_bitmap;
BITMAP reverse_bitmap;

enum {
    IDC_MEVIEW = 101,
    IDC_MEEYEVIEW,
    IDC_STATUS,
    IDC_NTL,
    IDM_MAIN_FILE,
    IDM_MAIN_VIEW,
    IDM_MAIN_DEMO,
    IDM_MAIN_HELP,
    IDM_FILE_OPENMAP,
    IDM_FILE_CLOSEMAP,
    IDM_FILE_SAVEMAP,
    IDM_FILE_EXIT,
    IDM_VIEW_ZOOMIN,
    IDM_VIEW_ZOOMOUT,
    IDM_VIEW_PAN,
    IDM_VIEW_ROTATE,
    IDM_VIEW_LAYER,
    IDM_DEMO_MOVINGLOCALIZE,
    IDM_DEMO_PATHTRACING,
    IDM_DEMO_SEARCH,
    IDM_HELP_ABOUT,
};

enum {
    OP_BASE = 10,
    OP_OPEN,
    OP_SAVE,
    OP_CLOSE,
    OP_GLOBAL,
    OP_ZOOMIN,
    OP_ZOOMOUT,
    OP_PAN,
    OP_ROTATE,
    OP_DISTANCE,
    OP_FIND,
    OP_LAYER,
    OP_THROW,
    OP_ZOOMIN2,
    OP_ZOOMOUT2,
    OP_MOVE_N,
    OP_MOVE_S,
    OP_MOVE_W,
    OP_MOVE_E,
    OP_ROTATE_R,
    OP_ROTATE_L,
    OP_NORTH,
    OP_EYEVIEW
};

void open_layer_window(HWND hParent, char*** layname);
void change_layer_window(HWND hParent, const char* lang);
void open_path_window(HWND hParent, const char* lang);
void set_path_info(int x, int y);
void end_area_search(HWND HwndEdit, int IsPath, HWND hParent, const char* lang);
void end_key_search(HWND HwndEdit, int IsPath, HWND hDlg, const char* lang);

void path_tracing();
void SetParamToPath(HWND HwndEdit, char * name, double lat, double lon);
void set_car_pos(int width, int height);
#endif
