#ifndef _MEAGLE_LAYER_H_
#define _MEAGLE_LAYER_H_


struct LayersName
{
    const char **LayersName;
    char **AvailableLayersName;
};
void open_layer_window(HWND hParent, struct LayersName* layname);
#endif
