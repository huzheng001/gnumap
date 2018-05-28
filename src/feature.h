/*
** $Id: feature.h,v 1.37 2006-10-31 01:53:22 jpzhang Exp $
**
** Classes define for Feature
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Zhang Ji Peng.
**
** Create date: 2006/08/09
*/

#ifndef _FEATURE_H_
#define _FEATURE_H_

#include <map>
#include <vector>

#include "ogr.h"
#include "utils.h"
#include "dc.h"
#include "mapfield.h"

class MEDC;
class MEColor;
class MELayer;
class MEStyle;
class MEGeometry;
class METextFeature;

struct MERect;

//abstract class of features
class MEFeature
{
public:
    MEFeature(MELayer * parent);
    virtual ~MEFeature();

    virtual void Draw(MEDC * dc) = 0;
    inline MELayer* GetParentLayer(void) const;
protected:
    MELayer * parent_layer;
};

MELayer* MEFeature::GetParentLayer(void) const
{
    return parent_layer;
}

//map feature class
class MEMapFeature : public MEFeature
{
public:
    MEMapFeature(MELayer * parent_layer, OGRFeature * ogrfeature);
    MEMapFeature(MELayer * parent_layer, char **p, std::vector<const char *> &Styles);
    virtual ~MEMapFeature();

    void Draw(MEDC * dc);

    MEGeometry* GetGeometry(void) const{return geometry;}
    long GetFid(void) const{return fid;}
    char* GetName(void) const{return m_name;}
    MEStyle *GetStyle();
    int draw_age;
private:    
    MEMapFeature(const MEMapFeature & o);
    MEMapFeature & operator = (const MEMapFeature & o);
    
    void set_text(METextFeature *text);

    MEStyle * style;
    MEGeometry* geometry;
    long fid;
    char * m_name;
    
    std::map<const char *, MEMapField *> m_fields;
};

//class textlayer feature 
class METextFeature : public MEFeature
{
public:
    METextFeature(MELayer * parent_layer, const char * text = NULL,
                             const MEColor *fc = NULL, const MEColor *bc = NULL);
    virtual ~METextFeature();

    void Draw(MEDC* dc);

    bool PointInRect(int width, int height);
    void SetText(const char * text);
    void SetTextPos(int x, int y);
    char* GetText(void) const;
    void SetFont(const MEFont * ft){if (ft) m_setfont = ft;}
    const MEFont * GetFont(void){return m_setfont;}
    MERect* GetTextRect(void){return rect;}

    MEColor *GetColor(void)const;
    void SetColor(const MEColor * color);

    MEColor *GetBgColor(void)const;
    void SetBgColor(const MEColor * color);
private:
    METextFeature(const METextFeature & o);
    METextFeature & operator = (const METextFeature & o);

    int m_x;
    int m_y;
    MEColor * m_fc;
    MEColor * m_bc;
    const char * m_text;
    const MEFont * m_setfont;
    MERect* rect;
};

#endif /* _FEATURE_H_ */
