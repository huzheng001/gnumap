/*
** $Id: layer.h,v 1.31 2006-10-31 01:53:22 jpzhang Exp $
**
** Classes for Layer
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Hu Zheng.
**
** Create date: 2006/08/11
*/

#ifndef _ME_LAYER_H_
#define _ME_LAYER_H_

#include <map>
#include <list>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>

#include "map.h"

class MEResultFeature;
class METextFeature;
class MEMapFeature;
class MEMap;


//This class is the abstract class of the layer.
class MELayer
{
public:
    MELayer(MEMap* parent_map);
    virtual ~MELayer();

    // return the pointer of the map.
    virtual char * GetName(void) const =0;
    virtual void Search(MESearcher* searcher)=0;
    virtual void Draw(MEDC* dc)=0;

    MEMap* GetMap(void) const {return map;}
    void SetLayerFont (MEFont * new_font); //new_font must create by new key word.
    MEFont * GetLayerFont (void)const {return layer_font;}
protected:
    MEMap* map;
    MEFont * layer_font;
private:
    MELayer(const MELayer & o);
    MELayer & operator=(const MELayer & o);
};


//This class is the map layer
class MEMapLayer : public MELayer
{
    typedef std::map<long, MEMapFeature*> FeaturePool;
public:
    MEMapLayer(MEMap* parent_map, OGRLayer* layer, int id=0);
    MEMapLayer(MEMap* parent_map, char *p, char *name, int id, std::vector<const char *> &Styles);
    virtual ~MEMapLayer();

    char * GetName(void) const;
    void Draw(MEDC* dc);

    void Search(MESearcher* searcher);

    void FeatureRewind();
    MEMapFeature* GetNextFeature();
    MEMapFeature* GetFeatureByFid(long fid);
    int GetFeatureCount(void);
    bool GetLayerVisible(void)const{ return layer_visible;}
    void SetLayerVisible(bool visible){layer_visible = visible;}

    int GetLayerId(void)const{return m_id;}
    void SetLayerId(int id){m_id = id;}
    void UpdateScreenPoint();

private:
    MEMapLayer(const MEMapLayer & o);
    MEMapLayer & operator=(const MEMapLayer & o);

    bool layer_visible;
    char * layer_name;
    int m_id;

    FeaturePool pool;
    FeaturePool::iterator pos;
    friend class MEMapFeature;
};

//This class is the text layer
class METextLayer : public MELayer
{
public:
    METextLayer(int width, int height, MEMap* parent_map);
    virtual ~METextLayer();

    char * GetName(void) const;
    void Search(MESearcher* searcher);

    void Draw(MEDC* dc);
    void Reset(void);
    void ResultReset(void);
    bool AddText(METextFeature *text);
    bool AddResultText(METextFeature *text);
    void SetTextWinrect(int width, int height);
private:
    METextLayer(const METextLayer & o);
    METextLayer & operator=(const METextLayer & o);

    int rect_in_region(METextFeature* text);
    int rect_in_result_region(METextFeature* text);

    std::list<METextFeature*> textlist;
    std::list<METextFeature*> result_textlist;

    CLIPRGN cliprgn;
    CLIPRGN display_cliprgn;
    BLOCKHEAP cliprc_heap;
    BLOCKHEAP display_cliprc_heap;
    int winrect_width;
    int winrect_height;
};

// this class is the result layer
class MEResultLayer : public MELayer
{
public:
    MEResultLayer(MEMap* parent_map);
    virtual ~MEResultLayer();

    char * GetName(void) const;
    void Search(MESearcher* searcher);

    int AddFeature(MEResultFeature* feature);
    void RemoveFeature(int frid);
    void ClearFeature(void);
    void Draw(MEDC* dc);
private:
    int last_frid;
    MEResultLayer(const MEResultLayer & o);
    MEResultLayer & operator=(const MEResultLayer & o);
    std::map<int, MEResultFeature*> feature_map;
};

#endif /* _ME_LAYER_H_ */
