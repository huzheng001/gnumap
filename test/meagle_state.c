#include <minigui/mgconfig.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "meagle.h"
#include "meagle_state.h"
#include "meagle_app.h"

int state = STATE_PAN;

/*car position*/
static int car_x = 375;
static int car_y = 257;

extern HWND meagle_hwnd;

static void start_moving_localize(Is_Reverse)
{
    RESULT result;
    result.GEOMDATA.PICTURE.width = 16;
    result.GEOMDATA.PICTURE.height = 16;
    if (Is_Reverse)
        result.GEOMDATA.PICTURE.pBitmap = &reverse_bitmap;
    else
        result.GEOMDATA.PICTURE.pBitmap = &moving_bitmap;

    result.GEOMDATA.PICTURE.win.x = car_x;
    result.GEOMDATA.PICTURE.win.y = car_y;
    result.GEOMDATA.PICTURE.isll = FALSE;
    result.feature_name = "Car";
    result.geom_type = PICTURE;

    SendMessage(meagle_hwnd, ME_ADD_RESULT, 0, (LPARAM) &result);
    SendMessage(meagle_hwnd, ME_REFRESH, 0, 0);
}

static void end_moving_localize()
{
    SendMessage (meagle_hwnd, ME_CLEAR, 0, 0);
    SetWindowText(status_dis_hwnd, "");
    SetWindowText(status_corner_hwnd, "");
}

void change_state(int new_state)
{ 
    if (state == new_state)
        if (state != STATE_MOVINGLOCALIZE && state != STATE_MOVINGREVERSE)
            return;
  
    if (state == STATE_MOVINGLOCALIZE)
        end_moving_localize();
    else if (state == STATE_MOVINGREVERSE)
        end_moving_localize();
    else if (state == STATE_DISTANCE){
        SendMessage (meagle_hwnd, ME_CLEAR, 0, 0);
        //end_path_tracing();
    }

    if (new_state == STATE_MOVINGLOCALIZE)
        start_moving_localize(FALSE);
    else if (new_state == STATE_MOVINGREVERSE)
        start_moving_localize(TRUE);
    else if (new_state == STATE_PATHTRACING)
    {
        //start_path_tracing();
    }
    state = new_state;
}

void set_status_ll_info()
{
    POINT point = { car_x, car_y};
    DOUBLE_POINT dp;
    char lat[50];
    char lon[50];

    SendMessage(meagle_hwnd, ME_WIN_TO_LL, (WPARAM)&point, (LPARAM)&dp);
    sprintf(lat, "latitude %f", dp.y);
    sprintf(lon, "longitude %f", dp.x);
    SetWindowText(status_dis_hwnd, lon);
    SetWindowText(status_corner_hwnd, lat);
}
