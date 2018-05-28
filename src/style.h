/*
** $Id: style.h,v 1.27 2006-10-31 01:53:22 jpzhang Exp $
**
** Style class define
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Zhang Ji Peng.
**
** Create date: 2006/08/10
**
*/


#ifndef _STYLE_H_
#define _STYLE_H_

#include <vector>

#include "ogr.h"
#include "dc.h"

class MEStyleTool;
class MEStyle;
class MEMapFeature;

class METoolPool
{
private:
    METoolPool();
    ~METoolPool();

public:
    static MEStyleTool *GetTool(const char *pszToolString);
    static MEStyle *GetStyle(const char *pszStyleString);
    static void GetStyleInfo(std::vector<const char *> &Styles, std::vector<MEStyle *> &StylePts);
    static void AddTool(const char *pszToolString, MEStyleTool *pmTool);
    static void AddStyle(const char *pszStyleString, MEStyle *pmStyle);
    static void ClearAll (void);
};


//style class of feature
class MEStyle
{
public:
    MEStyle();
    ~MEStyle();

    static MEStyle * CreateStyle(const char *pszStyleString, MEDC * dc);

    void Add(MEStyleTool *pmTool);

    void SetDC(MEDC *pmDC, MEMapFeature *feature);
    void ResetDC(MEDC *pmDC, MEMapFeature *feature);

    static volatile MEStyle * last_style;

private:
    MEStyle(const MEStyle & o);
    MEStyle & operator=(const MEStyle & o);

    std::vector<MEStyleTool *> pTools;

};


enum METoolType{
     METOOL_PEN = 0,
     METOOL_BRUSH,
     METOOL_SYMBOL,
     METOOL_LABEL
};
//style tool class 
class MEStyleTool
{
public:
    virtual ~MEStyleTool();

    static MEStyleTool * Create (OGRStyleTool *tool , MEDC * dc); 

    virtual void SetDC(MEDC *pmDC, MEMapFeature * feature) = 0;
    virtual void ResetDC(MEDC *pmDC, MEMapFeature * feature) = 0;

    METoolType GetType(void) const;
    int GetPriority (void) const;

    int GetToolId (const char * id_str);
protected:
    MEStyleTool();
    int m_nPriority;
    METoolType m_tool_type;
};


enum MEPenCapStyle{
     PEN_CAP_BUTT=0,
     PEN_CAP_ROUND,
     PEN_CAP_PROJECTING
};
enum MEPenJoinStyle{
     PEN_JOIN_MITER=0,
     PEN_JOIN_ROUND,
     PEN_JOIN_BEVEL
};
enum MEPenId{
     PEN_SOLID=0,
     PEN_NULL,
     PEN_DASH,
     PEN_SHORTDASH,
     PEN_LONGDASH,
     PEN_DOTLINE,
     PEN_DASHDOTLINE,
     PEN_DASHDOTDOTLINE,
     PEN_ALTERNATELINE
};
//class of style pen
class MEStylePen : public MEStyleTool
{
public:
    MEStylePen(OGRStylePen *poStylePen, MEDC * dc);
    virtual ~MEStylePen(); 

    void SetDC(MEDC *pmDC, MEMapFeature * feature);
    void ResetDC(MEDC *pmDC, MEMapFeature * feature);

private:
    MEStylePen(const MEStylePen & o);
    MEStylePen & operator=(const MEStylePen & o);

    int width;
    MEColor *color; 
    char * pattern; 
    double pre_offset;
    MEPenCapStyle cap; 
    MEPenJoinStyle join; 
    MEPenId id;
};


enum MEBrushId {
     BRUSH_SOLID=0,
     BRUSH_NULL,
     BRUSH_HORIZONTALHATCH,
     BRUSH_VERTICALHATCH,
     BRUSH_FDIAGONALHATCH,
     BRUSH_BDIAGONALHATCH,
     BRUSH_CROSSHATCH,
     BRUSH_DIAGCROSSHATCH
};

//class of style brush
class  MEStyleBrush : public MEStyleTool
{
public:
    MEStyleBrush(OGRStyleBrush *psStyleBrush, MEDC * dc);
    virtual ~MEStyleBrush();

    void SetDC(MEDC *pmDC, MEMapFeature * feature);
    void ResetDC(MEDC *pmDC, MEMapFeature * feature);

private:
    MEStyleBrush(const MEStyleBrush & o);
    MEStyleBrush & operator=(const MEStyleBrush & o);

    MEColor *fg_color; 
    MEColor *bg_color; 
    double angle;
    double size; 
    double space_x; 
    double space_y;
    MEBrushId  id; 
};


enum MESymbolId{
     SYMBOL_CROSS=0,
     SYMBOL_DIAGCROSS,
     SYMBOL_CIRCLE,
     SYMBOL_CIRCLEFILLED,
     SYMBOL_SQUARE,
     SYMBOL_SQUAREFILLED,
     SYMBOL_TRIANGLE,
     SYMBOL_TRIANGLEFILLED,
     SYMBOL_STAR,
     SYMBOL_STARFILLED,
     SYMBOL_VERTICALBAR
};
//class of style symbol
class MEStyleSymbol : public MEStyleTool
{
public:
    MEStyleSymbol(OGRStyleSymbol *poStyleSymbol, MEDC * dc);
    virtual ~MEStyleSymbol();

    void SetDC(MEDC *pmDC, MEMapFeature * feature);
    void ResetDC(MEDC *pmDC, MEMapFeature * feature);

private:
    MEStyleSymbol(const MEStyleSymbol & o);
    MEStyleSymbol & operator=(const MEStyleSymbol & o);

    MEColor * color; 
    double angle;
    double size; 
    double space_x;
    double space_y;
    double step;
    double perp;
    double offset; 
    MESymbolId id;
};

enum MEPlacement
{
    LABEL_MP=0,
    LABEL_ML,
    LABEL_MS,
    LABEL_MM,
    LABEL_MW
};

class MEStyleLabel : public MEStyleTool
{
public:
    MEStyleLabel(OGRStyleLabel *poStyleLabel, MEDC * dc);
    virtual ~MEStyleLabel();

    void SetDC(MEDC *pmDC, MEMapFeature * feature);
    void ResetDC(MEDC *pmDC, MEMapFeature * feature);
private:
    MEStyleLabel(const MEStyleLabel & o);
    MEStyleLabel & operator=(const MEStyleLabel & o);

    char * font_name;
    double size;
    char * text_string;
    double angle;
    MEColor * fc;
    MEColor * bc;
    MEPlacement mode;
    int anchor;
    double s_x;
    double s_y;
    double perp;
    bool bold;
    bool italic;
    bool underline;
    MEFont * m_font;
};

#endif /* _STYLE_H_ */

