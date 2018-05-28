/*
** $Id: eye.cpp,v 1.8 2006-08-30 09:02:25 jpzhang Exp $
**
** Eagle eye class implement
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Yang Yan.
**
** Create date: 2006/08/23
*/

#include "eye.h"
#include "eyeview.h"

MEEye::MEEye(HDC hdc, MEEyeView * view, int width, int height)
    :map(NULL),m_view(view),back_dc(hdc),eye_width(width),eye_height(height),
    eye_x0(0.0),eye_y0(0.0),eye_pixel_length(0.0),back_is_draw(false)
{
}

MEEye::~MEEye()
{
}

void MEEye::UpdateView(void)
{
    m_view->Refresh();
}
