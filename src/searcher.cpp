/*
** $Id: searcher.cpp,v 1.5 2006-09-04 03:49:29 jpzhang Exp $
**
** Classes implement for Search 
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Hu Zheng.
**
** Create date: 2006/08/11
*/

#include "layer.h"
#include "searcher.h"

MESearcher::MESearcher()
{
}

MESearcher::~MESearcher()
{
}

void MESearcher::SearchLayer(MELayer* layer)
{
    layer->Search(this);
}


