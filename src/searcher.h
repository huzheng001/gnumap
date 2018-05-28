/*
** $Id: searcher.h,v 1.6 2006-09-04 03:49:29 jpzhang Exp $
**
** Classes define for Search 
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Hu Zheng.
**
** Create date: 2006/08/11
*/

#ifndef __SEARCHER_H__
#define __SEARCHER_H__

#include "map.h"

#include <list>
#include <string>

class MELayer;

struct KeySearchResult
{
    long feature_id;
    std::string layer_name;
    std::string feature_name;
    double latitude;
    double longitude;
};

//abstract class for searcher
class MESearcher
{
public:
    MESearcher();
    virtual ~MESearcher();

    virtual void SearchLayer(MELayer* layer);
    virtual void SearchFeature(MEMapFeature* feature) = 0;
};

//keyword search
class KeySearcher : public MESearcher
{
public:
    KeySearcher(const char *key_string);
    virtual ~KeySearcher();

    void SearchFeature(MEMapFeature* feature);
    const KeySearchResult* GetResult(int index) const;
    int GetResultSize(void) const;
protected:
    KeySearcher(){}
    std::string key_string;
    std::vector<KeySearchResult*> results;
};

//area search
class AreaSearcher : public KeySearcher
{
public:
    AreaSearcher(double latitude, double longitude, int distance);
    virtual ~AreaSearcher();

    void SearchFeature(MEMapFeature* feature);
private:
    double latitude;
    double longitude;
    int distance;
};

#endif /* __SEARCHER_H__ */
