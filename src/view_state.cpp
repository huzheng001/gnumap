#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/fixedmath.h>

#include "view.h"
#include "eyeview.h"

static inline void refresh_some(HWND hwnd,
	   							int x1, int y1,
							   	int x2, int y2,
							   	int win_width)
{
    RECT rect;
    rect.top = MIN(y1, y2) -win_width;
    rect.left = MIN(x1, x2) -win_width;
    rect.bottom = MAX(y1, y2) +win_width;
    rect.right = MAX(x1, x2) +win_width;
    InvalidateRect( hwnd, &rect, FALSE );
}

static inline void refresh(HWND hwnd,
	   					   int x1, int y1,
						   int x2, int y2,
						   int win_width)
{
    const int inc = 20;
    int min_x, min_y, max_x, max_y;

	max_x = x1 > x2 ? (min_x = x2, x1) : (min_x = x1, x2);
	max_y = y1 > y2 ? (min_y = y2, y1) : (min_y = y1, y2);

    if(max_x - min_x < inc || max_y - min_y < inc)
    {
        refresh_some(hwnd, min_x, min_y, max_x, max_y, win_width);
        return;
    }

    float inc_x, inc_y;
    float tan = (max_x-min_x) / (float)(max_y-min_y);
    if(tan >= 1)
    {
        inc_y = inc;
        inc_x = inc_y * tan;
    }
    else
    {
        inc_x = inc;
        inc_y = inc_x / tan;
    }
    if(x1 > x2)
        inc_x = -inc_x;
    if(y1 > y2)
        inc_y = -inc_y;

    int now_x, now_y;
    float tmp_x = inc_x, tmp_y = inc_y; 
    do
    {
        refresh_some(hwnd, (int)(x1+inc_x-tmp_x), (int)(y1+inc_y-tmp_y),
                    (int)( x1+inc_x), (int)(y1+inc_y), win_width);
        now_x = (int)(min_x + abs((int)(inc_x-tmp_x)));
        now_y = (int)(min_y + abs((int)(inc_y-tmp_y)));
        inc_x += tmp_x;
        inc_y += tmp_y;
    }
    while(now_x <= max_x || now_y <= max_y);
}



//view state abstract class space

MEViewState::MEViewState(MEView* view)
                : view(view)
{
}

MEViewState::~MEViewState()
{
}

bool MEViewState::OpenMap(const char* filename)
{
   return view->memap->OpenMap(filename);
}

void MEViewState::CloseMap(void)
{
   view->memap->UnloadMap();
}

void MEViewState::OnPaint(HDC hdc)
{
    view->OnPaint(hdc);
}

//drag state space

MEViewDrag::MEViewDrag(MEView* view)
            : MEViewState(view),
		   	  key_down(false),
			  pre_x(0), pre_y(0),
			  move_x(0), move_y(0)
{
}

MEViewDrag::~MEViewDrag()
{
}

void MEViewDrag::OnMouseDown(int x, int y)
{
    key_down = true;
    pre_x = x;
    pre_y = y;
}

void MEViewDrag::OnMouseMove(int x, int y)
{
    if(key_down)
    {
        if(pre_x == x && pre_y == y)
            return;
        move_x = x - pre_x;
        move_y = y - pre_y;
        
        view->Refresh();
    }
}

void MEViewDrag::OnMouseUp(int x, int y)
{
    if(key_down)
    {
		key_down = false;
		view->memap->MoveMap(pre_x-x, pre_y-y);
        view->Refresh();
	}
}

void MEViewDrag::OnPaint(HDC hdc)
{
    if(key_down)
        return view->move_mem_dc(hdc, move_x, move_y);

    return MEViewState::OnPaint(hdc);
}

//rotate state space

MEViewRotate::MEViewRotate(MEView* view)
    : MEViewState(view), key_down(false),
    pre_x(0),pre_y(0),old_angle(0.0)
{
}

MEViewRotate::~MEViewRotate()
{
}

void MEViewRotate::OnMouseDown(int x, int y)
{
    key_down = true;
    pre_x = x;
    pre_y = y;
    old_angle = view->memap->GetRotateAngle();
    pre_angle = 0;
}


void MEViewRotate::OnMouseMove(int x, int y)
{
    if(key_down)
    {
        int o_x = view->win_width/2;
        int o_y = view->win_height/2;
        double x1 = x - o_x;
        double x2 = pre_x - o_x;
        double y1 = o_y - y;
        double y2 = o_y - pre_y;
        double angle1 = atan2(y1, x1);
        double angle2 = atan2(y2, x2);
        double angle = angle2 - angle1;

        if (angle > G_PI || angle < -G_PI)
            angle = angle2 - ((angle1 < 0) ? 2.0 * G_PI + angle1 : angle1 - 2.0 * G_PI);
        double abs_angle = angle - pre_angle;
        if (abs_angle < 0)
            abs_angle = -abs_angle;
        if (abs_angle < (G_PI/45))
            return;
        pre_angle = angle;
        double real_angle = old_angle + angle;
        if (real_angle < - G_PI)
            real_angle = 2.0 * G_PI + real_angle;
        else if (real_angle > G_PI)
            real_angle = real_angle - 2.0 * G_PI;

        view->memap->SetRotateAngle(real_angle);
        view->Refresh();
    }
}

void MEViewRotate::OnMouseUp(int x, int y)
{
    if(key_down)
    {
        key_down = false;

        view->memap->UpdateEyes();
    }
}

//zoom space

MEViewZoom::MEViewZoom(MEView* view)
            : MEViewState(view),
		   	  pre_x(0), pre_y(0),
			  des_x(0), des_y(0),
			  key_down(false)
{
   // FillBox (view->shdow_dc, 0, 0, view->win_width, view->win_height);
   memset(&old_pts, 0, sizeof(POINT)*5);
}

MEViewZoom::~MEViewZoom()
{
}

void MEViewZoom::OnMouseDown(int x, int y)
{
    if (!key_down){
        pre_x = des_x = x;
        pre_y = des_y = y;
        key_down = true;
    }
}

void MEViewZoom::OnMouseMove(int x, int y)
{
    if(key_down)
    {
        des_x = x;
        des_y = y;

        view->Refresh();
    }
}

void MEViewZoom::OnMouseUp(int x, int y)
{
    if (key_down)
    {
        key_down = false;
        view->Refresh();

        view->memap->UpdateEyes();
    }
}

void MEViewZoom::OnPaint(HDC hdc)
{

    if(key_down == true && pre_x != des_x && pre_y != des_y)
    {
    //    int blt_x, blt_y;
        POINT pts[5] = {{pre_x, pre_y},
		   				{des_x, pre_y},
					   	{des_x, des_y},
					   	{pre_x, des_y},
					   	{pre_x, pre_y}};
        
        SetPenColor(hdc, RGB2Pixel(hdc, 0x34, 0x66, 0xFC));
        SetPenWidth(hdc, 1);
        SetRasterOperation (hdc, ROP_XOR);
        PolyLineTo(hdc, old_pts, 5);
        PolyLineTo(hdc, pts, 5);
        SetRasterOperation (hdc, ROP_SET);

        memcpy (&old_pts, &pts, sizeof(POINT)*5);

  //      blt_x = pre_x < des_x ? pre_x : des_x;
   //     blt_y = pre_y < des_y ? pre_y : des_y;
      //  BitBlt (view->shdow_dc, 0, 0, abs(des_x-pre_x), abs(des_y-pre_y), hdc, blt_x, blt_y, 0);
    } else { 
        memset(&old_pts, 0, sizeof(POINT)*5);
        MEViewState::OnPaint(hdc);
    }
}

//zoom out space

MEViewZoomOut::MEViewZoomOut(MEView* view)
                : MEViewZoom(view)
{
}

MEViewZoomOut::~MEViewZoomOut()
{
}

void MEViewZoomOut::OnMouseUp(int x, int y)
{
    double pixel_length;
    int win_center_x, win_center_y;

    pixel_length = view->memap->GetMapScale();

    /*if zoom in to largest, then return directly. */
    if(pixel_length == view->memap->GetGlobalPixelLength())
    {
        MEViewZoom::OnMouseUp(x, y);
	    return;
    }

    if(pre_x == x || pre_y == y)
    {
        pixel_length *= 2;
        win_center_x = x;
        win_center_y = y;
    }
    else
    {
        double scale = ((double)view->win_width)/abs(x-pre_x);
        pixel_length *= scale;

        win_center_x =(int)((pre_x+x)/2);
        win_center_y =(int)((pre_y+y)/2);
    }

    view->memap->SetCenterWin(win_center_x, win_center_y);
    double ortho_center_x, ortho_center_y;
    view->memap->GetCenterOrtho(&ortho_center_x, &ortho_center_y);
    view->memap->SetMapScale(pixel_length);
    view->memap->SetCenterOrtho(ortho_center_x, ortho_center_y);

    MEViewZoom::OnMouseUp(x, y);
}

//zoom in space

MEViewZoomIn::MEViewZoomIn(MEView* view)
                : MEViewZoom(view)
{
}

MEViewZoomIn::~MEViewZoomIn()
{
}

void MEViewZoomIn::OnMouseUp(int x, int y)
{
    if (!key_down)
        return;
    double pixel_length;
    int win_center_x, win_center_y;

    pixel_length = view->memap->GetMapScale();

    /*if zoom in to largest, then return directly.*/ 
    if(pixel_length == view->memap->GetMinPixelLength())
    {
        MEViewZoom::OnMouseUp(x, y);
	    return;
    }

    if(pre_x == x || pre_y == y)
    {
        pixel_length /= 2;        
        win_center_x = x;
        win_center_y = y;
    }
    else
    {
        double scale = abs(x-pre_x)/((double)view->win_width);
        pixel_length *= scale;

        win_center_x =(int)((pre_x+x)/2);
        win_center_y =(int)((pre_y+y)/2);
    }

    view->memap->SetCenterWin(win_center_x, win_center_y);
    double ortho_center_x, ortho_center_y;
    view->memap->GetCenterOrtho(&ortho_center_x, &ortho_center_y);
    view->memap->SetMapScale(pixel_length);
    view->memap->SetCenterOrtho(ortho_center_x, ortho_center_y);

    MEViewZoom::OnMouseUp(x, y);
}

//distance space

MEViewDistance::MEViewDistance(MEView* view)
                : MEViewState(view), des_x(0), des_y(0),dis_finish(false),last_click(0)
{
}

MEViewDistance::~MEViewDistance()
{
}

void MEViewDistance::OnMouseDown(int x, int y)
{
    unsigned int cur_click = GetTickCount();
    if(last_click)
    {
        if(cur_click - last_click <= 40)
        {
            OnMouseDbclick(x, y);
            return;
        }
    }
    last_click = cur_click;
    POINT point = { x, y };
    select_points.push_back(point);
}

void MEViewDistance::OnMouseMove(int x, int y)
{
    if( !select_points.empty() && !dis_finish )
    {
        int old_x = des_x;
        int old_y = des_y;
        int tmp_x = select_points.back().x;
        int tmp_y = select_points.back().y;
        des_x = x;
        des_y = y;
        refresh(view->m_hwnd, tmp_x, tmp_y, old_x, old_y, 5);
        refresh(view->m_hwnd, tmp_x, tmp_y, des_x, des_y, 5);
    }
}

void MEViewDistance::OnMouseDbclick(int x, int y)
{
    char message[256] = {0};

    if( !select_points.empty() )
    {
        POINT point = { x, y };
        double distance = 0.0;
        select_points.push_back( point );
        std::vector<POINT>::iterator it = select_points.begin();
        std::vector<POINT>::iterator last = select_points.end();
        while( it != select_points.end() )
        {
            if(last != select_points.end())
                distance += view->memap->GetDistance( (*last).x,(*last).y, (*it).x, (*it).y);  
            last = it;
            it++;
        }
        dis_finish = true;
        sprintf(message, "The distance between these points is: %f metres.", distance); 
        MessageBox( view->m_hwnd, message, "Distance", MB_OK );
    }
    select_points.clear();
    view->Refresh();
    dis_finish = false;
}

void MEViewDistance::OnPaint(HDC hdc)
{
    MEViewState::OnPaint(hdc);
    
    if( !select_points.empty() )
    {
        SetPenColor(hdc, PIXEL_blue);
        SetPenWidth(hdc, 2);
        std::vector<POINT>::iterator it = select_points.begin();
        std::vector<POINT>::iterator last = select_points.end();
        while( it != select_points.end() )
        {
            if(last != select_points.end())
                LineEx( hdc, last->x, last->y, it->x, it->y );  
            last = it;
            it++;
        }
        if(last != select_points.end())
            LineEx( hdc, last->x, last->y, des_x, des_y );
    }
}
