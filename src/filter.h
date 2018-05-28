/*
** $Id: filter.h,v 1.4 2006-09-05 04:57:13 jpzhang Exp $
**
** Filter class define
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Hu Zheng.
**
** Create date: 2006/08/24
**
*/

#ifndef _ME_FILTER_H_
#define _ME_FILTER_H_

#include <vector>

#include "map.h"

using std::vector;

class MELayer;
//abstract class for filter
class MEFilter
{
public:
    MEFilter() {}
    virtual ~MEFilter() {}

    virtual bool FilterLayer(const MELayer* layer) const = 0;
    virtual bool FilterFeature(const MEMapFeature* feature) const = 0;
};

class MEFilterEx : public MEFilter
{
public:
    MEFilterEx();
    virtual ~MEFilterEx();

    bool FilterLayer(const MELayer* layer) const;
    bool FilterFeature(const MEMapFeature* feature) const;
    void AddFilter(MEFilter* filter);
    void ResetFilter(void);
private:
    vector<MEFilter*> filters;
};

class MEExtentFilter : public MEFilter
{
    MEExtent extent;
public:
    MEExtentFilter(double x1, double y1, double x2, double y2);
    virtual ~MEExtentFilter();

    bool FilterLayer(const MELayer* layer) const;
    bool FilterFeature(const MEMapFeature* feature) const;
    void SetExtent(double x1, double y1, double x2, double y2);
};

#endif /*_ME_FILTER_H_*/
