/*
** $Id: label.cpp,v 1.13 2006-11-13 09:35:13 jpzhang Exp $
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
#include "feature.h"
#include "geometry.h"

#include "mstring.h"

static const MEFont * old_font;
static MEColor old_bc; 
static MEColor old_fc; 

static MEPlacement get_placement (OGRStyleLabel * label, GBool & is_default)
{
    const char * p_plac = label->Placement(is_default);
    if (strncasecmp(p_plac, "p", 1) == 0)
        return LABEL_MP;
    else if (strncasecmp(p_plac, "l", 1) == 0)
        return LABEL_ML;
    else if (strncasecmp(p_plac, "s", 1) == 0)
        return LABEL_MS;
    else if (strncasecmp(p_plac, "m", 1) == 0)
        return LABEL_MM;
    else if (strncasecmp(p_plac, "w", 1) == 0)
        return LABEL_MW;

    return LABEL_MP;
}

MEStyleLabel::MEStyleLabel(OGRStyleLabel *poStyleLabel, MEDC *dc)
    :font_name(NULL),size(0.0),text_string(NULL),
    angle(0.0),fc(NULL),bc(NULL),mode(LABEL_MP),anchor(0),
    s_x(0.0),s_y(0.0),perp(0.0),bold(false),italic(false),
    underline(false),m_font(NULL)
{   
    font_name = strdup("");
    text_string = strdup("");
    fc = new MEColor((unsigned long)0x000000);
    bc = new MEColor(0x00,0x00,0x00,0x00);

    GBool is_default;
    unsigned long color_rgb;

    sscanf(poStyleLabel->ForeColor(is_default),"#%6lx",&color_rgb);
    if (!is_default) {
        delete fc;
        fc = new MEColor(color_rgb);
    }

    sscanf(poStyleLabel->BackColor(is_default),"#%6lx",&color_rgb);
    if (!is_default) {
        delete bc;
        bc = new MEColor(color_rgb);
    }

    const char * str_val = poStyleLabel->FontName (is_default);
    if (!is_default) {
        free (font_name);
        font_name = strdup(str_val);
    }

    str_val = poStyleLabel->TextString (is_default);
    if (!is_default) {
        free (text_string);
        text_string = strdup(str_val);
    }

    double pd_val = poStyleLabel->Size(is_default);
    if (!is_default)
        size = pd_val;

    pd_val = poStyleLabel->Angle(is_default);
    if (!is_default)
        angle = pd_val;

    MEPlacement p_mod = get_placement(poStyleLabel, is_default);
    if (!is_default)
        mode = p_mod;

    int p_val = poStyleLabel->Anchor(is_default);
    if (!is_default)
       anchor = p_val;

    pd_val = poStyleLabel->SpacingX(is_default);
    if (!is_default)
        s_x = pd_val;

    pd_val = poStyleLabel->SpacingY(is_default);
    if (!is_default)
        s_y = pd_val;
    
    pd_val = poStyleLabel->Perp(is_default);
    if (!is_default)
        perp = pd_val;

    bool pb_val = (poStyleLabel->Bold(is_default)==TRUE)?true:false;
    if (!is_default)
        bold = pb_val;

    pb_val = (poStyleLabel->Italic(is_default)==TRUE)?true:false;
    if (!is_default)
        italic = pb_val;

    pb_val = (poStyleLabel->Underline(is_default)==TRUE)?true:false;
    if (!is_default)
        underline = pb_val;

    p_val = poStyleLabel->Priority (is_default); 
    if (!is_default)
        m_nPriority = p_val;

    //set up default label style 
    old_font = dc->getFont();
    old_bc = dc->getBgColor();
    old_fc = dc->getTextColor();

    m_tool_type = METOOL_LABEL;
}

MEStyleLabel::~MEStyleLabel()
{
    if (m_font)
        delete m_font;

    free (font_name);
    free (text_string);
    delete fc;
    delete bc;
}

void MEStyleLabel::SetDC(MEDC *pmDC, MEMapFeature * feature)
{
    int x,y;
    unsigned char flags = 0;

    if (bold)
        flags |= Font_Bold;

    if (italic)
        flags |= Font_Italic;

    if (underline)
        flags |= Font_Underline; 

    if (m_font){
        delete m_font;
        m_font = NULL;
    }

    m_font = new MEFont(font_name,(int)size, flags); 
    pmDC->setFont(m_font);
    pmDC->setBgColor(*bc);
    pmDC->setTextColor(*fc);
//draw lable text
    if (!pmDC->getMiniDC()) {
        feature->GetGeometry()->GetTextPosition(text_string, (int)size, &x, &y);
        x += (int)s_x;
        y += (int)s_y;
        pmDC->drawText(text_string, x, y);
    }
}
void MEStyleLabel::ResetDC(MEDC *pmDC, MEMapFeature * feature)
{
    pmDC->setFont(old_font);
    pmDC->setBgColor(old_bc);
    pmDC->setTextColor(old_fc);
}
