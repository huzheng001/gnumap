/*
** $Id: dc.h,v 1.45 2006-10-23 08:13:12 jpzhang Exp $
**
** DC classes define
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Liu Peng.
**
** Create date: 2006/08/12
**
*/


#ifndef _DC_H_
#define _DC_H_


#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

/**
 *  MEPoint class...
 */

struct MEPoint
{
    int x;
    int y;
};

struct MEDoublePoint
{
  double x;
  double y;  
};

/**
 *  MERect class...
 */
struct MERect
{
    int left;
    int top;
    int right;
    int bottom;
};


/**
  * MEColor class
  */
class MEColor
{
public:
    /**
      *This constructor initialize the class with no param. And the member's value is zero.
      */
    MEColor()
    {
        color.rgba_struct.red   = 0;
        color.rgba_struct.green = 0;
        color.rgba_struct.blue  = 0;
        color.rgba_struct.alpha = 0;
    }
    /**
      *\param color_long Initialize the class with unsigned long type value.
      */
    MEColor(unsigned long color_long)
    {
        color.rgba_long = color_long & 0x00FFFFFF;
    }
    /**
      *\param red The red value of the color.
      *\param green The green value of the color.
      *\param blue The blue value of the color.
      *\param alpha The alpha value of the color.
      */
    MEColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 0xFF)
    {
        color.rgba_struct.red = red;
        color.rgba_struct.green = green;
        color.rgba_struct.blue = blue;
        color.rgba_struct.alpha = 0xFF - alpha;
    }
    /**
      *\param color_string Initialize the class with string type value.
      */
    MEColor(const char * color_string)
    {
        //...
        int red, green, blue, alpha, count;
        count = sscanf(color_string,"#%2x%2x%2x%2x",&red,&green,&blue,&alpha);
        if(count <= 3)
            alpha = 0xFF;
        color.rgba_struct.red = (unsigned char)red;
        color.rgba_struct.green = (unsigned char )green;
        color.rgba_struct.blue = (unsigned char)blue;
        color.rgba_struct.alpha = 0xFF - (unsigned char)alpha;
    }

    ~MEColor()
    {
    }


    unsigned long get_color_rgb() const { return color.rgba_long; }
    unsigned char get_red() const { return color.rgba_struct.red; }
    unsigned char get_green() const { return color.rgba_struct.green; }
    unsigned char get_blue() const { return color.rgba_struct.blue; }
    unsigned char get_alpha() const { return 0xFF - color.rgba_struct.alpha; }

    
    bool operator==(const MEColor& color) const
    {
        return (color.get_red() == get_red()
                && color.get_green() == get_green()
                && color.get_blue() == get_blue());
    }

private:
    union ColorUnion
    {

        unsigned long rgba_long;
        struct ColorStruct
        {
            unsigned char blue;
            unsigned char green;
            unsigned char red;
            unsigned char alpha;
        } rgba_struct;

    } color;
};


class MEDC;

enum FontStyle
{
    Font_Bold = 1,
    Font_Italic = 2,
    Font_Underline = 4
};
/**
  * MEFont class 
  */
class MEFont
{
public:
    MEFont(const char* name, int size, BYTE flags, const char * charset=NULL, int angle = 0);
    MEFont(const LOGFONT * font);
    MEFont();
    ~MEFont();

    char * GetFontName(void)const;
    void   SetFontName(const char * font_name);
    char * GetCharSet(void)const;
    void   SetCharSet(const char * charset);
    int    GetRotatAngle (void)const;
    void   SetRotatAngle(int angle);

    inline BYTE   GetFontStyle(void);
    inline void   SetFontStyle(BYTE styles);
    inline int    GetFontSize(void)const;
    inline void   SetFontSize(int size);

    void SetDCFont (MEDC *dc);
private:
    MEFont(const MEFont & o);
    MEFont & operator=(const MEFont & o);

    bool attr_change;
    char * m_name;
    int    m_size;
    unsigned char m_style;
    char * m_charset;
    int    m_angle;

    LOGFONT * logfont;
};

int MEFont::GetFontSize(void)const
{
    return m_size;
}

void MEFont::SetFontSize(int size)
{
    m_size = size;
    attr_change = true;
}

BYTE MEFont::GetFontStyle(void)
{
    return m_style;
}

void MEFont::SetFontStyle(BYTE styles)
{
    m_style = styles;
    attr_change = true;
}



enum PenCapStyle
{
    Pen_Cap_Butt = 0,
    Pen_Cap_Round,
    Pen_Cap_Projecting
};
enum PenJoinStyle
{
    Pen_Join_Miter = 0,
    Pen_Join_Round,
    Pen_Join_Bevel
};

enum PenStyle{
     Pen_Solid = 0,
     Pen_Null,
     Pen_Dash,
     Pen_ShortDash,
     Pen_LongDash,
     Pen_DotLine,
     Pen_DashDotLine,
     Pen_DashDotDotLine,
     Pen_AlternateLine
};

enum BrushStyle {
     Brush_Solid = 0,
     Brush_Null,
     Brush_HorizontalHatch,
     Brush_VerticalHatch,
     Brush_FDiagonalHatch,
     Brush_BDiagonalHatch,
     Brush_CrossHatch,
     Brush_DiagcrossHatch
};

enum SymbolStyle{
     Symbol_Point = -1,
     Symbol_Cross, //cross is 0
     Symbol_Diagcross,
     Symbol_Circle,
     Symbol_CircleFilled,
     Symbol_Square,
     Symbol_SquareFilled,
     Symbol_Triangle,
     Symbol_TriangleFilled,
     Symbol_Star,
     Symbol_StarFilled,
     Symbol_VerticalBar
};

enum SymbolSize{
     Symbol_8 = 0,
     Symbol_16,
};

/**
 * MEDC class
 */

class MEDC
{
public:
   
    MEDC(HDC hdcIn, bool minidc = false);
    ~MEDC();


    /* Pen attr */
    void   setPenColor( MEColor & color );
    void   setPenWidth( int width );
    void   setPenPattern( PenStyle patternId, int offset = 0);
    void   setPenCap( PenCapStyle cap );
    void   setPenJoin( PenJoinStyle join );

    MEColor      getPenColor(void);
    int          getPenWidth(void);
    PenStyle     getPenPattern(void);
    PenCapStyle  getPenCap(void);
    PenJoinStyle getPenJoin(void);

    /* Brush attr */
    void   setBrushType(BrushStyle brushId);
    void   setBrushFgColor(MEColor & color);
    void   setBrushBgColor(MEColor & color); 

    BrushStyle   getBrushType(void);
    MEColor      getBrushFgColor(void);
    MEColor      getBrushBgColor(void); 

    /* Symbol attr*/
    void         setSymbolPattern(SymbolStyle symbolId);
    SymbolStyle  getSymbolPattern(void);

    void         setSymbolSize(SymbolSize size);
    SymbolSize   getSymbolSize(void);

    /* Font */
    void   setTextColor( const MEColor & color );
    void   setBgColor( const MEColor & color );

    MEColor getTextColor(void);
    MEColor getBgColor(void);

    const MEFont* getFont(void) const;
    void    setFont(const MEFont *font);


    /* draw */
    void   drawPoint( int x, int y);
    void   drawLine( int x1, int y1, int x2, int y2 );
    void   drawRect(int x, int y, int width, int height); 
    void   fillRect(int x, int y, int width, int height);
    void   drawPolygon( const MEPoint * pPts, int nPts );
    void   fillPolygon( const MEPoint * pPts, int nPts );
    void   drawCircle(int x, int y, int r);
    void   drawCircleArc(int x, int y, int r, float fAngleBegin, float fAngleEnd);
    void   fillCircle(int x, int y, int r);
    void   drawEllipse(int sx, int sy, int rx, int ry);
    void   fillEllipse(int sx, int sy, int rx, int ry);
    void   drawText( const char * pszTextIn, int x, int y);
    void   drawBitmap( PBITMAP pBitmap, int x, int y, int w, int h );
    
    HDC    getDC() const;
    void   setDC(HDC hdcIn);

    void   setMiniDC(bool mini){m_minidc = mini;}
    bool   getMiniDC(void){return m_minidc;}
private:
    HDC hdc;

    MEFont default_font;
    MEFont * cur_font;
    bool m_minidc;
    unsigned char stipple_bits[8];
    STIPPLE my_stipple; 
    PenStyle m_pen_pattern;
    BrushStyle m_brush_pattern;
    SymbolStyle m_symbol_pattern;
    SymbolSize m_symbol_size;         /*if true,8*8; if false, 16*16*/
};

#endif /* _DC_H_ */

