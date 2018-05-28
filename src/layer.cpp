/*
** $Id: layer.cpp,v 1.8 2006-10-23 08:13:12 jpzhang Exp $
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


#include "layer.h"

MELayer::MELayer(MEMap* parent_map)
    : map(parent_map),layer_font(NULL)
{
    layer_font = new MEFont("fixed", 12, 0, FONT_CHARSET_GB2312_0, 0);
}

MELayer::~MELayer()
{
    delete layer_font;
}

void MELayer::SetLayerFont (MEFont * new_font)
{
    if (!new_font)
        return;
    
    delete layer_font;
    layer_font = new_font;
}
