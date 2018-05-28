/*
** $Id: utils.h,v 1.7 2006-10-11 02:38:52 jpzhang Exp $
**
** Class template for Rect
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

#ifndef _ME_UTILS_H_
#define _ME_UTILS_H_

template<class T>
class TRect
{
public:
    T min_x, min_y, max_x, max_y;
    TRect()
        : min_x(0), min_y(0), max_x(0), max_y(0)
    {}
    TRect(T x1, T y1, T x2, T y2)
    {
        set(x1, y1, x2, y2);
    }
    void set(T x1, T y1, T x2, T y2)
    {
        if(x1 < x2) { min_x = x1; max_x = x2; }
        else        { min_x = x2; max_x = x1; }
        if(y1 < y2) { min_y = y1; max_y = y2; }
        else        { min_y = y2; max_y = y1; }
    }
    void sort()
    {
        if(min_x > max_x)
        {
            T tmp;
            tmp = min_x;
            min_x = max_x;
            max_x = tmp;
        }
        if(min_y > max_y)
        {
            T tmp;
            tmp = min_y;
            min_y = max_y;
            max_y = tmp;
        }
    }
    bool intersect(const TRect& rect) const
    {
        return ! (max_x < rect.min_x || min_x > rect.max_x ||
                    max_y < rect.min_y || min_y > rect.max_y);
    }
};

typedef TRect<double> MEExtent;

#define G_PI    3.14159265358979323846E0

#endif /* _ME_UTILS_H_ */
