/*
** $Id: pool.cpp,v 1.11 2006-10-31 01:53:22 jpzhang Exp $
**
** Pool class define
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

#include <map>
#include <utility>
#include <algorithm>

#include "style.h"

static std::map<const char*,MEStyleTool*> mToolPool;
static std::map<const char*,MEStyle*> mStylePool;

//function object
class PoolFindStyle
{
public:
    PoolFindStyle(const char * str):m_str(str){};
    ~PoolFindStyle(){}

    bool operator()(std::pair<const char*, MEStyle*> p) const
    {
        if (strcmp(m_str, p.first)==0)
            return true;

        return false;
    } 
private:
    const char * m_str;
};

class PoolFindTool
{
public:
    PoolFindTool(const char * str):m_str(str){};
    ~PoolFindTool(){}

    bool operator()(std::pair<const char*, MEStyleTool*> p) const
    {
        if (strcmp(m_str, p.first)==0)
            return true;

        return false;
    } 
private:
    const char * m_str;
};



// pool classes
void METoolPool::ClearAll(void)
{
    //delete objects
    std::map<const char*,MEStyleTool*>::iterator pt;
    for (pt = mToolPool.begin(); pt != mToolPool.end(); pt++) {
        free ((char*)(pt->first));
        delete pt->second;
    }


    std::map<const char*,MEStyle*>::iterator ps;
    for (ps = mStylePool.begin(); ps != mStylePool.end(); ps++){
        free ((char*)(ps->first));
        delete ps->second;
    }

    //clear cache
    mToolPool.clear();
    mStylePool.clear();    
}

MEStyleTool * METoolPool::GetTool(const char *pszToolString)
{
    std::map<const char*,MEStyleTool*>::iterator p;
    
    p = find_if (mToolPool.begin(), mToolPool.end(), PoolFindTool(pszToolString));
    if (p == mToolPool.end())
        return NULL;

    return p->second;
}

void METoolPool::GetStyleInfo(std::vector<const char *> &Styles, std::vector<MEStyle *> &StylePts)
{
    for (std::map<const char*,MEStyle*>::iterator i = mStylePool.begin(); i != mStylePool.end(); ++i)
    {
        Styles.push_back(i->first);
        StylePts.push_back(i->second);
    }
}

MEStyle * METoolPool::GetStyle(const char *pszStyleString)
{
    std::map<const char*,MEStyle*>::iterator p;

    p = find_if (mStylePool.begin(), mStylePool.end(), PoolFindStyle(pszStyleString));
    if (p == mStylePool.end())
        return NULL;

    return p->second;
}

void METoolPool::AddTool(const char *pszToolString, MEStyleTool *pmTool)
{
    mToolPool.insert (std::make_pair(pszToolString, pmTool));
}

void METoolPool::AddStyle(const char *pszStyleString, MEStyle *pmStyle)
{
    mStylePool.insert (std::make_pair(pszStyleString, pmStyle));
}
