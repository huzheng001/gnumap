/*
** $Id: symbol.cpp,v 1.8 2006-09-30 09:09:48 jpzhang Exp $
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

static MEColor old_brush_color;
static MEColor old_pen_color;
static BrushStyle old_brush_id;
static SymbolStyle old_symbol_id;
static SymbolSize old_symbol_size;

MEStyleSymbol::MEStyleSymbol(OGRStyleSymbol *poStyleSymbol, MEDC *dc)
    :color(NULL),angle(0.0),size(0.0),space_x(0.0),
    space_y(0.0),step(0),perp(0),offset(0),id(SYMBOL_CROSS)
{   
    GBool is_default;

    color = new MEColor((unsigned long)0x000000);

    int p_val = GetToolId(poStyleSymbol->Id(is_default));
    if (!is_default)
        id = (MESymbolId)p_val;

    const char * str_val = poStyleSymbol->Color (is_default);
    if (!is_default) {
        delete color;
        color = new MEColor(str_val);
    }
    
    poStyleSymbol->SetUnit(OGRSTUPixel);

    double pd_val = poStyleSymbol->Angle (is_default);
    if (!is_default)
        angle = pd_val;

    pd_val = poStyleSymbol->Size (is_default);
    if (!is_default)
        size = pd_val;

    pd_val = poStyleSymbol->SpacingX(is_default);
    if (!is_default)
        space_x = pd_val;

    pd_val = poStyleSymbol->SpacingY(is_default);
    if (!is_default)
        space_y = pd_val;

    pd_val = poStyleSymbol->Step(is_default);
    if (!is_default)
        step = pd_val;

    pd_val = poStyleSymbol->Perp(is_default);
    if (!is_default)
        perp = pd_val;

    pd_val = poStyleSymbol->Offset(is_default);
    if (!is_default)
        offset = pd_val;

    p_val = (int)poStyleSymbol->Priority (is_default); 
    if (!is_default)
        m_nPriority = p_val;

    //set up default symbol style 
    old_brush_id = dc->getBrushType();
    old_brush_color = dc->getBrushFgColor();
    old_pen_color = dc->getPenColor();
    old_symbol_id = dc->getSymbolPattern();
    old_symbol_size = dc->getSymbolSize();

    m_tool_type = METOOL_SYMBOL;
}

MEStyleSymbol::~MEStyleSymbol()
{
    delete color;
}

void MEStyleSymbol::SetDC(MEDC *pmDC, MEMapFeature * feature)
{
    pmDC->setBrushFgColor(*color);
    pmDC->setPenColor(*color);
    pmDC->setSymbolPattern((SymbolStyle)id);
    if (size > 8)
        pmDC->setSymbolSize(Symbol_16);
    else
        pmDC->setSymbolSize(Symbol_8);
}
void MEStyleSymbol::ResetDC(MEDC *pmDC, MEMapFeature * feature)
{
    pmDC->setBrushFgColor(old_brush_color);
    pmDC->setPenColor(old_pen_color);
    pmDC->setSymbolPattern(old_symbol_id);
    pmDC->setSymbolSize(old_symbol_size);
    pmDC->setBrushType(old_brush_id);
}
