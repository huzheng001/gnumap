/*
** $Id: style.cpp,v 1.20 2006-10-31 01:53:22 jpzhang Exp $
**
** Style class implementation.
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Zhang Ji Peng.
**
** Create date: 2006/08/11
*/

#include "assert.h"
#include "feature.h"
#include "style.h"

volatile MEStyle * MEStyle::last_style = NULL;


MEStyle * MEStyle::CreateStyle(const char *pszStyleString, MEDC * dc)
{
    MEStyle * pret = NULL;

    if (NULL == pszStyleString)
        return NULL;
//try to get it from the pool
    pret = METoolPool::GetStyle (pszStyleString);
    if (pret) 
        return pret;

//create a new one
    OGRStyleMgr mgr;
    mgr.InitStyleString(pszStyleString);

    int count = mgr.GetPartCount();
    if (count < 1)
        return NULL;

    pret = new MEStyle;
    OGRStyleTool *tool;

    for(int i = 0; i < count; i++)
    {
        tool = mgr.GetPart(i);
        tool->SetUnit(OGRSTUPixel);//set uint pixel
        
        MEStyleTool* style_tool = MEStyleTool::Create(tool , dc);
        if(style_tool != NULL)
            pret->Add(style_tool);
        delete tool;
    }

//add new style to pool
    METoolPool::AddStyle (strdup(pszStyleString), pret);
    return pret;
}

MEStyle::MEStyle()
{
}

MEStyle::~MEStyle()
{
    pTools.clear();
}


void MEStyle::Add(MEStyleTool *pmTool)
{
    assert(pmTool != NULL); // pmTool is not NULL

    pTools.push_back (pmTool);
}

void MEStyle::SetDC(MEDC *pmDC, MEMapFeature *feature)
{
    assert(pmDC != NULL ); // pmDC is not NULL

    std::vector<MEStyleTool*>::iterator i;

    for (i = pTools.begin(); i != pTools.end(); i++)
        (*i)->SetDC(pmDC, feature);
}

void MEStyle::ResetDC(MEDC *pmDC, MEMapFeature *feature)
{
    assert(pmDC != NULL); // pmDC is not NULL

    std::vector<MEStyleTool*>::iterator i;

    for (i = pTools.begin(); i != pTools.end(); i++)
        (*i)->ResetDC(pmDC, feature);
}

