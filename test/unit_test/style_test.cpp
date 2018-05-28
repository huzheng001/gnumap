
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "ogrsf_frmts.h"
#include "cpl_conv.h"
#include "cpl_string.h"

#include "../../src/style.h"

#define IDC_PEN      110
#define IDC_BRUSH    111
#define IDC_SYMBOL   112


extern "C" {

const int s_w = 800;
const int s_h = 600;

static int d_flag = 0;

struct 
{
    char* style_string;
    char* text;
} pen_styles[] =
{
    {"PEN(w:1px,c:#0000cc,id:\"mapinfo-pen-1.ogr-pen-0\")", "   Solid"},
    {"PEN(w:4px,c:#0000cc,id:\"mapinfo-pen-1.ogr-pen-0\")", "Solid(4w)"},
    {"PEN(w:1px,c:#0000cc,id:\"mapinfo-pen-1.ogr-pen-1\")", "   NullPen"},
    {"PEN(w:1px,c:#0000cc,id:\"mapinfo-pen-1.ogr-pen-2\")", "    Dash"},
    {"PEN(w:4px,cap:r,c:#0000cc,id:\"mapinfo-pen-1.ogr-pen-2\")", "Dash(4w,cap(r))"},
    {"PEN(w:1px,c:#0000cc,id:\"mapinfo-pen-1.ogr-pen-3\")", "   ShortDash"},
    {"PEN(w:4px,c:#0000cc,id:\"mapinfo-pen-1.ogr-pen-3\")", "ShortDash(4w)"},
    {"PEN(w:4px,cap:p,c:#0000cc,id:\"mapinfo-pen-1.ogr-pen-3\")", "ShortDash(4w,cap:p)"},
    {"PEN(w:1px,c:#0000cc,id:\"mapinfo-pen-1.ogr-pen-4\")", " LongDash"},
    {"PEN(w:4px,c:#0000cc,id:\"mapinfo-pen-1.ogr-pen-4\")", "LongDash(4w)"},
    {"PEN(w:1px,c:#0000cc,id:\"mapinfo-pen-1.ogr-pen-5\")", "  DotLine"},
    {"PEN(w:4px,c:#0000cc,id:\"mapinfo-pen-1.ogr-pen-5\")", "DotLine(4w)"},
    {"PEN(w:1px,c:#0000cc,id:\"mapinfo-pen-1.ogr-pen-6\")", " DashDotLine"},
    {"PEN(w:4px,c:#0000cc,id:\"mapinfo-pen-1.ogr-pen-6\")", "DashDotLine(4w)"},
    {"PEN(w:4px,cap:r,c:#0000cc,id:\"mapinfo-pen-1.ogr-pen-6\")", "DashDotLine(4w,cap(r))"},
    {"PEN(w:1px,c:#0000cc,id:\"mapinfo-pen-1.ogr-pen-7\")", " DashD-DLine"},
    {"PEN(w:4px,c:#0000cc,id:\"mapinfo-pen-1.ogr-pen-7\")", "DashD-DLine(4w)"},
    {"PEN(w:1px,c:#0000cc,id:\"mapinfo-pen-1.ogr-pen-8\")", "  AlterLine"},
}, compare_two_pen[] =
{
    {"PEN(w:8px,c:#0000cc,id:\"mapinfo-pen-1.ogr-pen-0\")", "Normal(w:8)"},
    {"PEN(cap:r,w:8px,c:#0000cc,id:\"mapinfo-pen-1.ogr-pen-0\")", "width(8)cap(r)"},
    {"PEN(w:8px,c:#0000cc,id:\"mapinfo-pen-1.ogr-pen-0\")", "Normal(w:8)"},
    {"PEN(cap:p,w:8px,c:#0000cc,id:\"mapinfo-pen-1.ogr-pen-0\")", "width(8)cap(p)"},
}, join_pen[] =
{
    {"PEN(w:10px,c:#0000cc,j:m,id:\"mapinfo-pen-1.ogr-pen-0\")", "width(10)join(m)"},
    {"PEN(w:10px,c:#0000cc,j:r,id:\"mapinfo-pen-1.ogr-pen-0\")", "width(10)join(r)"},
    {"PEN(j:b,w:10px,c:#0000cc,id:\"mapinfo-pen-1.ogr-pen-0\")", "width(10)join(b)"}
};


struct 
{
    char* style_string;
    char* text;
}brush_styles[] =
{
    {"BRUSH(fc:#0000FF,bc:#ffff00,s:16px,id:\"mapinfo-brush-13.ogr-brush-0\")", "     Solid"},
    {"BRUSH(fc:#0000FF,bc:#ffff00,s:16px,id:\"mapinfo-brush-13.ogr-brush-1\")", "   NullBrush"},
    {"BRUSH(fc:#0000FF,bc:#ffff00,s:16px,id:\"mapinfo-brush-13.ogr-brush-2)", " Hor_Hatch( - )"},
    {"BRUSH(fc:#0000FF,bc:#ffff00,s:16px,id:\"mapinfo-brush-13.ogr-brush-3\")", " Ver_Hatch( | )"},
    {"BRUSH(fc:#0000FF,bc:#ffff00,s:16px,id:\"mapinfo-brush-13.ogr-brush-4\")", "FDia_Hatch( \\ )"},
    {"BRUSH(fc:#0000FF,bc:#ffff00,s:16px,id:\"mapinfo-brush-13.ogr-brush-5\")", "BDia_Hatch( / )"},
    {"BRUSH(fc:#0000FF,bc:#ffff00,s:16px,id:\"mapinfo-brush-13.ogr-brush-6\")", "Cross_Hatch( + )"},
    {"BRUSH(fc:#0000FF,bc:#ffff00,s:16px,id:\"mapinfo-brush-13.ogr-brush-7\")", "DiaCro_Hatch( x )"},
    {"BRUSH(fc:#0000FF,bc:#ffff00,s:16px,a:30,id:\"mapinfo-brush-13.ogr-brush-2\")", "Hor_Hatch (30)"},
    {"BRUSH(fc:#0000FF,bc:#ffff00,s:16px,a:60,id:\"mapinfo-brush-13.ogr-brush-2\")", "Hor_Hatch (60)"},
    {"BRUSH(fc:#0000FF,bc:#ffff00,s:16px,a:-30,id:\"mapinfo-brush-13.ogr-brush-2\")", "Hor_Hatch (-30)"},
    {"BRUSH(fc:#0000FF,bc:#ffff00,s:16px,a:-60,id:\"mapinfo-brush-13.ogr-brush-2\")", "Hor_Hatch (-60)"},
    {"BRUSH(fc:#0000FF,bc:#ffff00,s:16px,a:0,id:\"mapinfo-brush-13.ogr-brush-2\")", "Hor_Hatch (0)"},
    {"BRUSH(fc:#0000FF,bc:#ffff00,s:16px,a:45,id:\"mapinfo-brush-13.ogr-brush-2\")", "Hor_Hatch (45)"},
    {"BRUSH(fc:#0000FF,bc:#ffff00,s:16px,a:-45,id:\"mapinfo-brush-13.ogr-brush-2\")", "Hor_Hatch (-45)"},
    {"BRUSH(fc:#0000FF,bc:#ffff00,s:16px,a:90,id:\"mapinfo-brush-13.ogr-brush-2\")", "Hor_Hatch (90)"},
    {"BRUSH(fc:#0000FF,bc:#ffff00,s:16px,a:45,id:\"mapinfo-brush-13.ogr-brush-5\")", "BDia_Hatch(45)"},
    {"BRUSH(fc:#0000FF,bc:#ffff00,s:16px,a:45,id:\"mapinfo-brush-13.ogr-brush-6\")", "Cross_Hatch(45)"},
    {"BRUSH(fc:#0000FF,bc:#ffff00,s:16px,a:90,id:\"mapinfo-brush-13.ogr-brush-6\")", "Cross_Hatch(90)"},
    {"BRUSH(fc:#0000FF,bc:#ffff00,s:16px,a:45,id:\"mapinfo-brush-13.ogr-brush-7\")", "DiaCro_Hatch(45)"},
};

struct 
{
    char* style_string;
    char* text;
}symbol_styles[] =
{
    {"SYMBOL(a:0,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-0\")","   Cross"},
    {"SYMBOL(a:0,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-1\")"," DiaCross"},
    {"SYMBOL(a:0,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-2\")","  Circle"},
    {"SYMBOL(a:0,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-3\")","CircleFill"},
    {"SYMBOL(a:0,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-4\")","  Square"},
    {"SYMBOL(a:0,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-5\")","SquareFill"},
    {"SYMBOL(a:0,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-6\")","Triangle"},
    {"SYMBOL(a:0,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-7\")","TriangleFill"},
    {"SYMBOL(a:0,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-8\")","   Star"},
    {"SYMBOL(a:0,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-9\")","StarFill"},
    {"SYMBOL(a:0,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-10\")","VerticalBar"},
    {"SYMBOL(a:30,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-0\")","Cross(30)"},
    {"SYMBOL(a:45,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-0\")","Cross(45)"},
    {"SYMBOL(a:60,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-0\")","Cross(60)"},
    {"SYMBOL(a:-30,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-0\")","Cross(-30)"},
    {"SYMBOL(a:-60,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-0\")","Cross(-60)"},
    {"SYMBOL(a:30,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-1\")","DiaCross(30)"},
    {"SYMBOL(a:45,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-1\")","DiaCross(45)"},
    {"SYMBOL(a:30,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-4\")","Square(30)"},
    {"SYMBOL(a:45,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-4\")","Square(45)"},
    {"SYMBOL(a:60,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-4\")","Square(60)"},
    {"SYMBOL(a:90,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-4\")","Square(90)"},
    {"SYMBOL(a:0,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-6\")","Triangle(0)"},
    {"SYMBOL(a:30,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-6\")","Triangle(30)"},
    {"SYMBOL(a:60,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-6\")","Triangle(60)"},
    {"SYMBOL(a:90,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-6\")","Triangle(90)"},
    {"SYMBOL(a:180,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-6\")","Triangle(180)"},
    {"SYMBOL(a:0,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-8\")","  Star(0)"},
    {"SYMBOL(a:30,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-8\")","  Star(30)"},
    {"SYMBOL(a:60,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-8\")","  Star(60)"},
    {"SYMBOL(a:90,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-8\")","  Star(90)"},
    {"SYMBOL(a:180,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-8\")","  Star(180)"},
    {"SYMBOL(a:0,c:#0000ff,s:20pt,id:\"mapinfo-sym-52.ogr-sym-8\")","Star(20s)"},
    {"SYMBOL(a:0,c:#0000ff,s:10pt,id:\"mapinfo-sym-52.ogr-sym-8\")","Star(10s)"},
    {"SYMBOL(a:0,c:#0000ff,s:5pt,id:\"mapinfo-sym-52.ogr-sym-8\")","Star(5s)"},
    {"SYMBOL(a:0,c:#0000ff,s:20pt,id:\"mapinfo-sym-52.ogr-sym-10\")","VBar(20s)"},
    {"SYMBOL(a:45,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-10\")","VBar(45a)"},
    {"SYMBOL(a:90,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-10\")","VBar(90a)"},
    {"SYMBOL(a:180,c:#0000ff,s:40pt,id:\"mapinfo-sym-52.ogr-sym-10\")","VBar(180a)"},
    {"SYMBOL(a:180,c:#000080,s:5pt,id:\"mapinfo-sym-37.ogr-sym-8\")", "_Star(180a)"}
};
static void draw_pen (HDC hdc)
{
    MEDC* dc = new MEDC(hdc);
    MEStyle* style = NULL;
    int text_x = 50, text_y = 50, begin_x = 50, y = 100, end_x;
    unsigned int i = 0;
    while( i<TABLESIZE(pen_styles) )
    {
        end_x = begin_x + 130;
        TextOut( hdc, text_x, text_y, pen_styles[i].text );
        style = MEStyle::CreateStyle( pen_styles[i].style_string ,dc);
        style->SetDC(dc, NULL);
        dc->drawLine(begin_x, y, end_x, y);
        if( begin_x > 800 - 250 )
        {
            text_x = 50;
            text_y += 100;
            begin_x = 50;
            y += 100;
        }
        else
        {
            text_x  += 150;
            begin_x += 150;
        }
        i++;
    }

    i = 0;
    int tmp = text_y;
    while( i<TABLESIZE(compare_two_pen) )
    {
        end_x = begin_x + 130;
        y = tmp + 30;
        text_y = tmp;
        TextOut( hdc, text_x, text_y, compare_two_pen[i].text );
        style = MEStyle::CreateStyle( compare_two_pen[i].style_string,dc );
        style->SetDC(dc, NULL);
        dc->drawLine(begin_x, y, end_x, y);
        i++;
        y += 10;
        text_y = y+20;
        
        style = MEStyle::CreateStyle( compare_two_pen[i].style_string,dc );
        style->SetDC(dc, NULL);
        dc->drawLine(begin_x, y, end_x, y);
        TextOut( hdc, text_x, text_y, compare_two_pen[i].text );
        
        if( begin_x > 800 - 250 )
        {
            text_x = 50;
            text_y += 100;
            begin_x = 50;
            y += 100;
        }
        else
        {
            text_x  += 150;
            begin_x += 150;
        }
        i++;
        
    } 
    i = 0;
    tmp = text_y;
    while( i<TABLESIZE(join_pen) )
    {
        end_x = begin_x + 130;
        y = tmp + 50;
        text_y = tmp;
        MEPoint points[3]={{begin_x,y},{end_x,y},{begin_x,y+40}};

        TextOut( hdc, text_x, text_y, join_pen[i].text );
        style = MEStyle::CreateStyle( join_pen[i].style_string ,dc);
        style->SetDC(dc , NULL);
        dc->drawPolygon (points,3);
        style->ResetDC(dc,NULL);
        
        if( begin_x > 800 - 250 )
        {
            text_x = 50;
            text_y += 100;
            begin_x = 50;
            y += 100;
        }
        else
        {
            text_x  += 150;
            begin_x += 150;
        }
        i++;
    } 
    delete dc;
}

static void draw_brush (HDC hdc)
{
    SetPenWidth(hdc, 1);
    MEDC* dc = new MEDC(hdc);
    MEStyle* style = NULL;
    int top, bottom, left, right;
    
    unsigned int i = 0;
    top = 70;
    left = 50;
    while( i < TABLESIZE(brush_styles) )
    {
        bottom = top + 100;
        right = left + 100;

        MEPoint draw_points[5] ={
            {left, top},
            {left, bottom},
            {right, bottom},
            {right, top},
            {left, top}
        };

        TextOut( hdc, left, top - 30, brush_styles[i].text );
        style = MEStyle::CreateStyle( brush_styles[i].style_string,dc );
        style->SetDC(dc, NULL);
        dc->fillPolygon(draw_points,5);
        dc->drawPolygon(draw_points,5);
        style->ResetDC(dc, NULL);
        i++;

        left += 150;
        if(left > 800-100)
        {
            left = 50;
            top += 150;
        }
    }
    delete dc;
}

static void draw_symbol (HDC hdc)
{
    SetPenWidth(hdc, 1);
    MEDC* dc = new MEDC(hdc);
    MEStyle* style = NULL;
    int text_x = 10, text_y = 70, draw_x = 40, draw_y = 110;
    unsigned int i = 0;
    while( i<TABLESIZE(symbol_styles) )
    {
        TextOut( hdc, text_x, text_y, symbol_styles[i].text );
        style = MEStyle::CreateStyle( symbol_styles[i].style_string ,dc);
        style->SetDC(dc,NULL);
        dc->drawPoint(draw_x, draw_y);
        style->ResetDC(dc,NULL);
        i++;
        if( i == 0 || i%7 )
        {
            text_x += 110;
            draw_x += 110;
        }
        else
        {
            text_x = 10;
            text_y += 100;
            draw_x = 40;
            draw_y += 100;
        }
    }
    /*
    TextOut( hdc, text_x, text_y, "  Bitmap" );
    MGisSymbolBitmap bitmap("icon.bmp");
    bitmap.update();
    bitmap.draw_point( dc, draw_x, draw_y ); 
    */
    delete dc;
}
static int TestPen_Proc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam) 
{
    HDC hdc;
    switch (message) 
    {
        case MSG_CREATE:

            CreateWindow (CTRL_BUTTON,"pen",BS_DEFPUSHBUTTON | WS_VISIBLE,IDC_PEN,10,5,80,25,hDlg,0);
            CreateWindow (CTRL_BUTTON,"brush",BS_DEFPUSHBUTTON | WS_VISIBLE,IDC_BRUSH,100,5,80,25,hDlg,0);
            CreateWindow (CTRL_BUTTON,"symbol",BS_DEFPUSHBUTTON | WS_VISIBLE,IDC_SYMBOL,190,5,80,25,hDlg,0);

            break;
        case MSG_PAINT:
            hdc = BeginPaint(hDlg);
            
            if (d_flag == 0)
                draw_pen (hdc);
            else if (d_flag == 1)
                draw_brush(hdc);
            else if (d_flag == 2)
                draw_symbol(hdc);
            
            EndPaint(hDlg, hdc);
            break;
        case MSG_COMMAND:
            switch (wParam) {
            case IDC_PEN:
                d_flag = 0;
                InvalidateRect(hDlg, NULL, TRUE);
                break;
            case IDC_BRUSH:
                d_flag = 1;
                InvalidateRect(hDlg, NULL, TRUE);
                break;
            case IDC_SYMBOL:
                d_flag = 2;
                InvalidateRect(hDlg, NULL, TRUE);
                break;
            }
            break;
        case MSG_DESTROY:
            DestroyAllControls (hDlg);
            break;
        case MSG_CLOSE:
            METoolPool::ClearAll();
            DestroyMainWindow (hDlg);
            PostQuitMessage (hDlg);
            return 0;
    }
    return DefaultMainWinProc (hDlg, message, wParam, lParam);
}


int MiniGUIMain(int argc, const char **argv)
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

#if defined (_MGRM_PROCESSES) 
    JoinLayer(NULL, "style_test", 0 , 0);
#endif

    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "Test_Styles";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = TestPen_Proc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = 800;
    CreateInfo.by = 680;
    CreateInfo.iBkColor =COLOR_lightgray;
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
