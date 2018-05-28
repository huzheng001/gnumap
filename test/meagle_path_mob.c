#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include <minigui/mgconfig.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "meagle_app.h"
#include "meagle_state.h"
#include "meagle_path.h"

#define IDC_PATH_INFO  701
#define IDC_PATH_FROM  702
#define IDC_PATH_FPICK  703
#define IDC_PATH_END  704
#define IDC_PATH_EPICK  705
#define IDC_PATH_FSEARCH  706
#define IDC_PATH_ESEARCH  707
#define IDC_PATH_PB  708
#define IDC_PATH_PBPICK  709
#define IDC_PATH_LIMIT   710
#define IDC_PATH_SPEED   711
#define IDC_PATH_MONITOR 712
#define IDC_PATH_GAS  713
static HWND hwnd_path;
static HWND hEdit;
static BITMAP speed_bitmap;
static BITMAP gas_bitmap;
static BITMAP monitor_bitmap;
static BITMAP from_bitmap;
static BITMAP end_bitmap;
static BITMAP car_bitmap;

static HWND hSpeed;

static int pickstatus = 0;

static PATHTRAC * PathTrac = NULL;
static PATH_POINT * FromPoint = NULL;
static PATH_POINT * EndPoint = NULL;

static int result_id = 0;
static int old_result_id = 0;
static int info_result_id = 0;
static int step = 0;
static double off_lat, off_lon;
static double turn_alpha=0;
static double distance=0;
static int info_type = PT_MONITOR;
static int pb_num = 1;
static int current_num = 0;
static int draw = 0;
static int car_width= 0;
static int car_height= 0;

static DLGTEMPLATE PopWinData =
{
     WS_CAPTION | WS_BORDER,
     WS_EX_TOPMOST,
     20, 20, 320, 320,
     "Path Tracing Demo",
     0, 0,
     18, NULL,
     0
};

static CTRLDATA CtrlInitProgress [] =
{
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
        10, 15, 160, 20,
        IDC_PATH_INFO,
        "Select Path Tracing Objects",
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
        25, 40, 50, 20,
        0,
        "Start",
        0
    },
    {
        CTRL_SLEDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        70, 40, 130, 25,
        IDC_PATH_FROM,
        NULL,
        0
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON,
        210, 40, 50, 25,
        IDC_PATH_FSEARCH,
        "Search",
        0
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON,
        270, 40, 30, 25,
        IDC_PATH_FPICK,
        "Pick",
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
        10, 80, 60, 20,
        0,
        "Destination",
        0
    },
    {
        CTRL_SLEDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        70, 80, 130, 25,
        IDC_PATH_END,
        NULL,
        0
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON,
        210, 80, 50, 25,
        IDC_PATH_ESEARCH,
        "Search",
        0
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON,
        270, 80, 30, 25,
        IDC_PATH_EPICK,
        "Pick",
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_GROUPBOX,
        10, 110, 300, 140,
        0,
        "Pass by",
        0
    },
    {
        CTRL_LISTBOX,
        WS_VISIBLE | WS_VSCROLL | WS_BORDER | LBS_NOTIFY ,
        70, 130, 150, 80,
        IDC_PATH_PB,
        NULL,
        0
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON,
        230, 160, 50, 25,
        IDC_PATH_PBPICK,
        "Pick",
        0
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | BS_AUTORADIOBUTTON |BS_CHECKED |WS_GROUP,
        20, 220, 60, 25,
        IDC_PATH_MONITOR,
        "Monitor",
        0
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | BS_AUTORADIOBUTTON,
        80, 220, 80, 25,
        IDC_PATH_GAS,
        "Gas Station",
        0
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | BS_AUTORADIOBUTTON,
        160, 220, 100, 25,
        IDC_PATH_LIMIT,
        "Speed Limiting",
        0
    },
    {
        CTRL_SLEDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER | WS_DISABLED, 
        260, 220, 50, 25,
        IDC_PATH_SPEED,
        "120 Km",
        0
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON,
        60, 260, 60, 25,
        IDOK,
        "OK",
        0
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON,
        200, 260, 60, 25,
        IDCANCEL,
        "CANCEL",
        0
    }
};

void set_car_pos(int width, int height)
{
    car_width = width;
    car_height = height;
}

static void add_path_circle_location(DOUBLE_POINT dp, char * color, int size, char* name)
{
    RESULT result;
    result.GEOMDATA.CIRCLE.ll.latitude = dp.x;
    result.GEOMDATA.CIRCLE.ll.longitude = dp.y;
    result.GEOMDATA.CIRCLE.color = color;
    result.GEOMDATA.CIRCLE.size = size;
    result.feature_name = name;
    result.geom_type = CIRCLEFILLED;
    SendMessage(meagle_hwnd, ME_ADD_RESULT, (WPARAM)&result_id, (LPARAM) &result);
}

static void add_path_picture_location(DOUBLE_POINT *dpoint, POINT* point, PBITMAP pBitmap, 
                                        int width, int height, char* name, int isll)
{
    RESULT result;
    result.GEOMDATA.PICTURE.width = width;
    result.GEOMDATA.PICTURE.height = height;
    result.GEOMDATA.PICTURE.pBitmap = pBitmap;
    if (isll){
        result.GEOMDATA.PICTURE.isll = isll;
        result.GEOMDATA.PICTURE.ll.latitude = dpoint->x;
        result.GEOMDATA.PICTURE.ll.longitude = dpoint->y;
    }else{
        result.GEOMDATA.PICTURE.win.x = point->x;
        result.GEOMDATA.PICTURE.win.y = point->y;
        result.GEOMDATA.PICTURE.isll = FALSE;
    }
    result.feature_name = name;
    result.geom_type = PICTURE;
    SendMessage(meagle_hwnd, ME_ADD_RESULT, (WPARAM)&result_id, (LPARAM) &result);
}

void set_path_info(int x, int y)
{
    change_state(STATE_PAN);
    POINT point = { x, y };
    DOUBLE_POINT dp;
    char buf[50];

    SendMessage(meagle_hwnd, ME_WIN_TO_LL, (WPARAM)&point, (LPARAM)&dp);

    if (pickstatus){
        AREA_SEARCH area_search;
        area_search.latitude = dp.x;
        area_search.longitude = dp.y;
        area_search.distance = 200;
        SendMessage(meagle_hwnd, ME_AREA_SEARCH, 0, (LPARAM)&area_search);
        end_area_search(hEdit, 1,hwnd_path, "zh_cn");
        pickstatus =0;
    }else{
        if (pb_num > 8)
            return;
        sprintf(buf, "Point %d", pb_num);
        add_path_circle_location(dp, "#00FF0000", 10, buf);
        SendMessage(meagle_hwnd, ME_REFRESH, 0, 0);
        PathTrac->points[pb_num] = malloc(sizeof(PATH_POINT));
        memset(PathTrac->points[pb_num], '0', sizeof(PATH_POINT));
        PathTrac->points[pb_num]->dpoint.x =dp.x;
        PathTrac->points[pb_num]->dpoint.y =dp.y;
        PathTrac->points[pb_num]->name = strdup(buf);

        if (info_type == PT_MONITOR){
            strcat(buf, "-Monitor");
            PathTrac->points[pb_num]->type = PT_MONITOR;
            PathTrac->points[pb_num]->velocity =NULL;
        }
        else if (info_type == PT_GAS){
            strcat(buf, "-Gas Station");
            PathTrac->points[pb_num]->type = PT_GAS;
            PathTrac->points[pb_num]->velocity =NULL;
        }else if (info_type == PT_SPEED){
            int length;
            char velocity[MAX_NAME+1];
            
            length = GetWindowTextLength(hSpeed);
            GetWindowText(hSpeed, velocity, length);
            PathTrac->points[pb_num]->type = PT_SPEED;
            PathTrac->points[pb_num]->velocity =strdup(velocity);
            strcat(buf, "-Speed Limiting");
            strcat(buf, velocity);
        }
        SendMessage (hEdit, LB_ADDSTRING, 0, (LPARAM)buf);
        pb_num++;
    }
}

static void path_from_search(HWND hwnd_edit, HWND hDlg)
{
    char keyinfo[256] = {0};
    GetWindowText(hwnd_edit, keyinfo, 255);
    if (keyinfo[0] != '\0')
        SendMessage (meagle_hwnd, ME_KEY_SEARCH, 0, (LPARAM)keyinfo);
    else{
        MessageBox( GetActiveWindow(), "Please input information about location.", "Search", MB_OK );
        return;
    }
    end_key_search(hwnd_edit, 1, hDlg, NULL);
}

static void input_speed_enable(HWND hwnd, int id, int nc, DWORD add_data)
{
    if (nc == BN_CLICKED){
        EnableWindow(GetDlgItem(GetParent(hwnd), IDC_PATH_SPEED),TRUE);
        info_type = PT_SPEED;
    }
}

static void input_speed_disable(HWND hwnd, int id, int nc, DWORD add_data)
{
    if (nc == BN_CLICKED){
        EnableWindow(GetDlgItem(GetParent(hwnd), IDC_PATH_SPEED),FALSE);
        if (id == IDC_PATH_MONITOR)
            info_type = PT_MONITOR;
        else if (id == IDC_PATH_GAS)
            info_type = PT_GAS;
    }
}

void SetParamToPath(HWND hEdit, char *name, double lat, double lon)
{
    SetWindowText(hEdit, name);
    if(hEdit == GetDlgItem(GetParent(hEdit), IDC_PATH_FROM)){
        if (FromPoint == NULL){
            FromPoint = malloc(sizeof(PATH_POINT));
            memset(FromPoint, '0', sizeof(PATH_POINT));
        }
        FromPoint->dpoint.x = lat;
        FromPoint->dpoint.y = lon;
        FromPoint->name = strdup(name);
        FromPoint->velocity = NULL;
        FromPoint->type = PT_FROM;
    }else if(hEdit == GetDlgItem(GetParent(hEdit), IDC_PATH_END)){
        if (EndPoint == NULL){
            EndPoint = malloc(sizeof(PATH_POINT));
            memset(EndPoint, '0', sizeof(PATH_POINT));
        }
        EndPoint->dpoint.x = lat;
        EndPoint->dpoint.y = lon;
        EndPoint->name = strdup(name);
        EndPoint->velocity = NULL;
        EndPoint->type = PT_END;
    }
}

static int get_info_from_dlg(HWND hDlg)
{
    SendMessage(meagle_hwnd, ME_CLEAR, 0, 0);
    SendMessage(meagle_hwnd, ME_REFRESH, 0, 0);
   
    if ((FromPoint != NULL) && (EndPoint != NULL)){
        if (strcmp(FromPoint->name, EndPoint->name) == 0)
            return 0;
        else{
            PathTrac->points[0] = FromPoint;
            PathTrac->points[pb_num] = EndPoint;
            return 1;
        }
    }
    return 0;
}

static void add_path_line()
{
    DOUBLE_POINT points[pb_num+1];
    int i=0;
    for(i; i<pb_num+1; i++){
        points[i] = (PathTrac->points[i])->dpoint;
    }

    RESULT result;
    result.geom_type = LINES;
    result.GEOMDATA.LINES.width = 10;
    result.GEOMDATA.LINES.color = "#0000FF00";
    result.GEOMDATA.LINES.num = pb_num+1;
    result.GEOMDATA.LINES.points = points;
    result.feature_name = "path line";
    SendMessage(meagle_hwnd, ME_ADD_RESULT, (WPARAM)&result_id, (LPARAM) &result);
}

static void get_location_win_coordinate(DOUBLE_POINT* dp, POINT* p)
{
    SendMessage(meagle_hwnd, ME_LL_TO_WIN, (WPARAM)dp, (LPARAM)p);
}

static void get_path_info()
{
    PathTrac->len = malloc(pb_num*sizeof(int));
    memset(PathTrac->len, '0', pb_num*sizeof(int));
    
    int i;
    for(i=0; i<pb_num+1; i++)
        get_location_win_coordinate(&(PathTrac->points[i]->dpoint), &(PathTrac->points[i]->point));

    for(i=0; i<pb_num; i++){
    PathTrac->len[i] =(int) sqrt(((PathTrac->points[i+1]->point.x) -(PathTrac->points[i]->point.x)) * 
            ((PathTrac->points[i+1]->point.x) -(PathTrac->points[i]->point.x)) + 
            ((PathTrac->points[i+1]->point.y) -(PathTrac->points[i]->point.y)) * 
            ((PathTrac->points[i+1]->point.y) -(PathTrac->points[i]->point.y)));
    }
    PathTrac->total_distance = 0;
    for(i=0; i<pb_num; i++)
        PathTrac->total_distance += (int)PathTrac->len[i];

    PathTrac->part = malloc(pb_num*sizeof(int));
    memset(PathTrac->part, '0', pb_num*sizeof(int));
    PathTrac->part_offset = malloc(pb_num*sizeof(int));
    memset(PathTrac->part_offset, '0', pb_num*sizeof(int));

    PathTrac->perdistance = 20;

    for(i=0; i<pb_num; i++){
        PathTrac->part[i] =(int)(PathTrac->len[i]/PathTrac->perdistance);
        PathTrac->part_offset[i] =PathTrac->len[i]-PathTrac->part[i]*PathTrac->perdistance;
    }
 
    PathTrac->off_lat = malloc(pb_num*sizeof(double));
    memset(PathTrac->off_lat, '0', pb_num*sizeof(double));
    PathTrac->off_lon = malloc(pb_num*sizeof(double));
    memset(PathTrac->off_lon, '0', pb_num*sizeof(double));

    for(i=0; i<pb_num; i++){
        if (PathTrac->part_offset[i] == 0){
            PathTrac->off_lat[i] = ((PathTrac->points[i+1])->dpoint.x -(PathTrac->points[i])->dpoint.x)/PathTrac->part[i];
            PathTrac->off_lon[i] = ((PathTrac->points[i+1])->dpoint.y -(PathTrac->points[i])->dpoint.y)/PathTrac->part[i];
        }else{
            PathTrac->off_lat[i] = ((PathTrac->points[i+1])->dpoint.x -(PathTrac->points[i])->dpoint.x)/(PathTrac->part[i]+1);
            PathTrac->off_lon[i] = ((PathTrac->points[i+1])->dpoint.y -(PathTrac->points[i])->dpoint.y)/(PathTrac->part[i]+1);
        }
    }

    PathTrac->cpoint = PathTrac->points[0]->dpoint;

    off_lat = PathTrac->off_lat[0];
    off_lon = PathTrac->off_lon[0];

    distance = PathTrac->len[0];
    current_num = 0;
    step = 0;
    draw = 1;
    POINT point={car_width, car_height};
    add_path_picture_location(NULL, &point, &car_bitmap, 40, 40, "", FALSE);
    SetTimer(GetParent(meagle_hwnd), IDT_PATH, 10);
    EnableWindow(ntb_hwnd,FALSE);
}

static void change_path_direction(POINT* P0, POINT* P1, double *alpha)
{
    if (P1->x == P0->x){
        if (P1->y > P0->y)
            *alpha = G_PI;
        else
            *alpha = 0;
    }else{
        if (P1->x > P0->x)
            *alpha = atan2((P1->y - P0->y),(P1->x - P0->x));
        else if (P1->x < P0->x)
            *alpha = atan2((P0->y - P1->y),(P0->x - P1->x));
    }

    if (P1->x < P0->x)
        *alpha = (G_PI/2)-(*alpha);
    else if (P1->x > P0->x)
        *alpha = -(G_PI/2)-*alpha;
}


static void get_next_turn_alpha(int index, double* alpha)
{
    if (index < pb_num){
        get_location_win_coordinate(&((PathTrac->points[index])->dpoint),&((PathTrac->points[index])->point));
        get_location_win_coordinate(&((PathTrac->points[index+1])->dpoint),&((PathTrac->points[index+1])->point));

        change_path_direction(&((PathTrac->points[index])->point), &((PathTrac->points[index+1])->point), alpha);
    }else
        *alpha =0;
}

static void free_all()
{
    int i;
    if (PathTrac){
        if (PathTrac->points){
            for(i=1; i<pb_num; i++){
                if(PathTrac->points[i]){
                    if(PathTrac->points[i]->name)
                        free(PathTrac->points[i]->name);
                    if(PathTrac->points[i]->velocity)
                        free(PathTrac->points[i]->velocity);
                    free(PathTrac->points[i]);
                }
            }
            free(PathTrac->points);
            PathTrac->points =NULL;
        }
        if (PathTrac->len)
            free(PathTrac->len);
        if (PathTrac->part)
            free(PathTrac->part);
        if (PathTrac->part_offset)
            free(PathTrac->part_offset);
        if (PathTrac->off_lat)
            free(PathTrac->off_lat);
        if (PathTrac->off_lon)
            free(PathTrac->off_lon);
        free(PathTrac);
        PathTrac = NULL;
    }

    if (FromPoint){
        if(FromPoint->name)
            free(FromPoint->name);
        free(FromPoint);
        FromPoint = NULL;
    }
    if (EndPoint){
        if(EndPoint->name)
            free(EndPoint->name);
        free(EndPoint);
        EndPoint = NULL;
    }
    result_id = 0;
    info_type = PT_MONITOR;
    pb_num = 1;
    current_num = 0;
}

void path_tracing()
{
    char buf[80];
    PathTrac->cpoint.x += off_lat;
    PathTrac->cpoint.y += off_lon;
    
    SendMessage(meagle_hwnd, ME_SETCENTER_LAT,(DWORD)&PathTrac->cpoint.x, (LPARAM)&PathTrac->cpoint.y);
    SendMessage(meagle_hwnd, ME_REFRESH, 0, 0);

    if (step == 0){
        if (PathTrac->part_offset[current_num]== 0 )
            distance -= PathTrac->perdistance;
        else
            distance -= PathTrac->part_offset[current_num];
    }else
        distance -= PathTrac->perdistance;

    if (turn_alpha > 0){
        SetWindowText(status_corner_hwnd, "Turn Left");
        sprintf(buf, "%d m", (int)distance);
    }else if (turn_alpha < 0){
        SetWindowText(status_corner_hwnd, "Turn Right");
        sprintf(buf, "%d m", (int)distance);
    }else{
        SetWindowText(status_corner_hwnd, "Go Ahead");
        sprintf(buf, "%d m", (int)distance);
    }

    SetWindowText(status_dis_hwnd, buf);
    SetWindowText(status_speed_hwnd, "72 Km/H");
    POINT info_point={car_width*1.5, car_height};
    step ++;

    if ( (distance < 220) && (distance >0) && draw){
        draw = 0;
        old_result_id = result_id;
        SendMessage(meagle_hwnd, ME_REMOVE_RESULT, 0, (LPARAM) info_result_id);
        if (PathTrac->points[current_num+1]->type == PT_SPEED){
            add_path_picture_location(NULL, &info_point, &speed_bitmap, 89, 89, "info_pic", FALSE);
            sprintf(buf, "%s", PathTrac->points[current_num+1]->velocity);
            SetWindowText(status_limit_hwnd, buf);
        }
        else if (PathTrac->points[current_num+1]->type == PT_GAS)
            add_path_picture_location(NULL, &info_point, &gas_bitmap, 89, 89, "info_pic", FALSE);
        else if (PathTrac->points[current_num+1]->type == PT_MONITOR)
            add_path_picture_location(NULL, &info_point, &monitor_bitmap, 89, 89, "info_pic", FALSE);
        
        info_result_id = result_id;
        result_id = old_result_id;
    }

    if (distance == 0){
        draw =1;
        SendMessage(meagle_hwnd, ME_REMOVE_RESULT, 0, (LPARAM) info_result_id);
        SendMessage(meagle_hwnd, ME_ROTATE, 0, (LPARAM)&turn_alpha);
        SetWindowText(status_limit_hwnd, "");
        current_num++;
        if (current_num == pb_num){
            free_all();
            draw =0;
            SetWindowText(status_corner_hwnd, "");
            SetWindowText(status_dis_hwnd, "");
            SetWindowText(status_speed_hwnd, "");
            UnloadBitmap (&speed_bitmap);
            UnloadBitmap (&gas_bitmap);
            UnloadBitmap (&monitor_bitmap);
            UnloadBitmap (&from_bitmap);
            UnloadBitmap (&end_bitmap);
            UnloadBitmap (&car_bitmap);
            KillTimer(GetParent(meagle_hwnd), IDT_PATH);
            EnableWindow(ntb_hwnd,TRUE);
            SendMessage(meagle_hwnd, ME_CLEAR, 0, 0);
            SendMessage(meagle_hwnd, ME_REFRESH, 0, 0);
            return;
        }

        get_next_turn_alpha(current_num+1, &turn_alpha);

        PathTrac->cpoint = PathTrac->points[current_num]->dpoint;

        off_lat = PathTrac->off_lat[current_num];
        off_lon = PathTrac->off_lon[current_num];

        distance = PathTrac->len[current_num];
        step = 0;
    }
}

static void display_path_tracing()
{
    double pixel_length;
    int i=1;

    add_path_line();
    for(i; i<pb_num; i++){
        add_path_circle_location((PathTrac->points[i])->dpoint,"#FF00FF00", 20, (PathTrac->points[i])->name);
    }

    add_path_picture_location(&(PathTrac->points[0]->dpoint), NULL, 
            &from_bitmap, 80, 80, (PathTrac->points[0])->name, TRUE);
    add_path_picture_location(&(PathTrac->points[pb_num]->dpoint), NULL, 
            &end_bitmap, 80, 80, (PathTrac->points[pb_num])->name, TRUE);

   
    pixel_length = 1;
    SendMessage(meagle_hwnd, ME_SET_SCALE, 0, (LPARAM)&pixel_length);
    SendMessage(meagle_hwnd, ME_SETCENTER_LAT,
            (DWORD)&(((PathTrac->points[0])->dpoint).x), (LPARAM)&(((PathTrac->points[0])->dpoint).y));

    get_next_turn_alpha(0, &turn_alpha);
    
    SendMessage(meagle_hwnd, ME_ROTATE, 0, (LPARAM)&turn_alpha);
    get_next_turn_alpha(1, &turn_alpha);

    SendMessage(meagle_hwnd, ME_REFRESH, 0, 0);
}

static void init_path_tracing_info()
{
    memset (&speed_bitmap, 0, sizeof(speed_bitmap));
    memset (&gas_bitmap, 0, sizeof(gas_bitmap));
    memset (&monitor_bitmap, 0, sizeof(monitor_bitmap));
    memset (&from_bitmap, 0, sizeof(from_bitmap));
    memset (&end_bitmap, 0, sizeof(end_bitmap));
    memset (&car_bitmap, 0, sizeof(car_bitmap));

    if (LoadBitmap (HDC_SCREEN, &speed_bitmap, "res/demo.gif"))
    {
        fprintf (stderr, "Can not load the image for demo!\n");
        exit (-1);
    }
    if (LoadBitmap (HDC_SCREEN, &gas_bitmap, "res/gas.gif"))
    {
        fprintf (stderr, "Can not load the image for demo!\n");
        exit (-1);
    }
    if (LoadBitmap (HDC_SCREEN, &monitor_bitmap, "res/monitor.gif"))
    {
        fprintf (stderr, "Can not load the image for demo!\n");
        exit (-1);
    }
    if (LoadBitmap (HDC_SCREEN, &from_bitmap, "res/from.gif"))
    {
        fprintf (stderr, "Can not load the image for demo!\n");
        exit (-1);
    }

    if (LoadBitmap (HDC_SCREEN, &end_bitmap, "res/end.gif"))
    {
        fprintf (stderr, "Can not load the image for demo!\n");
        exit (-1);
    }

    if (LoadBitmap (HDC_SCREEN, &car_bitmap, "res/car.gif"))
    {
        fprintf (stderr, "Can not load the image for demo!\n");
        exit (-1);
    }

    PathTrac = malloc(sizeof(PATHTRAC));
    memset(PathTrac, '0', sizeof(PATHTRAC));
    PathTrac->points = NULL;
    PathTrac->len = NULL;
    PathTrac->part = NULL;
    PathTrac->part_offset = NULL;
    PathTrac->off_lat = NULL;
    PathTrac->off_lon = NULL;
    PathTrac->alpha = NULL;
    
    PathTrac->points = malloc(10*sizeof(int));
    memset(PathTrac->points, '0', 10*sizeof(int));
    pb_num =1;
}

static int PathTracProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_INITDIALOG:
            SetNotificationCallback(GetDlgItem(hDlg, IDC_PATH_LIMIT), input_speed_enable);
            SetNotificationCallback(GetDlgItem(hDlg, IDC_PATH_MONITOR), input_speed_disable);
            SetNotificationCallback(GetDlgItem(hDlg, IDC_PATH_GAS), input_speed_disable);

            SendMessage(meagle_hwnd, ME_CLEAR, 0, 0);
            SendMessage(meagle_hwnd, ME_REFRESH, 0, 0);

            hSpeed = GetDlgItem(hDlg, IDC_PATH_SPEED);
            init_path_tracing_info();
            return 1;
        case MSG_COMMAND:
            switch (wParam)
            {
                case IDC_PATH_FPICK:
                    change_state(STATE_PATHTRACING);
                    hEdit = GetDlgItem(hDlg, IDC_PATH_FROM);
                    pickstatus =1;
                    break;
                case IDC_PATH_EPICK:
                    change_state(STATE_PATHTRACING);
                    hEdit = GetDlgItem(hDlg, IDC_PATH_END);
                    pickstatus =1;
                    break;
                case IDC_PATH_FSEARCH:
                    path_from_search(GetDlgItem(hDlg, IDC_PATH_FROM), hDlg);
                    break;
                case IDC_PATH_ESEARCH:
                    path_from_search(GetDlgItem(hDlg, IDC_PATH_END), hDlg);
                    break;
                case IDC_PATH_PBPICK:
                    change_state(STATE_PATHTRACING);
                    hEdit = GetDlgItem(hDlg, IDC_PATH_PB);
                    break;
                case IDOK:
                    if (get_info_from_dlg(hDlg)){
                        display_path_tracing(hDlg);
                        get_path_info(hDlg);
                    }else
                        free_all();
                case IDCANCEL:
                    DestroyMainWindowIndirect(hwnd_path);
                    break;
            }
           break;
        }
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

void open_path_window(HWND hParent, const char* lang)
{
    PopWinData.controls = CtrlInitProgress;
    hwnd_path = CreateMainWindowIndirect (&PopWinData, hParent, PathTracProc);
}


#define LINESIZE 20

/** Get  path info from path.info. 
 *  if success return 1, otherwise return 0 
**/
static int get_path_info_from_file()
{

    if (pb_num > 8)
        return 0;

    DOUBLE_POINT dp;
    char buf[50];
    char line[20];
    FILE *fp;
    char *p=NULL;
    int r;
    int point_num = 0;

	/* Open path info file. */
    if((fp=fopen("./res/path.info","r"))==NULL)
    {
	    fprintf(stderr,"Can't open path.info file!\n");
	    return 0;             
    }
    
    /*Get the first line which records number of all points*/
    fgets(line,LINESIZE,fp);
    point_num = atoi(line);

    int i = 0; 
    int index = 0;
    while(i<point_num)
    { 
        /* This line recordes point Type*/
	    fgets(line,LINESIZE,fp);	
	    p = strchr(line,'\n');	      
	    if(p) 	  *p = '\0';
	    //printf("line[%d]=%s\n",i,line);

	    if(!strcmp(line,"PT_START"))
	    {
	        index = 0;	    
	        PathTrac->points[0] = malloc(sizeof(PATH_POINT));
	        memset(PathTrac->points[0], '0', sizeof(PATH_POINT));
	        PathTrac->points[0]->type = PT_FROM;
	        PathTrac->points[index]->velocity = NULL;	
	     }
	     else 
	    {
	        index = pb_num;
	        PathTrac->points[pb_num] = malloc(sizeof(PATH_POINT));
	        memset(PathTrac->points[pb_num], '0', sizeof(PATH_POINT));
	        if(!strcmp(line,"PT_MONITOR"))
	        {
		        PathTrac->points[pb_num]->type = PT_MONITOR;
		        PathTrac->points[index]->velocity = NULL;
	        }
	        else if(!strcmp(line,"PT_GAS"))
	        {
		        PathTrac->points[pb_num]->type = PT_GAS;
		        PathTrac->points[index]->velocity = NULL;
	        }
	        else if(!strcmp(line,"PT_SPEED"))
	        {
		        PathTrac->points[pb_num]->type = PT_SPEED;
		        fgets(line,LINESIZE,fp);	
		        p = strchr(line,'\n');	      
		        if(p) 	  *p = '\0';
		        PathTrac->points[index]->velocity = strdup(line);
	        }
	        else if(!strcmp(line,"PT_END"))
	        {
		        PathTrac->points[pb_num]->type = PT_END;
		        PathTrac->points[index]->velocity = NULL;
	        }
	        else 
	        {
		        fprintf(stderr,"path.info error.\n");
		        fclose(fp);
		        return 0;
	        }
	        pb_num++;
	     }
         /*This line records point's name.*/
	     fgets(line,LINESIZE,fp);	
	     p = strchr(line,'\n');	      
	     if(p) 	  *p = '\0';
         PathTrac->points[index]->name = strdup(line);
	     //printf("line[%d]=%s\n",i,line);

         /*The next two lines record the longitude and latitude.*/
	     fgets(line,LINESIZE,fp);	
	     p = strchr(line,'\n');	      
	     if(p) *p = '\0';
	     dp.x = atof(line);

	     fgets(line,LINESIZE,fp);	
	     p = strchr(line,'\n');	      
	     if(p)  *p = '\0';
	     dp.y = atof(line);

	
	     //printf("%f,%f",dp.x,dp.y);
         PathTrac->points[index]->dpoint.x = dp.x;
         PathTrac->points[index]->dpoint.y = dp.y;

	     //printf("line[%d]=%s\n",i,line);
	    i++;  
      }
      pb_num--;
      fclose(fp);
      return 1;
}

void tracing()
{
    init_path_tracing_info();
    if(get_path_info_from_file())
    {
        display_path_tracing();
        get_path_info();
    }
}

