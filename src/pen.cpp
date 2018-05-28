/*
** $Id: pen.cpp,v 1.18 2006-11-17 02:40:22 jpzhang Exp $
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
#include <string>

#include "style.h"
#include "mstring.h"

using std::string;


static int old_pen_width;
static MEColor old_pen_color;
static PenCapStyle old_pen_cap;
static PenJoinStyle old_pen_join;
static PenStyle old_pen_id;

static char * get_pattern (OGRStylePen * pen, GBool & is_default)
{
    string::size_type pos = 0;
    int count = 0;
    string m_pattern(pen->Pattern(is_default));
    while ((pos = m_pattern.find(" ",pos)) != string::npos)
    {
        count++;
        pos++;
    }
    unsigned char* pattern = (unsigned char *)malloc(count+1);
    int i = 0;
    while((pos = m_pattern.find_first_of(" ")) != string::npos)
    {
        pattern[i] = atoi((m_pattern.substr(0,pos).c_str()));
        m_pattern = m_pattern.substr(pos+1);
        i++;
    } 
    pattern[i] = 0;
    return (char*)pattern;
}
static MEPenCapStyle get_cap (OGRStylePen * pen, GBool & is_default)
{
    const char * p_cap = pen->Cap(is_default);
    if (strncasecmp(p_cap, "b", 1) == 0)
        return PEN_CAP_BUTT;
    else if (strncasecmp(p_cap, "r", 1) == 0)
        return PEN_CAP_ROUND;
    else if (strncasecmp(p_cap, "p", 1) == 0)
        return PEN_CAP_PROJECTING;
    return PEN_CAP_BUTT;
}

static MEPenJoinStyle get_join (OGRStylePen * pen, GBool & is_default)
{
    const char * p_join = pen->Join(is_default);
    if (strncasecmp(p_join, "m", 1) == 0)
        return PEN_JOIN_MITER;
    else if (strncasecmp(p_join, "r", 1) == 0)
        return PEN_JOIN_ROUND;
    else if (strncasecmp(p_join, "b", 1) == 0)
        return PEN_JOIN_BEVEL;
    return PEN_JOIN_MITER;
}

MEStylePen::MEStylePen(OGRStylePen *poStylePen, MEDC * dc)
    :width(1),color(NULL),pattern(NULL),pre_offset(0),
    cap(PEN_CAP_BUTT),join(PEN_JOIN_MITER),id(PEN_SOLID)
{   
    GBool is_default;

    color = new MEColor((unsigned long)0x000000);
    pattern = strdup("");

    int p_val = GetToolId(poStylePen->Id(is_default)); 
    if (!is_default)
        id = (MEPenId)p_val; // set pen id

    p_val = (int)poStylePen->Width(is_default);
    if (!is_default)
        width = p_val; // set pen width

    const char * str_val = poStylePen->Color(is_default);
    if (!is_default){
        delete color;
        color = new MEColor(str_val);//set color 
    }

    str_val = get_pattern(poStylePen, is_default);
    if (!is_default){
        free (pattern);
        pattern = (char *)str_val; //set pattern 
    }else
        free((char *)str_val);

    p_val = (int)poStylePen->PerpendicularOffset(is_default);
    if (!is_default)
        pre_offset = p_val; // set perp offset

    MEPenCapStyle s_cap = get_cap(poStylePen, is_default);
    if (!is_default)
       cap = s_cap;  // set pen cap 

    MEPenJoinStyle s_join = get_join (poStylePen, is_default);
    if (!is_default)
        join = s_join; // set pen join

    p_val = (int)poStylePen->Priority (is_default); 
    if (!is_default)
        m_nPriority = p_val;

    //set up default pen style 
    old_pen_width = dc->getPenWidth();
    old_pen_color = dc->getPenColor();
    old_pen_cap = dc->getPenCap();
    old_pen_join = dc->getPenJoin();
    old_pen_id = dc->getPenPattern();

    m_tool_type = METOOL_PEN;
}

MEStylePen::~MEStylePen()
{
    delete color;
    free (pattern);
}

void MEStylePen::SetDC(MEDC *pmDC, MEMapFeature * feature)
{
    pmDC->setPenWidth(width);
    pmDC->setPenColor(*color);
    pmDC->setPenCap((PenCapStyle)cap);
    pmDC->setPenJoin((PenJoinStyle)join);
    pmDC->setPenPattern((PenStyle)id);
}
void MEStylePen::ResetDC(MEDC *pmDC, MEMapFeature * feature)
{
    pmDC->setPenWidth(old_pen_width);
    pmDC->setPenColor(old_pen_color);
    pmDC->setPenCap(old_pen_cap);
    pmDC->setPenJoin(old_pen_join);
    pmDC->setPenPattern(old_pen_id);
}
