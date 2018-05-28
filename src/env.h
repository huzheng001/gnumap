/*
** $Id: env.h,v 1.40 2006-11-28 05:31:00 jpzhang Exp $
**
** Classes for coordinate conversion and environment
**
** Copyright (C) 2006 Feynman Software.
**
** Create date: 2006/08/09
*/

#ifndef _ME_ENV_H_
#define _ME_ENV_H_

#include "spatialref.h"
#include "dc.h"
#include "utils.h"

class MECS;

class MEEnv
{
public:
    MEEnv(double ll_long, double ll_lat, double win_x0, double win_y0, double win_pixel_length);
    ~MEEnv();

    void set_latlong_spatial(MESpatialReference* spatial);
    void set_ortho_spatial(MESpatialReference* spatial);
    void set_ortho_spatial(double ll_long, double ll_lat);
    inline MESpatialReference* get_latlong_spatial();
    inline MESpatialReference* get_ortho_spatial();
    inline double get_x0();
    inline double get_y0();
    inline int get_screen_x0();
    inline int get_screen_y0();
    inline double get_pixel_length();
    inline double get_pixel_length_reciprocal();
    inline void set_xy(double x, double y);
    inline void set_screen_xy(int x, int y);
    inline void set_pixel_length(double len);
    inline void set_rotate_point(int x, int y);
    inline void set_rotate_angle(double alpha);
    inline int get_rotate_point_x();
    inline int get_rotate_point_y();
    inline double get_rotate_angle();

    inline MECS* get_cs();
private:
    MEEnv (const MEEnv & o);
    MEEnv & operator=(const MEEnv & o);

    MESpatialReference* latlong_spatial;
    MESpatialReference* ortho_spatial;

    double x0, y0;
    int x0_s, y0_s;
    double pixel_length;
    double pixel_length_reciprocal;
    int rotate_point_x, rotate_point_y;
    struct Fraction {
        int a;
        int b;
    };
    static struct Fraction sin_table[128];
    int rotate_cos_a, rotate_cos_b, rotate_sin_a, rotate_sin_b;
    double rotate_angle;
    MECS *cs;
    friend class MECS;
};

class MECS
{
private:
    MEEnv* m_env;
    MECoordinateTransformation* trans_ll_ortho;
    MECoordinateTransformation* trans_ortho_ll;
    inline int mul_div(int x, int m, int d);
public:
    MECS(MEEnv *env);
    ~MECS();
    inline MECoordinateTransformation* get_trans_ll_ortho();
    inline MECoordinateTransformation* get_trans_ortho_ll();
    inline void latlong_to_ortho  (const int &num, double* x, double* y);
    inline void latlong_to_ortho  (const double &in_x, const double &in_y, double &out_x, double &out_y);

    inline void ortho_to_latlong  (const int &num, double* x, double* y);
    inline void ortho_to_latlong  (const double &in_x, const double &in_y, double &out_x, double &out_y);

    //inline void ortho_to_window   (const int &num, double* x, double* y);
    inline void ortho_to_window   (const double &in_x, const double &in_y, int   &out_x, int   &out_y);
    inline void ortho_to_window_without_rotate   (const double &in_x, const double &in_y, int   &out_x, int   &out_y);
    inline void ortho_to_window   (int num, const MEDoublePoint * in_array, MEPoint * out_array);
    
    inline void screen_to_window   (const int &in_x, const int &in_y, int   &out_x, int   &out_y);
    inline void screen_to_window(int num, const MEPoint * in_array, MEPoint * out_array);

    //inline void window_to_ortho   (const int &num, double* x, double* y);
    inline void window_to_ortho   (int in_x,int in_y, double &out_x, double &out_y);
    inline void window_to_ortho_without_rotate   (int in_x, int in_y, double &out_x, double &out_y);

    //inline void latlong_to_window (const int &num, double* x, double* y);
    inline void latlong_to_window (const double &in_x, const double &in_y, int   &out_x, int   &out_y);

    //inline void window_to_latlong (const int &num, double* x, double* y);
    inline void window_to_latlong (int in_x, int in_y, double &out_x, double &out_y);
};

//MEEnv spacial
MESpatialReference* MEEnv::get_latlong_spatial()
{
    return latlong_spatial;
}

MESpatialReference* MEEnv::get_ortho_spatial()
{
    return ortho_spatial;
}

double MEEnv::get_x0()
{
    return x0;
}

double MEEnv::get_y0()
{
    return y0;
}

int MEEnv::get_screen_x0()
{
    return x0_s;
}

int MEEnv::get_screen_y0()
{
    return y0_s;
}

double MEEnv::get_pixel_length()
{
    return pixel_length;
}

double MEEnv::get_pixel_length_reciprocal()
{
    return pixel_length_reciprocal;
}

void MEEnv::set_xy(double x, double y)
{
    x0 = x;
    y0 = y;
}

void MEEnv::set_screen_xy(int x, int y)
{
    x0_s = x;
    y0_s = y;
}

void MEEnv::set_pixel_length(double len)
{
    pixel_length = len;

    pixel_length_reciprocal = (pixel_length==0)?0.0:(1/(double)pixel_length);
}

void MEEnv::set_rotate_point(int x, int y)
{
    rotate_point_x = x;
    rotate_point_y = y;
}

void MEEnv::set_rotate_angle(double alpha)
{
    if (rotate_angle == alpha)
        return;
    rotate_angle = alpha;

    int index;
    int quad;
    index = (int)(128 * (alpha) / (G_PI * 0.5));
    quad = ( index >> 7 ) & 1;
    if (alpha >= 0)
        quad += 2;
    index &= 127;
    switch ( quad )
    {
        case 2:
            rotate_sin_a = sin_table[index].a;
            rotate_sin_b = sin_table[index].b;
            rotate_cos_a = sin_table[127-index].a;
            rotate_cos_b = sin_table[127-index].b;
            break;
        case 3:
            rotate_sin_a = sin_table[127-index].a;
            rotate_sin_b = sin_table[127-index].b;
            rotate_cos_a = sin_table[index].a;
            rotate_cos_b = sin_table[index].b;
            break;
        case 0:
            rotate_sin_a = sin_table[index].a;
            rotate_sin_b = sin_table[index].b;
            rotate_cos_a = sin_table[127-index].a;
            rotate_cos_b = sin_table[127-index].b;
            break;
        case 1:
            rotate_sin_a = sin_table[127-index].a;
            rotate_sin_b = sin_table[127-index].b;
            rotate_cos_a = sin_table[index].a;
            rotate_cos_b = sin_table[index].b;
            break;
    }
    if (sin(alpha)<0)
        rotate_sin_a = -rotate_sin_a;
    if (cos(alpha)<0)
        rotate_cos_a = -rotate_cos_a;
}

int MEEnv::get_rotate_point_x()
{
    return rotate_point_x;
}

int MEEnv::get_rotate_point_y()
{
    return rotate_point_y;
}

double MEEnv::get_rotate_angle()
{
    return rotate_angle;
}

MECS* MEEnv::get_cs()
{
    return cs;
}


//MECS spacial

int MECS::mul_div(int x, int m, int d)
{
    signed long long x0 = x;
    x0 = x0*m/d;
    return (int)x0;
}

MECoordinateTransformation* MECS::get_trans_ll_ortho()
{
    return trans_ll_ortho;
}

MECoordinateTransformation* MECS::get_trans_ortho_ll()
{
    return trans_ortho_ll;
}

void MECS::latlong_to_ortho(const int &num, double* x, double* y)
{
    get_trans_ll_ortho()->TransformEx(num, x, y);
}

void MECS::latlong_to_ortho(const double &in_x, const double &in_y, double &out_x, double &out_y)
{
    out_x = in_x; out_y = in_y;
    latlong_to_ortho(1, &out_x, &out_y);
}

void MECS::ortho_to_latlong(const int &num, double* x, double* y)
{
    get_trans_ortho_ll()->Transform(num, x, y);
}

void MECS::ortho_to_latlong(const double &in_x, const double &in_y, double &out_x, double &out_y)
{
    out_x = in_x; out_y = in_y;
    ortho_to_latlong(1, &out_x, &out_y);
}

void MECS::ortho_to_window(int num, const MEDoublePoint * in_array, MEPoint * out_array)
{
    double out_x1=0;
    double out_y1=0;

    if (m_env->rotate_angle == 0.0)
        for (int i=0; i < num; i++) {
            out_array[i].x  = (int)((in_array[i].x-m_env->x0)*(m_env->pixel_length_reciprocal));
            out_array[i].y  = -(int)((in_array[i].y-m_env->y0)*(m_env->pixel_length_reciprocal));
        }
    else
        for (int i=0; i < num; i++) {
            out_x1 = ((in_array[i].x-m_env->x0)*(m_env->pixel_length_reciprocal));
            out_y1 = -((in_array[i].y-m_env->y0)*(m_env->pixel_length_reciprocal));

            out_x1 = out_x1-m_env->rotate_point_x;
            out_y1 = out_y1-m_env->rotate_point_y;
            out_array[i].x = (int)(((out_x1*m_env->rotate_cos_a/m_env->rotate_cos_b)-
                            (out_y1*m_env->rotate_sin_a/m_env->rotate_sin_b))+m_env->rotate_point_x);
            out_array[i].y = (int)(((out_y1*m_env->rotate_cos_a/m_env->rotate_cos_b)+
                            (out_x1*m_env->rotate_sin_a/m_env->rotate_sin_b))+m_env->rotate_point_y);
        }
}

void MECS::screen_to_window(int num, const MEPoint * in_array, MEPoint * out_array)
{
    if (m_env->rotate_angle == 0.0)
    {
        for (int i=0; i < num; i++) {
            out_array[i].x  = in_array[i].x-m_env->x0_s;
            out_array[i].y  = in_array[i].y-m_env->y0_s;
        }
    }
    else
    {
        int out_x1;
        int out_y1;
        for (int i=0; i < num; i++) {
            out_x1 = in_array[i].x-m_env->x0_s;
            out_y1 = in_array[i].y-m_env->y0_s;

            out_x1 = out_x1-m_env->rotate_point_x;
            out_y1 = out_y1-m_env->rotate_point_y;
            out_array[i].x = (((mul_div(out_x1,m_env->rotate_cos_a,m_env->rotate_cos_b))-
                        (mul_div(out_y1,m_env->rotate_sin_a,m_env->rotate_sin_b)))+m_env->rotate_point_x);
            out_array[i].y = (((mul_div(out_y1,m_env->rotate_cos_a,m_env->rotate_cos_b))+
                        (mul_div(out_x1,m_env->rotate_sin_a,m_env->rotate_sin_b)))+m_env->rotate_point_y);
        }
    }
}

void MECS::screen_to_window(const int &in_x, const int &in_y, int &out_x, int &out_y)
{
    if (m_env->rotate_angle == 0.0)
    {
        out_x = in_x-m_env->x0_s;
        out_y = in_y-m_env->y0_s;
    }
    else
    {
        int out_x1;
        int out_y1;
        out_x1 = in_x-m_env->x0_s;
        out_y1 = in_y-m_env->y0_s;
        out_x1 = out_x1-m_env->rotate_point_x;
        out_y1 = out_y1-m_env->rotate_point_y;
        out_x = (((mul_div(out_x1,m_env->rotate_cos_a,m_env->rotate_cos_b))-
                    (mul_div(out_y1,m_env->rotate_sin_a,m_env->rotate_sin_b)))+m_env->rotate_point_x);
        out_y = (((mul_div(out_y1,m_env->rotate_cos_a,m_env->rotate_cos_b))+
                    (mul_div(out_x1,m_env->rotate_sin_a,m_env->rotate_sin_b)))+m_env->rotate_point_y);
    }
}

void MECS::ortho_to_window(const double &in_x, const double &in_y, int &out_x, int &out_y)
{
    if (m_env->rotate_angle == 0.0)
    {
        out_x = (int)((in_x-m_env->x0)*(m_env->pixel_length_reciprocal));
        out_y = (int)-((in_y-m_env->y0)*(m_env->pixel_length_reciprocal));
    }
    else
    {
        double out_x1 = ((in_x-m_env->x0)*(m_env->pixel_length_reciprocal));
        double out_y1 = -((in_y-m_env->y0)*(m_env->pixel_length_reciprocal));

        out_x1 = out_x1-m_env->rotate_point_x;
        out_y1 = out_y1-m_env->rotate_point_y;
        out_x = (int)(((out_x1*m_env->rotate_cos_a/m_env->rotate_cos_b)-
                        (out_y1*m_env->rotate_sin_a/m_env->rotate_sin_b))+m_env->rotate_point_x);
        out_y = (int)(((out_y1*m_env->rotate_cos_a/m_env->rotate_cos_b)+
                        (out_x1*m_env->rotate_sin_a/m_env->rotate_sin_b))+m_env->rotate_point_y);
    }
}

void MECS::ortho_to_window_without_rotate(const double &in_x, const double &in_y, int &out_x, int &out_y)
{
    out_x = (int)((in_x-m_env->x0)*(m_env->pixel_length_reciprocal));
    out_y = (int)-((in_y-m_env->y0)*(m_env->pixel_length_reciprocal));
}

void MECS::window_to_ortho(int in_x, int in_y, double &out_x, double &out_y)
{
    double out_x1 = in_x-m_env->rotate_point_x;
    double out_y1 = in_y-m_env->rotate_point_y;

    double out_x2 = (out_x1*m_env->rotate_cos_a/m_env->rotate_cos_b)+
            (out_y1*m_env->rotate_sin_a/m_env->rotate_sin_b)+m_env->rotate_point_x;
    double out_y2 = (out_y1*m_env->rotate_cos_a/m_env->rotate_cos_b)-
            (out_x1*m_env->rotate_sin_a/m_env->rotate_sin_b)+m_env->rotate_point_y;

    out_x =  (out_x2*m_env->pixel_length)+m_env->x0;
    out_y =  (m_env->y0-(out_y2*m_env->pixel_length));
}

void MECS::window_to_ortho_without_rotate(int in_x, int in_y, double &out_x, double &out_y)
{
    out_x =  ((double)in_x)*m_env->pixel_length+m_env->x0;
    out_y =  m_env->y0-((double)in_y)*m_env->pixel_length;
}

void MECS::latlong_to_window(const double &in_x, const double &in_y, int   &out_x, int   &out_y)
{
    double tmp_x, tmp_y;
    latlong_to_ortho(in_x, in_y, tmp_x, tmp_y);
    ortho_to_window(tmp_x, tmp_y, out_x, out_y);
}

void MECS::window_to_latlong(int in_x, int in_y, double &out_x, double &out_y)
{
    window_to_ortho(in_x, in_y, out_x, out_y);
    ortho_to_latlong(out_x, out_y, out_x, out_y);
}

#endif /* _ME_ENV_H_ */
