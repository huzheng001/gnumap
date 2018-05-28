/*
 * $Id: styletool.cpp,v 1.10 2006-09-30 09:09:48 jpzhang Exp $
 *
 * Class  implementation.
 *
 * Copyright (C) 2006 Feynman Software.
 *
 * Create date: 2006/08/11
 *
 */

#include <string>

#include "style.h"

using std::string;

MEStyleTool * MEStyleTool::Create (OGRStyleTool *tool, MEDC * dc)
{
    MEStyleTool * ptool = NULL;

    if (NULL == tool)
       return NULL;

    const char * tool_str = tool->GetStyleString(); 
//try get it from pool
    ptool = METoolPool::GetTool (tool_str);
    if (ptool)
        return ptool;

//create a new one
    switch(tool->GetType())
    {
    case OGRSTCPen:
        ptool = new MEStylePen((OGRStylePen*)tool, dc);
        break;
    case OGRSTCBrush:
        ptool = new MEStyleBrush((OGRStyleBrush*)tool, dc);
        break;
    case OGRSTCSymbol:
        ptool = new MEStyleSymbol((OGRStyleSymbol*)tool, dc);
        break;
    case OGRSTCLabel:
        ptool = new MEStyleLabel((OGRStyleLabel*)tool, dc);
        break;
    default:
        return NULL;
    }
        
//add new tool to pool
    METoolPool::AddTool (strdup(tool_str), ptool);
    return ptool;
} 




MEStyleTool::MEStyleTool()
    :m_nPriority(1)
{
   
}

MEStyleTool::~MEStyleTool()
{
}


METoolType MEStyleTool::GetType() const
{
    return m_tool_type;
}

int MEStyleTool::GetPriority (void) const
{
    return m_nPriority;
}

int MEStyleTool::GetToolId (const char * id_str)
{
    string in_str(id_str);
    string::size_type pos=0;
    int id = 0;
    pos = in_str.find_last_of("-");
    if(pos != string::npos){
        sscanf(in_str.substr(pos+1).c_str(),"%d",&id) ;
    }
    return id;
}
