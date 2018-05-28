/*
** $Id: eye.h,v 1.12 2006-09-25 08:48:42 yangyan Exp $
**
** Eagle eye class define
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Yang Yan.
**
** Create date: 2006/08/23
*/

#ifndef _ME_EYE_H_
#define _ME_EYE_H_

#include "dc.h"
#include "map.h"

class MEEyeView;

class MEEye
{
public:
    MEEye(HDC hdc, MEEyeView * view, int width, int height);
    ~MEEye();

    void UpdateView(void);//update the eyeview

    inline void SetEyeMap(MEMap* memap);
    inline MEMap* GetEyeMap(void) const;

    inline void SetEyePixelLength(double pixel_len);
    inline double GetEyePixelLength(void ) const;

    inline void SetEyeOrigin(double x0, double y0);
    inline void GetEyeOrigin(double * x0, double * y0);

    inline void OrthoToEye (double in_x, double in_y, int *out_x, int *out_y);
    inline void EyeToOrtho (int in_x, int in_y, double *out_x, double *out_y);

    bool GetDrawStatus(void) const{return back_is_draw;}
    void SetDrawStatus(bool draw){back_is_draw = draw;}

    HDC  GetBackDC(void)const{return back_dc;}
    void SetBackDC(HDC hdc){back_dc = hdc;}

    int GetWidth(void)const{return eye_width;}
    int GetHeight(void)const{return eye_height;}
 
private:
    MEEye(const MEEye & o);
    MEEye & operator=(const MEEye & o);
    
    MEMap *map;
    MEEyeView *m_view;
    HDC back_dc;
    int eye_width, eye_height;

    double eye_x0, eye_y0;
    double eye_pixel_length;
    
    bool back_is_draw;
};

void MEEye::SetEyeMap(MEMap* memap)
{
    map = memap;

    double max_width, max_height;
    max_width = memap->ortho_extent.max_x - memap->ortho_extent.min_x;
    max_height = memap->ortho_extent.max_y - memap->ortho_extent.min_y;
    SetEyeOrigin(memap->ortho_extent.min_x, memap->ortho_extent.max_y);
    SetEyePixelLength(MAX((max_width/eye_width),(max_height/eye_height)));
}

MEMap* MEEye::GetEyeMap(void) const
{ 
    return map;
}

void MEEye::SetEyeOrigin(double x0, double y0)
{
    eye_x0 = x0;
    eye_y0 = y0;
}
void MEEye::GetEyeOrigin(double *x0, double *y0)
{
    *x0 = eye_x0;
    *y0 = eye_y0;
}

void MEEye::SetEyePixelLength(double pixel_len)
{
    eye_pixel_length = pixel_len;
}

double MEEye::GetEyePixelLength(void) const
{ 
    return eye_pixel_length;
}

void MEEye::OrthoToEye(double in_x, double in_y, int *out_x, int *out_y)
{
    *out_x = (int)((in_x - eye_x0)/eye_pixel_length);
    *out_y = -(int)((in_y - eye_y0)/eye_pixel_length);
}

void MEEye::EyeToOrtho(int in_x, int in_y, double *out_x, double *out_y)
{
    *out_x = in_x * eye_pixel_length + eye_x0;
    *out_y =  eye_y0 - in_y * eye_pixel_length;
}

#endif /* _ME_EYE_H_ */
