#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "meagle.h"
#include "eyeview.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

BOOL RegisterMEagleEyeControl (void)
{
    WNDCLASS MEEyeViewClass;

    MEEyeViewClass.spClassName = MEAGLEEYE_CTRL;
    MEEyeViewClass.dwStyle = WS_NONE;
    MEEyeViewClass.dwExStyle = WS_EX_NONE;
    MEEyeViewClass.hCursor = GetSystemCursor (IDC_ARROW);
    MEEyeViewClass.iBkColor = COLOR_lightwhite;
    MEEyeViewClass.WinProc = MEEyeView::_on_eye_message;

    return RegisterWindowClass (&MEEyeViewClass);
}

void UnregisterMEagleEyeControl (void)
{
    UnregisterWindowClass (MEAGLEEYE_CTRL);
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
