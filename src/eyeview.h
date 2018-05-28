/*
** $Id: eyeview.h,v 1.11 2006-10-23 02:35:13 yangyan Exp $
**
** Eagle eye view class define
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Yang Yan.
**
** Create date: 2006/08/23
*/

#ifndef _ME_EYE_VIEW_H_
#define _ME_EYE_VIEW_H_

#include "ogr.h"
#include "eye.h"
#include "view.h"

class MEEye;

class MEEyeView
{
public:
    MEEyeView(HWND hwnd, int width, int height);
    ~MEEyeView();

    void DrawEye (HDC hdc);
    void Refresh(void);

    static int _on_eye_message(HWND hwnd, int message, WPARAM wParam, LPARAM lParam);
private:
    MEEyeView(const MEEyeView & o);
    MEEyeView & operator=(const MEEyeView & o);

    void set_bound_to_eye(void);
    void set_bound_to_ortho(void);
    void get_map_center(double *eye_x0, double *eye_y0, double *len);

    HWND m_hwnd;
    HWND map_hwnd;
    HDC eye_mem_dc0;
    HDC eye_mem_dc1;
    int eye_width;
    int eye_height;
    POINT bound[6];
    double bound_ortho[10];
    bool eye_move;
    bool eye_click;
    bool is_move;
    
    POINT old_pts[4];

    MEEye *eye;
    int on_message(int message, WPARAM wParam, LPARAM lParam);
};

#endif /* _ME_EYE_VIEW_H_ */

