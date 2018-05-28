/*
** $Id: brush.cpp,v 1.10 2006-09-30 09:09:48 jpzhang Exp $
**
** Class  implementation.
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Zhang Ji Peng.
**
** Create date: 2006/08/11
 */

#include "style.h"

static MEColor old_brush_bg_color;
static MEColor old_brush_fg_color;
static BrushStyle old_brush_id;

MEStyleBrush::MEStyleBrush(OGRStyleBrush *poStyleBrush, MEDC * dc)
    :fg_color(NULL),bg_color(NULL),angle(0.0),size(1),
    space_x(0.0),space_y(0.0),id(BRUSH_SOLID)
{   
    GBool is_default;

    fg_color = new MEColor(0x00,0x00,0x00);
    bg_color = new MEColor(0xFF,0xFF,0xFF);
    
    int p_val = GetToolId(poStyleBrush->Id(is_default));
    if (!is_default)
        id = (MEBrushId)p_val;

    const char * str_val = poStyleBrush->ForeColor(is_default);
    if (!is_default) {
        delete fg_color;
        fg_color = new MEColor(str_val);
    }

    str_val = poStyleBrush->BackColor(is_default);
    if (!is_default) {
        delete bg_color;
        bg_color = new MEColor(str_val);
    }

    double pd_val = poStyleBrush->Angle(is_default);
    if (!is_default)
        angle = pd_val;

    pd_val = poStyleBrush->Size(is_default);
    if (!is_default)
        size = pd_val;

    pd_val = poStyleBrush->SpacingX (is_default);
    if (!is_default)
        space_x = pd_val;

    pd_val = poStyleBrush->SpacingY (is_default);
    if (!is_default)
        space_y = pd_val;

    p_val = poStyleBrush->Priority (is_default); 
    if (!is_default)
        m_nPriority = p_val;

    //set up default brush style 
    old_brush_bg_color = dc->getBrushBgColor();
    old_brush_fg_color = dc->getBrushFgColor();
    old_brush_id = dc->getBrushType();

    m_tool_type = METOOL_BRUSH;
}

MEStyleBrush::~MEStyleBrush()
{
    delete fg_color;
    delete bg_color;
}

void MEStyleBrush::SetDC(MEDC *pmDC, MEMapFeature * feature)
{
    pmDC->setBrushBgColor(*bg_color);
    pmDC->setBrushFgColor(*fg_color);
    pmDC->setBrushType((BrushStyle)id);
}
void MEStyleBrush::ResetDC(MEDC *pmDC, MEMapFeature * feature)
{
    pmDC->setBrushBgColor(old_brush_bg_color);
    pmDC->setBrushFgColor(old_brush_fg_color);
    pmDC->setBrushType(old_brush_id);
}
