#include "dc.h"
#include "layer.h"
#include "feature.h"

METextFeature::METextFeature(MELayer * parent_layer, const char * text, const MEColor *fc, const MEColor *bc)
    :MEFeature(parent_layer),m_x(0),m_y(0),m_text(text),m_setfont(NULL),rect(NULL)
{
    m_fc = new MEColor((unsigned long)0x000000);
    m_bc = new MEColor(0x00,0x00,0x00,0x00);

    if (fc)
        *m_fc = *fc;

    if (bc)
        *m_bc = *bc;


    m_setfont = parent_layer->GetLayerFont(); // pointer to default font.

    rect = new MERect;
}

METextFeature::~METextFeature()
{
    delete rect;
    delete m_fc;
    delete m_bc;
}

void METextFeature::SetText(const char *text)
{
    m_text = text;
}

void METextFeature::SetTextPos(int x, int y)
{
    m_x = x;
    m_y = y;
    div_t num;
    int len = 1;
    if (m_text)
        len = strlen(m_text);
    num = div(len, 2);
    if (num.rem == 0)
        len = num.quot;
    else 
        len = num.quot +1;
    rect->left = m_x;
    rect->top = m_y;
    rect->right = m_x + len*m_setfont->GetFontSize();
    rect->bottom = m_y + m_setfont->GetFontSize();
}

bool METextFeature::PointInRect(int width, int height)
{
    const RECT win_rect = {0, 0, width, height};
    if (PtInRect(&win_rect, m_x, m_y))
        return true;
    else
        return false;
}

void METextFeature::Draw(MEDC *dc)
{
    const MEFont * old_font = dc->getFont();
    MEColor old_fc = dc->getTextColor();
    MEColor old_bc = dc->getBgColor();

    dc->setFont(m_setfont);
    dc->setBgColor(*m_bc);
    dc->setTextColor(*m_fc);
    
    dc->drawText(m_text, m_x, m_y);

    dc->setFont(old_font);
    dc->setBgColor(old_bc);
    dc->setTextColor(old_fc);
}

char* METextFeature::GetText(void) const
{
    return (char *)m_text;
}

MEColor *METextFeature::GetColor(void)const
{
    return m_fc;
}

void METextFeature::SetColor(const MEColor * color)
{
    delete m_fc;
    m_fc = new MEColor(*color);
}

MEColor *METextFeature::GetBgColor(void) const
{
    return m_bc;
}

void METextFeature::SetBgColor(const MEColor * color)
{
    delete m_bc;
    m_bc = new MEColor(*color);
}
