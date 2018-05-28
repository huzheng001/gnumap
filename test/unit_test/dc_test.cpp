


#include <stdio.h>
#include <string.h>


#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "../../src/dc.h"

extern "C" {

static BITMAP bmp;

static char symbol_styles[12][20] = {
    {"Point"},
    {"Cross"},
    {"Diagcross"},
    {"  Circle"},
    {"  Filled"},
    {"  Square"},
    {" Filled"},
    {"Triangle"},
    {" Filled"},
    {" Star"},
    {" Filled"},
    {"VerticalBar"}
};

static const  SymbolStyle Symbol_Id[] = {
     Symbol_Point ,
     Symbol_Cross, 
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


static char pen_styles[9][20] = {
    {"    Solid"},
    {"  NullPen"},
    {"    Dash"},
    {"  ShortDash"},
    {"  LongDash"},
    {"   DotLine"},
    {"DashDotLine"},
    {"DashD-DLine"},
    {"AlterLine"}
};

static const PenStyle  Pen_Id[] = {
    Pen_Solid,
    Pen_Null,
    Pen_Dash,
    Pen_ShortDash,
    Pen_LongDash,
    Pen_DotLine,
    Pen_DashDotLine,
    Pen_DashDotDotLine,
    Pen_AlternateLine
};


static char pen_cap[3][20] = {
    {"    Butt"},
    {"    Round"},
    {"Projecting"}
};

static const PenCapStyle  Pen_Cap_Id[] = {
    Pen_Cap_Butt,
    Pen_Cap_Round,
    Pen_Cap_Projecting
};


static char pen_join[3][10] = {
    {"    Miter"},
    {"    Round"},
    {"    Bevel"}
};

static const PenJoinStyle  Pen_Join_Id[] = {Pen_Join_Miter,
     Pen_Join_Round,
     Pen_Join_Bevel};


static char brush_styles[8][20] = {
    {"    Solid"},
    {"    Null"},
    {"Horizontal"},
    {"Vertical"},
    {"FDiagonal"},
    {"BDiagonal"},
    {"CrossHatch"},
    {"Diagcross"}
};

static const BrushStyle  Brush_Id[] = {     
    Brush_Solid,
    Brush_Null,
    Brush_HorizontalHatch,
    Brush_VerticalHatch,
    Brush_FDiagonalHatch,
    Brush_BDiagonalHatch,
    Brush_CrossHatch,
    Brush_DiagcrossHatch
};


struct font_style
{
    char * fontname;
    char * charset;
    int size;
    BYTE flags;
    int angle;
    char * fcolor;
    char * bcolor;
}Fonts[]={
    {"fixed", FONT_CHARSET_GB18030_0, 16, 0, 0, "#000000", "#00000000"},
    {"Courier",FONT_CHARSET_GB2312_0 , 14, 0, 0, "#0000FF", "#00000000"},
    {"SansSerif",FONT_CHARSET_GB2312_0 , 12, 0, 0, "#00FF00", "#00000000"},
    {"SansSerif",FONT_CHARSET_GB1988_0 , 14, Font_Bold, 0, "#FF0000", "#00000000"},
    {"SansSerif",FONT_CHARSET_GB1988_0 , 16, Font_Italic | Font_Underline, 0, "#FF0000", "#00000000"},
    {"Times",FONT_CHARSET_GB2312_0 , 16, 0, 0, "#FFFFFF", "#000000"},
    {"Helvetica",FONT_CHARSET_GB2312_0 , 10, 0, 0, "#00FF00", "#FF0000"},
};

static int DCTestProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    int i = 0;
    int h = 0;
    MEDC *test = NULL;

    static MEColor red(255, 0, 0); 
    static MEColor black(0, 0, 0);   

    switch (message) 
    {
        case MSG_CREATE:
	    if( LoadBitmap(HDC_SCREEN, &bmp, "new2.jpg"))
	        return -1;
            return 0;
          
        case MSG_PAINT:
            hdc = BeginPaint(hWnd);
	    
            test = new MEDC(hdc);
	   	      

            /***************************************************************** 
             *                      test drawPoint                           *
             ****************************************************************/
            /* 8*8 bits */
            h = 0;
            test->setBrushFgColor(red);
            for(i=0; i<12; i++)
            {
                test->drawText(symbol_styles[i], i*70, h);
                test->setSymbolPattern(Symbol_Id[i]);
                test->drawPoint(i*70+30, h+30);
                test->setSymbolPattern(Symbol_Id[0]);
            }
            /* 16*16 bits */
            h = 60;
            test->setBrushFgColor(black);
            test->setSymbolSize(Symbol_16);
            for(i=0; i<12; i++)
            {
                test->drawText(symbol_styles[i], i*70, h);
                test->setSymbolPattern(Symbol_Id[i]);
                test->drawPoint(i*70+30, h+30);
                test->setSymbolPattern(Symbol_Id[0]);
            }
	    

            /*****************************************************************
             *                    test drawLine                              *
              ****************************************************************/
            /* drawLine on different Pen Pattern */
            h = 130;
            test->setPenWidth(5); 
            test->setPenColor(red);
            for(i=0; i<9; i++)
            {		
                test->drawText(pen_styles[i], i*100, h);
                test->setPenPattern(Pen_Id[i]);
                test->drawLine(i*100+5, h+30, i*100+95, h+30);
            }
	    
            /* drawLine on different Pen Cap */
            h = 180;
            test->setPenPattern(Pen_Solid);
            for(i=0; i<6; i++)
            {
                if(i<3)         /*setPenCap*/
                {
                    test->drawText(pen_cap[i], i*100, h);	
                    test->setPenCap(Pen_Cap_Id[i]);
                    test->drawLine(i*100+5, h+30, i*100+95, h+30);
                }
                else            /*setPenJoin*/         
                {
                    test->drawText(pen_join[i-3], i*100, h);
                    MEPoint pntsAngle[3] = {
                           {i*100+5, h+30},
                           {i*100+95,h+30},
                           {i*100+5, h+50}
                    }; 
                    test->setPenJoin(Pen_Join_Id[i-3]);
                    test->drawPolygon(pntsAngle, 3);		  
                }
            }


            /*****************************************************************
             *      test drawRect, drawCircle, drawEllipse, drawPolygon      *
             ****************************************************************/
            h = 220;
            test->setPenWidth(1);

            test->drawRect(15, h+20, 60, 60);
            test->drawCircle(150, h+50, 30);
            test->drawEllipse(250, h+50, 30, 20);
            //draw triangle
            static MEPoint pntsTri[4] = {
                    {350, h+20},
                    {400, h+80},
                    {300, h+80},
                    {350, h+20}
            };
            test->drawPolygon(pntsTri, 4);
            //draw  trapezoid
            static MEPoint pntsTrape[5] = {
                    {435,h+20},
                    {475,h+20},
                    {500,h+80},
                    {415,h+80},
                    {435,h+20}
            };
            test->drawPolygon(pntsTrape, 5);
            
            //draw pentagon
            static MEPoint pntsPent[6] = {
                    {575,h+20},
                    {600,h+50},
                    {580,h+80},
                    {530,h+60},
                    {515,h+30},
                    {575,h+20}
            };
            test->drawPolygon(pntsPent, 6);

            //draw bitmap
            test->drawBitmap(&bmp, 630, h-40, 128, 128);

            /****************************************************************
             *         test fill* on different Brush style                  *
             ***************************************************************/

            h = 320;
            for(i=0; i<8; i++)
            {
                test->drawText(brush_styles[i], i*100, h);
                test->setBrushType(Brush_Id[i]);
                //draw Circle
                test->fillCircle(i*100+50, h+70, 45);
                //draw Ellipse
                test->fillEllipse(i*100+50, h+150, 40, 20);
                //draw hexagon
                MEPoint pntsHgn[9] = {
                       {i*100+10+20, h+180},
                       {i*100+10+65, h+180},
                       {i*100+10+80, h+190},
                       {i*100+10+80, h+240},
                       {i*100+10+65, h+250},
                       {i*100+10+20, h+250},
                       {i*100+10,    h+240},
                       {i*100+10,    h+190},
                       {i*100+10+15, h+180}
                };
                test->fillPolygon(pntsHgn,9);
            } 


            /****************************************************************
             *         test font , charset and text color                   * 
             ***************************************************************/
            for (int i = 0; i < TABLESIZE(Fonts); i++)
            {
                char buf[256]={0};
                MEColor fc(Fonts[i].fcolor);
                MEColor bc(Fonts[i].bcolor);

                MEFont font(Fonts[i].fontname, Fonts[i].size, Fonts[i].flags, Fonts[i].charset, Fonts[i].angle);

                test->setFont(&font);
                test->setTextColor(fc);
                test->setBgColor(bc);
                sprintf (buf, "DC Font test  name:%s  size: %d , charset: %s", Fonts[i].fontname, Fonts[i].size, Fonts[i].charset);
                test->drawText(buf, 20, h+260+i*20);
                
                MEColor ffc("#000000");
                font.SetFontSize(16);
                font.SetRotatAngle(16);
                test->setTextColor(ffc);
                test->setFont(&font);
                test->drawText(buf, 500, h+260+i*20);

            }

            delete test;  


            EndPaint (hWnd, hdc);
            return 0;

        case MSG_CLOSE:
	    UnloadBitmap( &bmp );
            DestroyMainWindow (hWnd);
            PostQuitMessage (hWnd);
            return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

#ifdef _MGRM_PROCESSES
    JoinLayer(NULL , "dc test" , 0 , 0);
#endif


    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "DC Test";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = DCTestProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = g_rcScr.right;
    CreateInfo.by = g_rcScr.bottom;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    hMainWnd = CreateMainWindow (&CreateInfo);
    
    if (hMainWnd == HWND_INVALID)
        return -1;

    ShowWindow(hMainWnd, SW_SHOWNORMAL);

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (hMainWnd);

    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

}
