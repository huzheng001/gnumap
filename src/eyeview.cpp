/*
** $Id: eyeview.cpp,v 1.28 2006-10-23 02:35:13 yangyan Exp $
**
** Eagle eye view class implement
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Yang Yan.
**
** Create date: 2006/08/23
*/

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "eyeview.h"
#include "view.h"
#include "map.h"

int MEEyeView::_on_eye_message(HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
    MEEyeView *eyeview = (MEEyeView*)GetWindowAdditionalData2(hwnd);
    if(eyeview == NULL)
    {
        RECT r;
        GetClientRect(hwnd, &r);
        eyeview = new MEEyeView(hwnd, r.right-r.left, r.bottom-r.top);
        SetWindowAdditionalData2(hwnd, (DWORD)eyeview);
    }
    return eyeview->on_message(message, wParam, lParam);
}

int MEEyeView::on_message(int message, WPARAM wParam, LPARAM lParam)
{
    

    MEMap * memap = NULL;
    MEView *view = NULL;

    switch (message) {
        case MSG_CREATE:

            break;
        case ME_ADD_EYE:
            map_hwnd = lParam;
            view = (MEView*)GetWindowAdditionalData2(lParam);
            if (view){
                memap = view->memap;
                if (memap && memap->MapIsLoad()){
                    memap->AddEagleEye(eye);
                    eye->SetEyeMap(memap);
                    set_bound_to_eye();
                }
            }
            InvalidateRect(m_hwnd, NULL, FALSE);
            break;
        case ME_REMOVE_EYE:
            memap = eye->GetEyeMap();
            if (memap)
                memap->RemoveEagleEye(eye);
            break;
        case MSG_PAINT:
            {
                HDC hdc;
                hdc = BeginPaint(m_hwnd);
                DrawEye(hdc);
                EndPaint(m_hwnd, hdc);
            }
            break;
        case MSG_LBUTTONDOWN:
            memap = eye->GetEyeMap();
            if (memap && memap->MapIsLoad()) {
                eye_move = true;
                eye_click = true;
                is_move = false;
                SetCapture(m_hwnd);
                bound[4].x = LOWORD(lParam);
                bound[4].y = HIWORD(lParam);
                memset(&old_pts, 0, sizeof(POINT)*4);
                InvalidateRect(m_hwnd, NULL, FALSE);
            }
            break;
        case MSG_MOUSEMOVE:
            memap = eye->GetEyeMap();
            if (memap && memap->MapIsLoad()) {
                if (eye_move){
                    bound[5].x = LOWORD(lParam);
                    bound[5].y = HIWORD(lParam);
                    ScreenToClient(m_hwnd, &bound[5].x, &bound[5].y);
                    if ((ABS(bound[5].x-bound[4].x) > 4) ||(ABS(bound[5].y-bound[4].y) > 4)){
                        if (eye_click) {
                            InvalidateRect(m_hwnd, NULL, FALSE);
                        }

                        eye_click = false;
                        is_move = true;
                        bound[0].x = bound[3].x = bound[4].x;
                        bound[0].y = bound[1].y = bound[4].y;
                        bound[1].x = bound[2].x = bound[5].x;
                        bound[3].y = bound[2].y = bound[5].y;

                        InvalidateRect(m_hwnd, NULL, FALSE);
                    }
                }
            }
            break;
        case MSG_LBUTTONUP:
            memap = eye->GetEyeMap();
            if (memap && memap->MapIsLoad())
            {
                ReleaseCapture();
                if (eye_click){
                    int max_x, max_y, offset_x;
                    int min_x, min_y, offset_y;
                    max_x = bound[0].x;
                    max_y = bound[0].y;
                    min_x = bound[0].x;
                    min_y = bound[0].y;

                    int i;
                    for(i=1; i<4; i++)
                        max_x = MAX(max_x, bound[i].x);
                    for(i=1; i<4; i++)
                        max_y = MAX(max_y, bound[i].y);
                    for(i=1; i<4; i++)
                        min_x = MIN(min_x, bound[i].x);
                    for(i=1; i<4; i++)
                        min_y = MIN(min_y, bound[i].y);

                    offset_x = bound[4].x - (min_x + (max_x - min_x)/2);
                    offset_y = bound[4].y - (min_y + (max_y - min_y)/2);

                    for (i=0; i<4; i++)
                        bound[i].x += offset_x;
                    for (i=0; i<4; i++)
                        bound[i].y += offset_y;
                }
                double eye_x0, eye_y0;
                double pixel_len;
                set_bound_to_ortho();
                get_map_center(&eye_x0, &eye_y0, &pixel_len);

                if (!eye_click){
                    eye->GetEyeMap()->ResetNorth();
                    eye->GetEyeMap()->SetMapScale(pixel_len);
                }
                memap->GetEnv()->set_xy(memap->ortho_extent.min_x, memap->ortho_extent.max_y);
                int x, y;
                memap->GetEnv()->get_cs()->ortho_to_window_without_rotate(eye_x0, eye_y0, x, y);
                memap->GetEnv()->set_screen_xy(x, y);
                memap->GetEnv()->set_xy(eye_x0, eye_y0);


                eye_move = false;
                eye_click = false;
                is_move = false;
                InvalidateRect(m_hwnd, NULL, FALSE);
                InvalidateRect(map_hwnd, NULL, FALSE);
                
                memap->UpdateEyes();

            }
            break;

        case MSG_DESTROY:
            delete this;
            break;
        }
    return DefaultControlProc (m_hwnd, message, wParam, lParam);
}


MEEyeView::MEEyeView(HWND hwnd, int width, int height)
    :   m_hwnd(hwnd),map_hwnd(0),
        eye_mem_dc0(0),eye_mem_dc1(0),
        eye_width(width), eye_height(height),
        eye_move(false), eye_click(false), is_move(false), eye(NULL)
{

    memset(&old_pts, 0, sizeof(POINT)*4);
    memset(&bound, 0, sizeof(POINT)*6);
    memset(&bound_ortho, 0 , sizeof(double)*10);

    eye_mem_dc0 = CreateCompatibleDCEx(GetClientDC(m_hwnd), eye_width, eye_height);
    eye_mem_dc1 = CreateCompatibleDCEx(GetClientDC(m_hwnd), eye_width, eye_height);
    SetMemDCAlpha(eye_mem_dc1, MEMDC_FLAG_SRCALPHA, 60);

    eye = new MEEye(eye_mem_dc0,this, eye_width, eye_height);
    eye->SetDrawStatus(true);
}

MEEyeView::~MEEyeView()
{
    delete eye;

    if (eye_mem_dc0)
        DeleteCompatibleDC(eye_mem_dc0);

    if (eye_mem_dc1)
        DeleteCompatibleDC(eye_mem_dc1);
}

void MEEyeView::Refresh(void)
{
    set_bound_to_eye();
    InvalidateRect(m_hwnd, NULL, FALSE);
}

void MEEyeView::DrawEye (HDC hdc)
{
    int i;
    MEMap * memap = NULL;
    memap = eye->GetEyeMap();
    if (memap && memap->MapIsLoad()){
        if (!is_move && !eye_click) {
            SetBrushColor (eye_mem_dc1, RGB2Pixel(eye_mem_dc1, 0xFF, 0xFF, 0xFF));
            FillBox(eye_mem_dc1, 0, 0, eye_width, eye_height);

            SetBrushColor (eye_mem_dc1, RGB2Pixel(eye_mem_dc1, 0x0F, 0x0F, 0xFF));
            SetPenColor (hdc, RGB2Pixel(hdc, 0x00, 0x00, 0xFF));

            FillPolygon(eye_mem_dc1,bound,4);

            BitBlt(eye_mem_dc0, 0, 0, eye_width, eye_height, hdc, 0 ,0, 0);
            BitBlt(eye_mem_dc1, 0, 0, eye_width, eye_height, hdc, 0 ,0, 0);
            MoveTo(hdc, bound[3].x, bound[3].y);
            for (i=0; i<4; i++){
                if (bound[i].x == eye_width)
                    bound[i].x -= 1;
                if (bound[i].y == eye_height)
                    bound[i].y -= 1;
                LineTo(hdc, bound[i].x, bound[i].y);
            }
        } else{
            if (eye_click)
                BitBlt(eye_mem_dc0, 0, 0, eye_width, eye_height, hdc, 0 ,0, 0);
            else{
                SetPenColor(hdc, RGB2Pixel(hdc, 0xFF, 0xFF, 0x00));
                SetPenWidth(hdc, 1);
                SetRasterOperation (hdc, ROP_XOR);

                MoveTo(hdc, old_pts[3].x, old_pts[3].y);
                for (i=0; i<4; i++){
                    if (old_pts[i].x == eye_width)
                        old_pts[i].x -= 1;
                    if (old_pts[i].y == eye_height)
                        old_pts[i].y -= 1;
                    LineTo(hdc, old_pts[i].x, old_pts[i].y);
                }
                MoveTo(hdc, bound[3].x, bound[3].y);
                for (i=0; i<4; i++){
                    if (bound[i].x == eye_width)
                        bound[i].x -= 1;
                    if (bound[i].y == eye_height)
                        bound[i].y -= 1;
                    LineTo(hdc, bound[i].x, bound[i].y);
                }
                SetRasterOperation (hdc, ROP_SET);

                memcpy (&old_pts, &bound, sizeof(POINT)*4);
            }
        }
    } else {
        FillBox(hdc, 0, 0, eye_width, eye_height);
        eye->SetDrawStatus(true);
    }
}

void MEEyeView::set_bound_to_eye()
{
    MEMap *memap = eye->GetEyeMap();
    int width = memap->GetWidth();
    int height = memap->GetHeight();
    memap->WindowToOrtho(0, 0, &bound_ortho[0], &bound_ortho[1]); 
    memap->WindowToOrtho(width, 0, &bound_ortho[2], &bound_ortho[3]); 
    memap->WindowToOrtho(width, height, &bound_ortho[4], &bound_ortho[5]); 
    memap->WindowToOrtho(0, height, &bound_ortho[6], &bound_ortho[7]);
    
    eye->OrthoToEye(bound_ortho[0], bound_ortho[1], &(bound[0].x), &(bound[0].y)); 
    eye->OrthoToEye(bound_ortho[2], bound_ortho[3], &(bound[1].x), &(bound[1].y)); 
    eye->OrthoToEye(bound_ortho[4], bound_ortho[5], &(bound[2].x), &(bound[2].y)); 
    eye->OrthoToEye(bound_ortho[6], bound_ortho[7], &(bound[3].x), &(bound[3].y)); 
}



void MEEyeView::set_bound_to_ortho()
{
    eye->EyeToOrtho(bound[0].x, bound[0].y, &(bound_ortho[0]), &(bound_ortho[1])); 
    eye->EyeToOrtho(bound[1].x, bound[1].y, &(bound_ortho[2]), &(bound_ortho[3])); 
    eye->EyeToOrtho(bound[2].x, bound[2].y, &(bound_ortho[4]), &(bound_ortho[5])); 
    eye->EyeToOrtho(bound[3].x, bound[3].y, &(bound_ortho[6]), &(bound_ortho[7]));
}

void MEEyeView::get_map_center(double *eye_x0, double *eye_y0, double *len)
{
    int i, width;
    double max_x, min_x;
    double max_y, min_y;
    double pixel;
    max_x = bound_ortho[0];
    max_y = bound_ortho[1];
    min_x = bound_ortho[0];
    min_y = bound_ortho[1];

    for(i=2; i<8; i+=2)
        max_x = MAX(max_x, bound_ortho[i]);
    for(i=3; i<9; i+=2)
        max_y = MAX(max_y, bound_ortho[i]);
    for(i=2; i<8; i+=2)
        min_x = MIN(min_x, bound_ortho[i]);
    for(i=3; i<9; i+=2)
        min_y = MIN(min_y, bound_ortho[i]);

    *eye_x0 = min_x;
    *eye_y0 = max_y; 

    width = eye->GetEyeMap()->GetWidth();
    pixel = ABS(max_x-min_x)/width;
    *len = pixel;
}
