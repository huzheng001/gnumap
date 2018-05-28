/*
** $Id: mapfeature.cpp,v 1.44 2007-07-03 10:42:47 jpzhang Exp $
**
** map feature class implement
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Zhang Ji Peng.
**
** Create date: 2006/08/09
*/

#include "feature.h"
#include "geometry.h"
#include "style.h"
#include "layer.h"
#include "env.h"

static const char defaultstyle[4][128]=
{
    "BRUSH(fc:#ffd0a0,bc:#000000,id:'mapinfo-brush-2.ogr-brush-0')",
    "BRUSH(fc:#00d000,bc:#000000,id:'mapinfo-brush-1.ogr-brush-0')",
    "BRUSH(fc:#f0e040,bc:#000000,id:'mapinfo-brush-13.ogr-brush-0')",
    "BRUSH(fc:#ff9470,bc:#000000,id:'mapinfo-brush-2.ogr-brush-0')"
};

static int stylenum=0;

MEMapFeature::MEMapFeature(MELayer * parent_layer, OGRFeature * ogrfeature)
             :MEFeature(parent_layer),draw_age(-1),m_name(NULL) 
{
    fid = ogrfeature->GetFID();
//store all field of a feature
/*
    for (int i=0; i < ogrfeature->GetFieldCount(); i++) {
        MEMapField * mf = MEMapField::GetMapField(ogrfeature, i);
        m_fields.insert (std::make_pair(mf->GetName(), mf));
    }
*/
    int index;
    if ((index = ogrfeature->GetFieldIndex("Ãû³Æ"))!= -1)
        m_name = strdup(ogrfeature->GetFieldAsString(index));
    else if ((index = ogrfeature->GetFieldIndex("NAME")) != -1)
        m_name = strdup(ogrfeature->GetFieldAsString(index));

    geometry = MEGeometry::GetGeometry(ogrfeature->GetGeometryRef(), this);

    if (ogrfeature->GetStyleString())
        style = MEStyle::CreateStyle(ogrfeature->GetStyleString(),parent_layer->GetMap()->dc); 
                                                            //create from the pool shouldn't delete 
    else{
        style = MEStyle::CreateStyle(defaultstyle[stylenum],parent_layer->GetMap()->dc); 
        stylenum++;
        if (stylenum == 4) stylenum = 0;
    } 
}

MEMapFeature::MEMapFeature(MELayer * parent_layer, char **data, std::vector<const char *> &Styles)
             :MEFeature(parent_layer),draw_age(-1)
{
    char *p = *data;
    fid = ArchSwapLE32(read_uint(p));
    p+=4;
    m_name = strdup(p);
    p = p + strlen(p) +1;
    unsigned int StyleId;
    StyleId = ArchSwapLE32(read_uint(p));
    p+=4;
    style = MEStyle::CreateStyle(Styles[StyleId],parent_layer->GetMap()->dc);
    geometry = MEGeometry::GetGeometry(&p, this);
    *data = p;
}

MEMapFeature::~MEMapFeature()
{
    std::map<const char *, MEMapField *>::iterator pt;
    for (pt = m_fields.begin(); pt != m_fields.end(); ++pt)  
        delete pt->second;

    delete geometry;

    free (m_name);
}

MEStyle * MEMapFeature::GetStyle()
{
    return style;
}

void MEMapFeature::set_text(METextFeature *text)
{
    int x=0, y=0;
    text->SetText(m_name);

    if (geometry)
        geometry ->GetTextPosition (m_name, 12, &x, &y);

    text->SetTextPos(x, y);
}

void MEMapFeature::Draw(MEDC * dc)
{
    
    MELayer * textlayer = parent_layer->GetMap()->textlayer;

    if (textlayer) {
        METextFeature *text = new METextFeature(textlayer);
        set_text(text);

        if (parent_layer->GetLayerFont())
            text->SetFont(parent_layer->GetLayerFont());

        if (!((METextLayer*)textlayer)->AddText(text))
            delete text;
    }
    
    if (geometry)
        geometry->Draw(dc, style);
}

