#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "meagle.h"
#include "view.h"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

BOOL RegisterMEagleControl (void)
{
    WNDCLASS MEViewClass;

    MEViewClass.spClassName = MEAGLE_CTRL;
    MEViewClass.dwStyle = WS_NONE;
    MEViewClass.dwExStyle = WS_EX_NONE;
    MEViewClass.hCursor = GetSystemCursor (IDC_ARROW);
    MEViewClass.iBkColor = COLOR_lightwhite;
    MEViewClass.WinProc = MEView::_on_message;

    return RegisterWindowClass (&MEViewClass);
}

void UnregisterMEagleControl (void)
{
    UnregisterWindowClass (MEAGLE_CTRL);
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
