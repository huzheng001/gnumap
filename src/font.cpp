/*
** $Id: font.cpp,v 1.13 2006-11-13 09:35:13 jpzhang Exp $
**
** Font classes define
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Zhang ji Peng.
**
** Create date: 2006/08/12
**
*/

#include <string.h>
#include <stdlib.h>

#include "dc.h"


MEFont::MEFont()
    :attr_change(false),m_name(NULL),m_size(10), m_style(0),m_charset(NULL),m_angle(0),logfont(NULL)
{
    m_charset = strdup(FONT_CHARSET_GB2312_0);
}

MEFont::MEFont(const LOGFONT * font)
    :attr_change(false),m_name(NULL),m_size(10), m_style(0),m_charset(NULL),m_angle(0),logfont(NULL)
{
    //set font attribute
    if (font) {
        m_name = strdup(font->family);
        m_charset = strdup(font->charset);
        m_size = font->size;
        m_angle = font->rotation;
        
        if (font->style & FONT_WEIGHT_BOLD)
            m_style |= Font_Bold;
        if (font->style & FONT_SLANT_ITALIC)
            m_style |= Font_Italic;
        if (font->style & FONT_UNDERLINE_LINE)
            m_style |= Font_Underline;

        logfont = CreateLogFontIndirect ((PLOGFONT)font);
    }
    else
        m_charset = strdup(FONT_CHARSET_GB2312_0);

}

MEFont::MEFont(const char* name, int size, BYTE flags, const char * charset, int angle)
    :attr_change(false),m_name(NULL),m_size(size), m_style(flags),m_charset(NULL),m_angle(angle),logfont(NULL)
{
    if (name)
        m_name = strdup(name);

    if (charset)
        m_charset = strdup(charset);
    else
        m_charset = strdup(FONT_CHARSET_GB2312_0);
}

MEFont::~MEFont()
{
    if (logfont){
        DestroyLogFont(logfont);
        logfont = NULL;
    }

    if (m_name)
        free (m_name);

    free (m_charset);
}

char * MEFont::GetFontName(void) const
{
    return m_name;
}

void MEFont::SetFontName(const char * font_name)
{
    if (m_name)
        free (m_name);

    m_name = strdup(font_name);
    attr_change = true;
}


char * MEFont::GetCharSet(void)const
{
    return m_charset;
}

void MEFont::SetCharSet(const char * charset)
{
    free (m_charset);
    m_charset = strdup(charset);
    attr_change = true;
}

int MEFont::GetRotatAngle (void)const
{
    return m_angle;
}

void MEFont::SetRotatAngle(int angle)
{
    m_angle = angle;
    attr_change = true;
}

void MEFont::SetDCFont(MEDC *dc)
{
    if (logfont && !attr_change) { 
        SelectFont(dc->getDC(), logfont);
    } else { //create the logfont

        if (logfont) {
            DestroyLogFont(logfont);
            logfont = NULL;
        }
        
        if (m_name) {
            unsigned char bold, italic, underline;
            if (m_style & Font_Bold)
                bold = FONT_WEIGHT_BOLD;
            else
                bold = FONT_WEIGHT_REGULAR;

            if (m_style & Font_Italic)
                italic = FONT_SLANT_ITALIC;
            else
                italic = FONT_SLANT_ROMAN;

            if (m_style & Font_Underline)
                underline = FONT_UNDERLINE_LINE;
            else
                underline = FONT_UNDERLINE_NONE;

            logfont = CreateLogFont(NULL, m_name, m_charset, 
                                bold, italic, FONT_SETWIDTH_NORMAL, FONT_SPACING_CHARCELL, 
                                underline, FONT_STRUCKOUT_NONE, m_size, m_angle);
            if (logfont)
                SelectFont(dc->getDC(), logfont);
        }
    }

    attr_change = false;
}

