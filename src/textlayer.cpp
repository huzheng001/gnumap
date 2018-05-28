#include "feature.h"
#include "layer.h"

METextLayer::METextLayer(int width, int height, MEMap* parent_map)
    :MELayer(parent_map),winrect_width(width),winrect_height(height)
{
    InitFreeClipRectList(&cliprc_heap, 100);
    InitClipRgn(&cliprgn, &cliprc_heap);
    InitFreeClipRectList(&display_cliprc_heap, 100);
    InitClipRgn(&display_cliprgn, &display_cliprc_heap);
}

METextLayer::~METextLayer()
{
    Reset();
    ResultReset();
    DestroyFreeClipRectList(&cliprc_heap);
    DestroyFreeClipRectList(&display_cliprc_heap);
}

char * METextLayer::GetName(void) const
{
    return "meagle_text_layer";
}

void METextLayer::Search(MESearcher* searcher)
{
}

void METextLayer::Draw(MEDC* dc)
{
    for (std::list<METextFeature*>::iterator i = textlist.begin(); i != textlist.end(); i++) 
        (*i)->Draw(dc);
    for (std::list<METextFeature*>::iterator i = result_textlist.begin(); i != result_textlist.end(); i++) 
        (*i)->Draw(dc);
}

void METextLayer::Reset(void)
{
    for (std::list<METextFeature*>::iterator i = textlist.begin(); i != textlist.end(); i++)
        delete (*i);

    textlist.clear();
    EmptyClipRgn(&cliprgn);
}

void METextLayer::ResultReset(void)
{
    for (std::list<METextFeature*>::iterator i = result_textlist.begin(); i != result_textlist.end(); i++)
        delete (*i);

    result_textlist.clear();
    EmptyClipRgn(&display_cliprgn);
}

bool METextLayer::AddText(METextFeature *text)
{
    if (text->PointInRect(winrect_width, winrect_height) && !rect_in_region(text))
    {
        AddClipRect(&cliprgn, (RECT*)text->GetTextRect());
        textlist.push_back(text);
        return true;
    }
    return false;
}

bool METextLayer::AddResultText(METextFeature *text)
{
        if (text->PointInRect(winrect_width, winrect_height) && !rect_in_result_region(text))
        {
            AddClipRect(&display_cliprgn, (RECT*)text->GetTextRect());
            result_textlist.push_back(text);
            return true;
        }
        return false;
}

int METextLayer::rect_in_region(METextFeature* text)
{
    return  RectInRegion(&cliprgn, (RECT*)text->GetTextRect());
}

int METextLayer::rect_in_result_region(METextFeature* text)
{
    return  RectInRegion(&display_cliprgn, (RECT*)text->GetTextRect());
}

void METextLayer::SetTextWinrect(int width, int height)
{
    winrect_height = height;
    winrect_width = width;
}

