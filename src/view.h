/*
** $Id: view.h,v 1.43 2006-10-20 06:35:11 yangyan Exp $
**
** Map view class define
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

#ifndef _MEVIEW_H_
#define _MEVIEW_H_

#include "map.h"
#include "meagle.h"
#include "utils.h"


class MEViewState;
class KeySearcher;
class AreaSearcher;

class MEMap;

class MEView
{
public:
    MEView(HWND hwnd, int width, int height);
    ~MEView();

    void OnPaint(HDC hdc);

    void Refresh(RECT * rc = NULL);

    static int _on_message(HWND hwnd, int message, WPARAM wParam, LPARAM lParam);

private:
    MEView (const MEView & o);
    MEView & operator=(const MEView & o);
private:
    int on_message(int message, WPARAM wParam, LPARAM lParam);
    void move_mem_dc(HDC hdc, int x, int y);
    void change_state(MEViewState* state);
    void set_scale(double scale);
    double get_scale(void);
    void rotate (double alpha);
    void draw_widget(HDC hdc);

    MEMap* memap;
    MEViewState* state;

    HWND m_hwnd;
    HDC mem_dc;
    //HDC shdow_dc;

    BITMAP * np_bmp;
    BITMAP * sp_bmp;

    int win_width;
    int win_height;

    KeySearcher* key_search;
    AreaSearcher* area_search;


    friend class MEViewState;
    friend class MEViewDrag;
    friend class MEViewZoom;
    friend class MEViewZoomIn;
    friend class MEViewZoomOut;
    friend class MEViewDistance;
    friend class MEViewRotate;

    friend class MEEyeView;
};

//class view state 
class MEViewState
{
public:
    MEViewState(MEView* view);
    virtual ~MEViewState();

    bool OpenMap(const char* filename);
    void CloseMap(void);

    virtual void OnPaint(HDC hdc);

    virtual void OnMouseDown(int x, int y){}
    virtual void OnMouseMove(int x, int y){}
    virtual void OnMouseUp(int x, int y){}
    virtual void OnMouseDbclick(int x, int y){}

protected:
    MEView* view;
private:
    MEViewState(const MEViewState & o);
    MEViewState & operator=(const MEViewState & o);
};


//class drag
class MEViewDrag : public MEViewState
{
public:
    MEViewDrag(MEView* view);
    virtual ~MEViewDrag();

    void OnPaint(HDC hdc);

    void OnMouseDown(int x, int y);
    void OnMouseMove(int x, int y);
    void OnMouseUp(int x, int y);
private:
    MEViewDrag(const MEViewDrag & o);
    MEViewDrag & operator=(const MEViewDrag & o);

    bool key_down;
    int pre_x;
    int pre_y;
    int move_x;
    int move_y;
};

//class rotate
class MEViewRotate: public MEViewState
{
public:
    MEViewRotate(MEView* view);
    virtual ~MEViewRotate();

    void OnMouseDown(int x, int y);
    void OnMouseMove(int x, int y);
    void OnMouseUp(int x, int y);
private:
    MEViewRotate(const MEViewRotate & o);
    MEViewRotate & operator=(const MEViewRotate & o);

    bool key_down;
    int pre_x;
    int pre_y;
    double old_angle;
    double pre_angle;
};

//class zoom
class MEViewZoom : public MEViewState
{
public:
    MEViewZoom(MEView* view);
    virtual ~MEViewZoom();

    void OnPaint(HDC hdc);

    void OnMouseDown(int x, int y);
    void OnMouseMove(int x, int y);
    void OnMouseUp(int x, int y);
protected:
    int pre_x;
    int pre_y;
    int des_x;
    int des_y;

    bool key_down;
    POINT old_pts[5];
private:
    MEViewZoom(const MEViewZoom & o);
    MEViewZoom & operator=(const MEViewZoom & o);
};

//class zoom out
class MEViewZoomOut : public MEViewZoom
{
public:
    MEViewZoomOut(MEView* view);
    virtual ~MEViewZoomOut();

    void OnMouseUp(int x, int y);
private:
    MEViewZoomOut(const MEViewZoomOut & o);
    MEViewZoomOut & operator=(const MEViewZoomOut & o);
};

//class zoom int
class MEViewZoomIn : public MEViewZoom
{
public:
    MEViewZoomIn(MEView* view);
    virtual ~MEViewZoomIn();

    void OnMouseUp(int x, int y);
private:
    MEViewZoomIn(const MEViewZoomIn & o);
    MEViewZoomIn & operator=(const MEViewZoomIn & o);
};

//class distance
class MEViewDistance : public MEViewState
{
public:
    MEViewDistance(MEView* view);
    virtual ~MEViewDistance();

    void OnPaint(HDC hdc);

    void OnMouseDown(int x, int y);
    void OnMouseMove(int x, int y);
    void OnMouseDbclick(int x, int y);
private:
    MEViewDistance(const MEViewDistance & o);
    MEViewDistance & operator=(const MEViewDistance & o);

    std::vector<POINT> select_points;
    int des_x;
    int des_y;
    bool dis_finish;
    unsigned int last_click;
};

#endif /* _MEVIEW_H_ */
