/*
** $Id: dc.cpp,v 1.55 2006-10-23 08:13:12 jpzhang Exp $
**
** DC classes implement
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

#include <string.h>

#include "dc.h"
#include "symbolstipplebits.h"

MEDC::MEDC(HDC hdcIn, bool minidc)
    :hdc(hdcIn),m_minidc(minidc),
     m_pen_pattern(Pen_Solid),
     m_brush_pattern(Brush_Solid),
     m_symbol_pattern(Symbol_Point),
     m_symbol_size(Symbol_8)
{
    memset (stipple_bits, 0, 8);
    my_stipple.width = 8;
    my_stipple.height= 8;
    my_stipple.pitch = 1;
    my_stipple.size = 8;
    my_stipple.bits = stipple_bits;

    //set default font
    PLOGFONT pfont = GetCurFont(hdc); 

    default_font.SetFontName (pfont->family);
    default_font.SetFontSize (pfont->size);
    default_font.SetCharSet (pfont->charset);
    default_font.SetRotatAngle (pfont->rotation);
    
    BYTE style = 0;
    if (pfont->style & FONT_WEIGHT_BOLD)
        style |= Font_Bold;

    if (pfont->style & FONT_SLANT_ITALIC)
        style |= Font_Italic;

    if (pfont->style & FONT_UNDERLINE_LINE)
        style |= Font_Underline;

    default_font.SetFontStyle(style);

    cur_font = &default_font;
}

MEDC::~MEDC()
{
}


HDC MEDC::getDC() const
{
    return hdc;
}

void MEDC::setDC(HDC hdcIn)
{
    hdc = hdcIn;
}

/* Pen attr */


/* need modify... */
void MEDC::setPenColor( MEColor & color )
{
    gal_pixel pixel = RGBA2Pixel( hdc,
                        color.get_red(), 
                        color.get_green(), 
                        color.get_blue(), 
                        color.get_alpha() );

    SetPenColor( hdc, pixel );
}

void MEDC::setPenWidth( int width )
{
    SetPenWidth( hdc, width );
}

void MEDC::setPenPattern( PenStyle patternId, int offset )
{
    const char* pattern = NULL;

    m_pen_pattern = patternId;
    
    switch(patternId)
    {
    case Pen_Dash:
	    pattern = "\7\5";
	    break;
    case Pen_ShortDash:
	    pattern = "\4\5";
	    break;
    case Pen_LongDash:
	    pattern = "\10\5";
	    break;
    case Pen_DotLine:
	    pattern = "\1\1";
	    break;
    case Pen_DashDotLine:
	    pattern = "\7\2\2\2";
	    break;
    case Pen_DashDotDotLine:
	    pattern = "\7\2\2\2\2\2";
	    break;
    case Pen_AlternateLine:
        pattern = "\2\2";
        break;
    case Pen_Null:        //.. to be finished.
    case Pen_Solid:
    default:
        /* make complier happy*/
       break; 
    }

    if (pattern)
    {
        SetPenType( hdc, PT_ON_OFF_DASH);
        SetPenDashes( hdc, offset, (unsigned char *)pattern, strlen(pattern));
    }
    else
        SetPenType( hdc, PT_SOLID );
}

static const int minigui_cap_table[] = {PT_CAP_BUTT, PT_CAP_ROUND, PT_CAP_PROJECTING};
void MEDC::setPenCap( PenCapStyle cap )
{
    SetPenCapStyle( hdc, minigui_cap_table[cap]);
}

static const int minigui_join_table[] = {PT_JOIN_MITER, PT_JOIN_ROUND, PT_JOIN_BEVEL};
void MEDC::setPenJoin( PenJoinStyle join )
{
    SetPenJoinStyle( hdc, minigui_join_table[join]);
}


MEColor MEDC::getPenColor(void)
{
    BYTE r,g,b,a;
    Pixel2RGBA(hdc ,GetPenColor(hdc), &r, &g, &b, &a);
    return MEColor (r,g,b,a);
}

int MEDC::getPenWidth(void)
{
    return GetPenWidth(hdc);
}

PenStyle MEDC::getPenPattern(void)
{
    return m_pen_pattern;
}

PenCapStyle MEDC::getPenCap(void)
{
    PenCapStyle cap = Pen_Cap_Butt;
    switch (GetPenCapStyle(hdc))
    {
        case PT_CAP_BUTT:
           cap = Pen_Cap_Butt;
           break;
        case PT_CAP_ROUND:
           cap = Pen_Cap_Round;
           break;
        case PT_CAP_PROJECTING:
           cap = Pen_Cap_Projecting; 
           break;
    }
    return cap;
}

PenJoinStyle MEDC::getPenJoin(void)
{
    PenJoinStyle join = Pen_Join_Miter;
    switch (GetPenJoinStyle(hdc))
    {
        case PT_JOIN_MITER:
           join = Pen_Join_Miter;
           break;
        case PT_JOIN_ROUND:
           join = Pen_Join_Round;
           break;
        case PT_JOIN_BEVEL:
           join = Pen_Join_Bevel; 
           break;
    }
    return join;
}

/* Brush attr */
void MEDC::setBrushType(BrushStyle brushId)
{
    m_brush_pattern = brushId;

   if (Brush_Solid == brushId)
   {
        SetBrushType(hdc,BT_SOLID);
        return;
   }
    
  // "\x00\x00\x00\x00\x00\x00\x00\x00";   Brush_Null:
  // "\x81\x42\x24\x18\x18\x24\x42\x81";   Brush_DiagcrossHatch
  // "\x80\x40\x20\x10\x08\x04\x02\x01";   Brush_BDiagonalHatch
  // "\x01\x02\x04\x08\x10\x20\x40\x80";   Brush_FDiagonalHatch
  // "\x10\x10\x10\xFF\x10\x10\x10\x10";   Brush_CrossHatch
  // "\x10\x10\x10\x10\x10\x10\x10\x10";   Brush_VerticalHatch
  // "\x00\x00\x00\xF0\x00\x00\x00\x00";   Brush_HorizontalHatch
           
    switch(brushId)
    {
    case Brush_Null:
      stipple_bits[0] = 0x00;
      stipple_bits[1] = 0x00;
      stipple_bits[2] = 0x00;
      stipple_bits[3] = 0x00;
      stipple_bits[4] = 0x00;
      stipple_bits[5] = 0x00;
      stipple_bits[6] = 0x00;
      stipple_bits[7] = 0x00;
      break;

    case Brush_HorizontalHatch:
      stipple_bits[0] = 0x00;
      stipple_bits[1] = 0x00;
      stipple_bits[2] = 0x00;
      stipple_bits[3] = 0xFF;
      stipple_bits[4] = 0x00;
      stipple_bits[5] = 0x00;
      stipple_bits[6] = 0x00;
      stipple_bits[7] = 0x00;
      break;

    case Brush_VerticalHatch:
      stipple_bits[0] = 0x10;
      stipple_bits[1] = 0x10;
      stipple_bits[2] = 0x10;
      stipple_bits[3] = 0x10;
      stipple_bits[4] = 0x10;
      stipple_bits[5] = 0x10;
      stipple_bits[6] = 0x10;
      stipple_bits[7] = 0x10;
      break;
    case Brush_FDiagonalHatch:      
      stipple_bits[0] = 0x01;
      stipple_bits[1] = 0x02;
      stipple_bits[2] = 0x04;
      stipple_bits[3] = 0x08;
      stipple_bits[4] = 0x10;
      stipple_bits[5] = 0x20;
      stipple_bits[6] = 0x40;
      stipple_bits[7] = 0x80;
      break;              

    case Brush_BDiagonalHatch:
      stipple_bits[0] = 0x80;
      stipple_bits[1] = 0x40;
      stipple_bits[2] = 0x20;
      stipple_bits[3] = 0x10;
      stipple_bits[4] = 0x08;
      stipple_bits[5] = 0x04;
      stipple_bits[6] = 0x02;
      stipple_bits[7] = 0x01;
      break;

    case Brush_CrossHatch:   
      stipple_bits[0] = 0x10;
      stipple_bits[1] = 0x10;
      stipple_bits[2] = 0x10;
      stipple_bits[3] = 0xFF;
      stipple_bits[4] = 0x10;
      stipple_bits[5] = 0x10;
      stipple_bits[6] = 0x10;
      stipple_bits[7] = 0x10;
      break;        

    case Brush_DiagcrossHatch:
      stipple_bits[0] = 0x81;
      stipple_bits[1] = 0x42;
      stipple_bits[2] = 0x24;
      stipple_bits[3] = 0x18;
      stipple_bits[4] = 0x18;
      stipple_bits[5] = 0x24;
      stipple_bits[6] = 0x42;
      stipple_bits[7] = 0x81;
      break;
   
    case Brush_Solid:   
    default:
        break;
    }    
    
    SetBrushInfo(hdc, NULL, &my_stipple);
    if (Brush_Null == brushId)
        SetBrushType(hdc,BT_STIPPLED);
    else
        SetBrushType(hdc,BT_OPAQUE_STIPPLED);
    return;
}


void MEDC::setBrushFgColor(MEColor & color)
{
    SetBrushColor(hdc, RGBA2Pixel(hdc, color.get_red(), color.get_green(), color.get_blue(), color.get_alpha()));
}

void MEDC::setBrushBgColor(MEColor & color)
{
    SetBkColor(hdc, RGBA2Pixel(hdc, color.get_red(), color.get_green(), color.get_blue(), color.get_alpha()));
}

BrushStyle MEDC::getBrushType(void)
{
    return m_brush_pattern;
}

MEColor MEDC::getBrushFgColor(void)
{
    BYTE r,g,b,a;
    Pixel2RGBA(hdc ,GetBrushColor(hdc), &r, &g, &b, &a);
    return MEColor (r,g,b,a);
}

MEColor MEDC::getBrushBgColor(void)
{
    BYTE r,g,b,a;
    Pixel2RGBA(hdc ,GetBkColor(hdc), &r, &g, &b, &a);
    return MEColor (r,g,b,a);
}

//Symbol attr
void MEDC::setSymbolPattern(SymbolStyle symbolId)
{
    m_symbol_pattern = symbolId;
}

SymbolStyle MEDC::getSymbolPattern(void)
{
    return m_symbol_pattern;
}

void MEDC::setSymbolSize(SymbolSize size)
{
    m_symbol_size = size;
}

SymbolSize MEDC::getSymbolSize(void)
{
    return m_symbol_size;
}

/* Font arrt */
void MEDC::setTextColor(const MEColor & color )
{
    SetTextColor(hdc, RGBA2Pixel(hdc, color.get_red(), color.get_green(), color.get_blue(), color.get_alpha()));
}

void MEDC::setBgColor(const MEColor & color )
{
    if(color.get_alpha() == 0)
        SetBkMode(hdc, BM_TRANSPARENT);
    else
    {
        SetBkMode(hdc, BM_OPAQUE);
        SetBkColor(hdc, RGBA2Pixel(hdc, color.get_red(), color.get_green(), color.get_blue(), color.get_alpha()) );
    }
}

MEColor MEDC::getTextColor(void)
{
    BYTE r,g,b,a;
    Pixel2RGBA(hdc ,GetTextColor(hdc), &r, &g, &b, &a);
    return MEColor (r,g,b,a);
}

MEColor  MEDC::getBgColor(void)
{
    BYTE r,g,b,a;
    Pixel2RGBA(hdc ,GetBkColor(hdc), &r, &g, &b, &a);
    
    if (GetBkMode(hdc) == BM_TRANSPARENT)
        a = 0;

    return MEColor (r,g,b,a);
}

const MEFont* MEDC::getFont(void) const
{
    return cur_font;
}

void MEDC::setFont(const MEFont * font)
{
    if (!font)
        return;

    cur_font = (MEFont*)font;
    
    cur_font->SetDCFont(this);
}

/*draw*/
void MEDC::drawPoint( int x, int y )
{
    if (m_minidc) {
        SetPixel(hdc, x, y, GetPenColor(hdc));  
        return;
    }

    int offset = 8;

    if(Symbol_16 == m_symbol_size)
        offset = 16;
  
    POINT pns[5] = {{x,y},{x+offset,y},{x+offset,y+offset},{x,y+offset},{x,y}};  
    POINT pnsTri[4] = {{x,y},{x+(offset>>1),y+offset-3},{x-(offset>>1),y+offset-3},{x,y}};    
    
    switch(m_symbol_pattern)
    {
    case Symbol_Point:
        SetBrushType(hdc, BT_SOLID);
        SetPixel(hdc, x, y, GetPenColor(hdc));  
	break;
    case Symbol_Cross:    

        if(Symbol_8 == m_symbol_size)    /*-- 8*8 Cross --*/
        {
            SetBrushInfo(hdc, NULL, &stipple_cross8);
            SetBrushType(hdc,BT_STIPPLED);
            SetBrushOrigin(hdc,x+4,y);
            FillPolygon(hdc,pns,5);
        }
        else                             /*-- 16*16 Cross --*/
        {
            SetBrushInfo(hdc, NULL, &stipple_cross16);
            SetBrushType(hdc,BT_STIPPLED);
            SetBrushOrigin(hdc,x+9,y);  
            FillPolygon(hdc,pns,5);
        }

        break;

    case Symbol_Diagcross:   

        if(Symbol_8 == m_symbol_size)    /*-- 8*8 Diagcross --*/
        {
            SetBrushInfo(hdc, NULL, &stipple_diagcross8);
            SetBrushType(hdc,BT_STIPPLED);
            SetBrushOrigin(hdc,x+4,y+4);  
            FillPolygon(hdc,pns,5);
        }
        else                            /*-- 16*16 Diagcross --*/
        {
            SetBrushInfo(hdc, NULL, &stipple_diagcross16);
            SetBrushType(hdc,BT_STIPPLED);
            SetBrushOrigin(hdc,x+8,y);  
            FillPolygon(hdc,pns,5);
        }
          
        break;

    case Symbol_Circle:

        SetBrushType(hdc, BT_SOLID);
        if(Symbol_8 == m_symbol_size)    /*-- 8*8 Circle --*/
	        Ellipse(hdc, x+4, y+4, 4, 4);
    	else                             /*-- 16*16 Circle --*/
	        Ellipse(hdc, x+8, y+8, 8, 8);

        break;

    case Symbol_CircleFilled:

        SetBrushType(hdc, BT_SOLID);
        if(Symbol_8 == m_symbol_size)    /*-- 8*8 CircleFilled --*/
            FillCircle( hdc, x+4, y+4, 4);
        else                             /*-- 16*16 CircleFilled --*/
            FillCircle( hdc, x+8, y+8, 8);

        break;

    case Symbol_Square:

        SetBrushType(hdc, BT_SOLID);
        if(Symbol_8 == m_symbol_size)    /*-- 8*8 Square --*/
            Rectangle (hdc, x, y, x + 8, y + 8);
        else                             /*-- 16*16 Square --*/
            Rectangle (hdc, x, y, x + 16, y + 16);

        break;
    case Symbol_SquareFilled:

        SetBrushType(hdc, BT_SOLID);
        if(Symbol_8 == m_symbol_size)    /*-- 8*8 Square Filled --*/
            FillBox( hdc, x, y, 8, 8);
        else                             /*-- 16*16 Square Filled --*/
            FillBox( hdc, x, y, 16, 16);

	    break;
    case Symbol_Triangle:
	
        SetBrushType(hdc, BT_SOLID);
        PolyLineEx( hdc, pnsTri, 4);     /*-- 8*8/16*16 Triangle --*/

        break;
    case Symbol_TriangleFilled:
 
        SetBrushType(hdc, BT_SOLID);
      FillPolygon(hdc, pnsTri, 4);     /*-- 8*8/16*16 Triangle Filled--*/

        break;
    case Symbol_Star:    
        if(Symbol_8 == m_symbol_size)    /*-- 8*8 Star --*/
        {      
            SetBrushInfo(hdc, NULL, &stipple_star8);
            SetBrushType(hdc,BT_STIPPLED);
            SetBrushOrigin(hdc,x,y);
            FillPolygon(hdc,pns,5);
        }
        else                            /*-- 16*16 Star --*/
        {
            SetBrushInfo(hdc, NULL, &stipple_star16);
            SetBrushType(hdc,BT_STIPPLED);
            SetBrushOrigin(hdc,x+8,y);  
            FillPolygon(hdc,pns,5);
        } 

        break;
    case Symbol_StarFilled:              

       if(Symbol_8 == m_symbol_size)     /*-- 8*8 Filled Star --*/
       {
           SetBrushInfo(hdc, NULL, &stipple_starfilled8);
           SetBrushType(hdc,BT_STIPPLED);
           SetBrushOrigin(hdc,x+8,y);
           FillPolygon(hdc,pns,5);
       }
       else                              /*-- 16*16 Filled Star --*/
       {
            SetBrushInfo(hdc, NULL, &stipple_starfilled16);
            SetBrushType(hdc,BT_STIPPLED);
            SetBrushOrigin(hdc,x+8,y);  
            FillPolygon(hdc,pns,5);	   
       }

       break;
    case Symbol_VerticalBar:

        SetBrushType(hdc, BT_SOLID);
       if(Symbol_8 == m_symbol_size)     /*-- 8*8 VerticalBar --*/
	       FillBox(hdc, x, y, 2, 8);
       else                              /*-- 16*16 VerticalBar--*/
	       FillBox(hdc, x, y, 4, 16);

       break;
    default:
       break;
    }
}

void MEDC::drawLine(int x1, int y1, int x2, int y2)
{
    LineEx(hdc, x1, y1, x2, y2);    
}

void MEDC::drawRect(int x, int y, int width, int height)
{
    Rectangle (hdc, x, y, x + width, y + height);
} 

void MEDC::fillRect(int x, int y, int width, int height)
{
    FillBox( hdc, x, y, width, height);
}

void MEDC::drawPolygon( const MEPoint * pPts, int nPts )
{
    PolyLineEx( hdc, (POINT*)pPts, nPts );
}

void MEDC::fillPolygon( const MEPoint * pPts, int nPts )
{
    FillPolygon( hdc,(POINT*)pPts, nPts );
}

void MEDC::drawCircle(int x, int y, int r)
{
    Ellipse(hdc, x, y, r, r);
}

void MEDC::drawCircleArc(int x, int y, int r, float fAngleBegin, float fAngleEnd)
{
    ArcEx( hdc, x, y, 2*r, 2*r, 64*(int)fAngleBegin, 64*(int)fAngleEnd);
}

void MEDC::fillCircle(int x, int y, int r)
{
    FillCircle(hdc, x, y, r);
}

void MEDC::drawEllipse(int sx, int sy, int rx, int ry)
{
    Ellipse(hdc, sx, sy, rx, ry);
}

void MEDC::fillEllipse(int sx, int sy, int rx, int ry)
{
    FillEllipse(hdc, sx, sy, rx, ry);
}

void MEDC::drawText( const char * pszTextIn, int x, int y )
{
    TextOut( hdc, x, y, pszTextIn );  
}

void MEDC::drawBitmap(PBITMAP pBitmap, int x, int y, int w, int h )
{
    FillBoxWithBitmap( hdc, x, y, w, h, pBitmap );
}

