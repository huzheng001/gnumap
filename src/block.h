/*
** $Id: block.h,v 1.24 2006-09-30 01:09:35 jpzhang Exp $
**
** Classes for Block divide
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

#ifndef _ME_BLOCK_H_
#define _ME_BLOCK_H_

#include <vector>
#include <list>
#include <map>

#include "utils.h"
#include "dc.h"

class MEMap;
class MEMapLayer;
class MEMapFeature;
class MEMapBlockLayer;

class MEFeatureBlockSet;
class MEFilterEx;

class MEFeatureBlock 
{
public:
    std::map<int, MEMapBlockLayer*> layers;

    MEFeatureBlock(double x1, double y1, double x2, double y2, MEFeatureBlockSet *blockset, MEMap *map);
    ~MEFeatureBlock();

    void DrawLayer(int id, MEDC * dc, int draw_age);
    MEExtent * GetExtent(void){return (&extent);}
    MEFeatureBlockSet *GetBlockSet(void){return block_set;}
private:
    MEFeatureBlock(const MEFeatureBlock & o);
    MEFeatureBlock & operator=(const MEFeatureBlock & o);

    MEExtent extent;
    MEFeatureBlockSet *block_set;
    MEMap *memap;
};


class MEMapBlockLayer
{
public:    
    std::list<MEMapFeature*> features;

    MEMapBlockLayer(MEFeatureBlock *block);
    ~MEMapBlockLayer();

    void Draw(MEDC * dc, int draw_age);
private:
    MEMapBlockLayer(const MEMapBlockLayer & o);
    MEMapBlockLayer & operator=(const MEMapBlockLayer & o);

    MEFeatureBlock *m_block;
};


class MEFeatureBlockSet 
{
public:
    MEFeatureBlockSet(double min_x, double max_x, double min_y, double max_y, int feature_count, MEMap *memap);
    ~MEFeatureBlockSet();

    int GetBlockCount(void);
    void AddLayer(MEMapLayer *layer);
    void Draw(MEDC * dc, int draw_age);
    MEMap *GetMap(void);
    MEFilterEx *GetFilters(void);
private:
    MEFeatureBlockSet(const MEFeatureBlockSet & o);
    MEFeatureBlockSet & operator=(const MEFeatureBlockSet & o);

    void get_block_wh(double width, double height, int feature_count, int &nw, int &nh);
    std::vector<int>* get_blocks();
    MEMap *memap;
    MEFilterEx* filters;
    std::vector<MEFeatureBlock*> blocks;
};
#endif /* _ME_BLOCK_H_ */
