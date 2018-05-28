/*
** $Id: view.cpp,v 1.95 2012-05-02 10:17:41 weiym Exp $
**
** Map view class implement
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Hu Zheng.
**
** Create date: 2006/08/14
**
*/

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "view.h"
#include "eyeview.h"
#include "searcher.h"
#include "layer.h"
#include "resource.h"


MEView::MEView(HWND hwnd, int width, int height)
    :m_hwnd(hwnd),win_width(width), win_height(height),key_search(NULL),area_search(NULL)
{
    mem_dc = CreateCompatibleDCEx(GetClientDC(hwnd), win_width, win_height);
//    shdow_dc = CreateCompatibleDC (GetClientDC(hwnd));
   // SetMemDCAlpha (shdow_dc, MEMDC_FLAG_SRCALPHA, 100);
  //  SetBrushColor (shdow_dc, RGB2Pixel(GetClientDC(hwnd), 0xE3, 0xE1, 0x53));

    memap = new MEMap(mem_dc, this, win_width, win_height);
    state = new MEViewState(this);

    np_bmp = new BITMAP;
    LoadBitmapFromMem(GetClientDC(hwnd), np_bmp, north_pointer, north_pointer_len, "gif");
    sp_bmp = new BITMAP;
    LoadBitmapFromMem(GetClientDC(hwnd), sp_bmp, scale_pointer, scale_pointer_len, "gif");
}

MEView::~MEView()
{
    if (key_search)
    {
        delete key_search;
        key_search = NULL;
    }

    if (area_search){
        delete area_search;
        area_search = NULL;
    }

    UnloadBitmap(np_bmp);
    delete np_bmp;
    UnloadBitmap(sp_bmp);
    delete sp_bmp;

    delete state;
    delete memap;
  //  DeleteCompatibleDC (shdow_dc);
    DeleteCompatibleDC(mem_dc);
}


void MEView::move_mem_dc(HDC hdc, int move_x, int move_y)
{

    SetBrushColor(hdc, RGB2Pixel(hdc, 0xFF, 0xFF, 0xFF));
    int begin_x = 0;
    int begin_y = 0;
    if(move_x < 0)
        begin_x = win_width + move_x;
    if(move_y < 0)
        begin_y = win_height + move_y;

    FillBox(hdc, begin_x, 0, abs(move_x), win_height);
    FillBox(hdc, 0, begin_y, win_width, abs(move_y));

    BitBlt(mem_dc, -move_x, -move_y, win_width, win_height, hdc, 0, 0, 0);
}


void MEView::change_state(MEViewState* state)
{
    this->state = state;
}

int MEView::on_message(int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) 
    {
    case MSG_CREATE:
        break;
    case MSG_DESTROY:
    {
        MEView *view = (MEView*)GetWindowAdditionalData2(m_hwnd);
        delete view;
        break;
    }
    case ME_CHANGE_TOOL:
        
        if(state != NULL)
        {
            delete state;
            state = NULL;
        }
        switch(wParam)
        {
        case ME_STATUS_DRAG:
            state = new MEViewDrag(this);
            break;
        case ME_STATUS_ZOOMIN:
            state = new MEViewZoomIn(this);
            break;
        case ME_STATUS_ZOOMOUT:
            state = new MEViewZoomOut(this);
            break;
        case ME_STATUS_DISTANCE:
            state = new MEViewDistance(this);
            break;
        case ME_STATUS_ROTATE:
            state = new MEViewRotate(this);
            break;
        default:
            state = new MEViewDrag(this);
            break;
        }
        break;
    case ME_OPEN_MAP:
        if (state != NULL) {
            if (memap) {
                memap->ClearResultFeatures();
                Refresh();
            }
            state->CloseMap();
            delete state;
        }
        state = new MEViewState(this);
        state->OpenMap((char*)lParam);
        SendMessage(m_hwnd, ME_CHANGE_TOOL, ME_STATUS_DRAG, 0);
        break;
    case ME_LOAD_MEAGLE_MAP:
        if (state != NULL) {
            if (memap) {
                memap->ClearResultFeatures();
                Refresh();
            }
            state->CloseMap();
            delete state;
        }
        state = new MEViewState(this);

        memap->LoadMegMap((const char*)lParam);
        Refresh();
        break;
    case ME_GET_MAPINFO:
        memap->GetMapInfo((const char***)lParam);
        break;
    case ME_LOAD_MAP:
        memap->LoadMap((const char**)lParam);
        Refresh();
        break;
    case ME_CLOSE_MAP:
        if (state != NULL) {
            if (memap) {
                memap->ClearResultFeatures();
                Refresh();
            }
            state->CloseMap();
            delete state;
        }
        state = new MEViewState(this);
        break;
    case ME_SAVE_MEAGLE_MAP:
        memap->SaveMap((const char*)lParam);
        break;
    case ME_GLOBAL:
        if (memap)
        {
            memap->SetMapGlobal();
            Refresh();

            memap->UpdateEyes();
        }
        break;
    case ME_CLEAR:
        if (memap)
        {
            memap->ClearResultFeatures();
            Refresh();
        }
        break;
    case ME_NORTH:
        if (memap)
        {
            memap->ResetNorth();
            Refresh();
            memap->UpdateEyes();
        }
        break;
    case ME_MOVE_X:
        if (memap)
        {
            if (wParam == ME_COORDINATE_WIN)
               memap->MoveMap((int)lParam , 0); 
            else if (wParam == ME_COORDINATE_LAT)
               memap->MoveMapLat(*((double *)lParam) , 0); 
                    
            Refresh();
            memap->UpdateEyes();
        }
        break;
    case ME_MOVE_Y:
        if (memap)
        {
            if (wParam == ME_COORDINATE_WIN)
               memap->MoveMap(0, (int)lParam); 
            else if (wParam == ME_COORDINATE_LAT)
               memap->MoveMapLat(0, *((double*)lParam)); 

            Refresh();
            memap->UpdateEyes();
        }
        break;
    case ME_SET_SCALE:
        if (memap && lParam){
            set_scale(*((double*)lParam));
            Refresh();
            memap->UpdateEyes();
        }
        break;
    case ME_GET_SCALE:
        if (memap && lParam)
            *((double*)lParam) = get_scale();
        
        break;
    case ME_ROTATE:
        if (memap){
            rotate(*(double*)lParam);
            Refresh();
        
            memap->UpdateEyes();
        }
        break;
    case ME_GET_LAYERCOUNT:
        if (memap){
           *(int*)lParam = memap->GetLayerCount();
        }
        break;
    case ME_GET_LAYERNAME:
        if (memap){
            *(char **)lParam = memap->GetLayerName(wParam);
        }
        break;
    case ME_GET_LAYERVISIBLE:
        if (memap){
            bool visible;
            visible = memap->GetLayerVisible(wParam);
            if (visible == true)
                *(int *)lParam =1;
            else
                *(int *)lParam =0;
        }
        break;
    case ME_CHANGE_LAYERINDEX:
        if (memap){
            memap->SetLayerIndex((char *)lParam, wParam);
        }
        break;
    case ME_CHANGE_LAYERVISIBLE:
        if (memap){
            if (lParam == 1)
                memap->SetLayerVisible(wParam,true);
            else
                memap->SetLayerVisible(wParam,false);
        }
        break;
    case ME_UPDATE_EYE:
        if(memap){
            memap->UpdateEyes(true);
        }
        break;
    case ME_KEY_SEARCH:
        if(((char*)lParam)[0] != '\0')
        {
            if (key_search != NULL)
            {
                delete key_search;
                key_search = NULL;
            }
            key_search = new KeySearcher( (char*)lParam );
            if(!wParam)
                memap->SearchAllLayers(key_search);
            else
            {
                char* layer_name = (char *)wParam;
                memap->SearchSepecialLayer(layer_name, key_search);
            }
        }
        break;

    case ME_GET_KEYRESULTCOUNT:
        if (key_search != NULL)
            return key_search->GetResultSize();
        else
            return 0;

        break;
    case ME_GET_KEYRESULTITEM:
        if (key_search != NULL)
        {
            KEYWORD_SEARCH* result = (KEYWORD_SEARCH*)lParam;
            const KeySearchResult* search_result = key_search->GetResult(wParam);
            result->feature_name = strdup(search_result->feature_name.c_str());
            result->latitude = search_result->latitude;
            result->longitude = search_result->longitude;
        }
        break;
    case ME_AREA_SEARCH:
        {
            if (area_search != NULL){
                delete area_search;
                area_search = NULL;
            }
            AREA_SEARCH* search = (AREA_SEARCH*)lParam;
            area_search = new AreaSearcher (search->latitude,
				  						 	search->longitude,
										   	search->distance);
            if(!wParam)
                memap->SearchAllLayers(area_search);
            else
                memap->SearchSepecialLayer((const char*)wParam, area_search);
        }
        break;
    case ME_GET_AREARESULTCOUNT:
        if (area_search != NULL)
            return area_search->GetResultSize();
        else
            return 0;

        break;
    case ME_GET_AREARESULTITEM:
        if (area_search != NULL){
            KEYWORD_SEARCH* result = (KEYWORD_SEARCH*)lParam;
            const KeySearchResult* search_result = area_search->GetResult(wParam);
            result->feature_name = strdup(search_result->feature_name.c_str());
            result->latitude = search_result->latitude;
            result->longitude = search_result->longitude;
        }
        break;
    case ME_ADD_RESULT:
        {
            RESULT* draw_key_result = (RESULT*)lParam;
            int frid = memap->AddResult(draw_key_result);
            if (wParam != 0)
                *(int *)wParam = frid;
        }
        break;
    case ME_REMOVE_RESULT:
        memap->RemoveResult(lParam);
        break;
    case ME_REFRESH:
        Refresh((RECT *)lParam);
        break;
    case ME_WIN_TO_LL:
    {
        if (memap && memap->MapIsLoad())
        {
            POINT *p = (POINT *)wParam;
            MEDoublePoint *dp = (MEDoublePoint *)lParam;
            memap->WindowToLatlong(p->x, p->y, &(dp->x), &(dp->y));
        }
        break;
    }
    case ME_LL_TO_WIN:
    {
        if (memap && memap->MapIsLoad())
        {
            MEDoublePoint *p = (MEDoublePoint *)wParam;
            POINT *dp = (POINT *)lParam;
            memap->LatlongToWindow(p->x, p->y, &(dp->x), &(dp->y));
        }
        break;
    }
    case ME_SETCENTER_LAT:
        if (memap)
            memap->SetCenterLat (*(double*)wParam, *(double*)lParam);
        break;
    case ME_SET_LAYERFONT:
        if (memap && ((const char*)wParam != NULL)) {
            if (memap->GetLayerIndex((const char*)wParam) > 0){
                MEFont * ft = new MEFont((LOGFONT*)lParam);
                // shouldn't delete ,because layer object will delete it automatic.
                memap->SetLayerFont ((const char*)wParam, ft); 
            }
        }
        break;
    case MSG_PAINT:
    {
        HDC hdc;
        hdc = BeginPaint(m_hwnd);
        state->OnPaint(hdc);
        draw_widget(hdc);
        EndPaint(m_hwnd, hdc);
        break;
    }
    case MSG_LBUTTONDOWN:
        state->OnMouseDown(LOWORD(lParam), HIWORD(lParam));
        break;
    case MSG_MOUSEMOVE:
        state->OnMouseMove(LOWORD(lParam), HIWORD(lParam));
        break;
    case MSG_LBUTTONUP:
        state->OnMouseUp(LOWORD(lParam), HIWORD(lParam));
        break;
    case MSG_LBUTTONDBLCLK:
        state->OnMouseDbclick(LOWORD(lParam), HIWORD(lParam));
        break;
    }
    
    return DefaultControlProc (m_hwnd, message, wParam, lParam);
}

int MEView::_on_message(HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
    MEView *view = (MEView*)GetWindowAdditionalData2(hwnd);
    if(view == NULL)
    {
        RECT r;
        GetWindowRect(hwnd, &r);
        view = new MEView(hwnd, r.right-r.left, r.bottom-r.top);
        SetWindowAdditionalData2(hwnd, (DWORD)view);
    }
    return view->on_message(message, wParam, lParam);
}

void MEView::OnPaint(HDC hdc)
{
    if (memap == NULL)
    {
        FillBox(hdc, 0, 0, win_width, win_height);
        return;
    }
    memap->DrawMap();
    BitBlt(mem_dc, 0, 0, win_width, win_height, hdc, 0, 0, 0);
}

void MEView::Refresh(RECT * rc)
{
    InvalidateRect(m_hwnd, rc, FALSE);
}

void MEView::set_scale(double scale)
{
    double ortho_center_x, ortho_center_y;
    memap->GetCenterOrtho(&ortho_center_x, &ortho_center_y);
    memap->SetMapScale(scale);
    memap->SetCenterOrtho(ortho_center_x, ortho_center_y);
}

double MEView::get_scale(void)
{
    return memap->GetMapScale();
}

void MEView::rotate(double alpha)
{
    double old_angle = memap->GetRotateAngle();
    double real_angle = old_angle + alpha;
    if (real_angle < - G_PI)
        real_angle = 2.0 * G_PI + real_angle;
    else if (real_angle > G_PI)
        real_angle = real_angle - 2.0 * G_PI;
    memap->SetRotateAngle(real_angle);
}

void MEView::draw_widget (HDC hdc)
{
    double angle = memap->GetRotateAngle();
    double scale = np_bmp->bmHeight / (win_height/10);
    double pixel_len = get_scale()*(sp_bmp->bmWidth);
    char buf[20] = {0};

    if (scale < 1)
        RotateScaledBitmap (hdc, np_bmp, 0,(win_height - np_bmp->bmHeight-5),
                            (int)(angle*180/G_PI*64),np_bmp->bmWidth, np_bmp->bmHeight);
    else
        RotateScaledBitmap (hdc, np_bmp, 0,(win_height - np_bmp->bmHeight-5),
                            (int)(angle*180/G_PI*64),(int)(np_bmp->bmWidth/scale), (int)(np_bmp->bmHeight/scale));

    FillBoxWithBitmap(hdc,(win_width - sp_bmp->bmWidth - 37) , 20, sp_bmp->bmWidth, sp_bmp->bmHeight, sp_bmp);

    if (memap && memap->MapIsLoad()){
        SetBkMode (hdc, BM_TRANSPARENT);
        sprintf(buf," %.3f m",pixel_len);
        //sprintf(buf," 1: %.3f",pixel_len);
        TextOut(hdc, (win_width - sp_bmp->bmWidth - 42), 30, buf);
    }
}

