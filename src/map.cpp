/*
** $Id: map.cpp,v 1.138 2008-01-09 08:50:26 jpzhang Exp $
**
** Map class implement
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Zhang Ji Peng.
**
** Create date: 2006/08/09
*/


#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "view.h"
#include "feature.h"
#include "map.h"
#include "layer.h"
#include "eye.h"
#include "style.h"
#include "searcher.h"
#include "resultfeature.h"
#include "geometry.h"

#define MIN_PIXEL_LEN 64

MEMap::MEMap(HDC hdc, MEView * view, int width, int height, DWORD flags)
    :poDS(NULL),m_view(view),map_load(false),draw_age(0),win_width(width), win_height(height),mem_px_len_global(1.0),ortho_extent(0,0,0,0),mem_px_len_min(1.0),textlayer(NULL),blockset(NULL)
{
    RegisterOGRTAB();//mapinfo data driver
    RegisterOGRShape();//arcinfo data driver

    dc = new MEDC(hdc);
    env = new MEEnv(0, 0, 0, 0, 1);

    if (flags & DRAW_TEXT)
        textlayer = new METextLayer(win_width, win_height, this);

    if (flags & DRAW_RESULT)
        resultlayer = new MEResultLayer(this);
}

MEMap::~MEMap()
{
    UnloadMap();

    if (textlayer)
        delete textlayer;

    if (resultlayer)
        delete resultlayer;

    delete env;
    delete dc;
}
bool MEMap::AddEagleEye(MEEye * eye)
{
    if (eye == NULL)
        return false;
    
    for (std::list<MEEye *>::iterator i = eyes.begin(); i !=eyes.end(); i++)
    {
        if((*i) == eye)
            return true;
    }

    eyes.push_back(eye);
    return true;
}

void MEMap::RemoveEagleEye(MEEye * eye)
{
    for (std::list<MEEye *>::iterator i = eyes.begin(); i !=eyes.end(); i++)
    {
        if((*i) == eye){
            eyes.erase(i);
            return;
        }
    }
}

void MEMap::UpdateEyes(bool redrawback)
{
    for (std::list<MEEye *>::iterator i = eyes.begin(); i !=eyes.end(); i++){
        if (redrawback)
            (*i)->SetDrawStatus(true);
        (*i)->UpdateView();
    }
}

void MEMap::UpdateScreenPoint()
{
    double x0, y0;
    x0 = env->get_x0();
    y0 = env->get_y0();
    double angle;
    angle = env->get_rotate_angle();

    env->set_xy(ortho_extent.min_x, ortho_extent.max_y);
    env->set_rotate_angle(0.0);

    vector<MEMapLayer *>::iterator i;
    for (i = tablayers.begin(); i != tablayers.end(); i++)
        (*i)->UpdateScreenPoint();

    env->set_xy(x0, y0);
    env->set_rotate_angle(angle);
}

void MEMap::SearchAllLayers(MESearcher* searcher)
{
    vector<MEMapLayer *>::iterator i;
    for (i = tablayers.begin(); i != tablayers.end(); i++)
        searcher->SearchLayer(*i);
}

void MEMap::SearchSepecialLayer(const char* layer_name, MESearcher* searcher)
{
    int layer_index = GetLayerIndex(layer_name);
    vector<MEMapLayer *>::iterator i;
    i = tablayers.begin() + layer_index;
    searcher->SearchLayer(*i);
}

int MEMap::GetLayerCount()
{
    return tablayers.size();
}

bool MEMap::OpenMap(const char * datasource)
{
    UnloadMap();

    poDS = OGRSFDriverRegistrar::Open(datasource, FALSE);
    return (poDS != NULL);
}

int MEMap::GetMapInfo(const char ***layers)
{
    if (poDS == NULL)
        return 0;
    
    *layers = (const char **)malloc((poDS->GetLayerCount()+1) * sizeof(char *));
    int layer_idx = 0;
    for( int iLayer = 0; iLayer < poDS->GetLayerCount(); iLayer++ )
    {
        OGRLayer *poLayer = poDS->GetLayer(iLayer);
        if ( poLayer == NULL )
            break;
         (*layers)[layer_idx] = poLayer->GetLayerDefn()->GetName();
        layer_idx++;
    }
    (*layers)[layer_idx] = NULL;

    return layer_idx;
}

bool MEMap::LoadMap(const char ** layers)
{
    if (poDS == NULL)
        return false;
    
    if (poDS->GetLayer(0)->GetSpatialRef() == NULL){
        printf("Can not get Spatial Reference from the map!\n");
        return false;
    }

    bool first = true;
    double min_x=0.0, max_x=0.0, min_y=0.0, max_y=0.0;
    OGREnvelope envelop;
    OGRLayer *poLayer;
    const char **layer;
    layer = layers;
    while (*layer) {
        poLayer = poDS->GetLayerByName(*layer);

        if (!poLayer) {
            printf("Can not get layer info from the map!\n");
            return false;
        }
        
        if (poLayer->GetExtent(&envelop) == OGRERR_NONE) {
            if (first) {
                min_x = envelop.MinX;
                max_x = envelop.MaxX;
                min_y = envelop.MinY;
                max_y = envelop.MaxY;
                first = false;
            } else {
                if (envelop.MinX < min_x)
                    min_x = envelop.MinX;
                if (envelop.MaxX > max_x)
                    max_x = envelop.MaxX;
                if (envelop.MinY < min_y)
                    min_y = envelop.MinY;
                if (envelop.MaxY > max_y)
                    max_y = envelop.MaxY;
            }
        }
        *layer++;
    }
    
    MESpatialReference *ref = new MESpatialReference(poDS->GetLayer(0)->GetSpatialRef());
    env->set_latlong_spatial(ref);
    delete ref;

    env->set_ortho_spatial((max_x+min_x)/2, (max_y + min_y)/2);
    env->set_rotate_point(win_width/2, win_height/2);
    env->set_rotate_angle(0.0);

    env->get_cs()->latlong_to_ortho(min_x, min_y,
                                    ortho_extent.min_x, ortho_extent.min_y);
    env->get_cs()->latlong_to_ortho(max_x, max_y,
                                    ortho_extent.max_x, ortho_extent.max_y);

    mem_px_len_global = MAX(((ortho_extent.max_x-ortho_extent.min_x)/GetWidth()),
                                 ((ortho_extent.max_y-ortho_extent.min_y)/GetHeight()) );
    mem_px_len_min = mem_px_len_global/MIN_PIXEL_LEN;

    env->set_pixel_length(mem_px_len_global);
    env->set_xy(ortho_extent.min_x, ortho_extent.max_y);
    env->set_screen_xy(0, 0);

    layer = layers;
    int id = 1;
    while (*layer) {
        poLayer = poDS->GetLayerByName(*layer);
        MEMapLayer *maplayer;
        maplayer = new MEMapLayer(this, poLayer, id);
        tablayers.push_back(maplayer);

        *layer++;
        id++;
    }
    UpdateScreenPoint();

    int feature_count = 0;
    for (vector<MEMapLayer *>::iterator i = tablayers.begin(); i != tablayers.end(); ++i)
        feature_count+= (*i)->GetFeatureCount();

    blockset = new MEFeatureBlockSet(ortho_extent.min_x, ortho_extent.max_x,
                                ortho_extent.min_y, ortho_extent.max_y, feature_count, this);
    for (vector<MEMapLayer *>::iterator i = tablayers.begin(); i != tablayers.end(); ++i)
        blockset->AddLayer(*i);

    map_load = true;

    return true;
}

void MEMap::load_meg_layers_block(char *p, FILE *file)
{
    MESpatialReference *ref = new MESpatialReference(p);
    env->set_latlong_spatial(ref);
    delete ref;
    p = p + strlen(p) +1;
    ortho_extent.min_x = ArchSwapLE64(read_double(p));
    p += 8;
    ortho_extent.max_x = ArchSwapLE64(read_double(p));
    p += 8;
    ortho_extent.min_y = ArchSwapLE64(read_double(p));
    p += 8;
    ortho_extent.max_y = ArchSwapLE64(read_double(p));
    p += 8;
    env->set_ortho_spatial(0, 0);
    env->set_rotate_point(win_width/2, win_height/2);
    env->set_rotate_angle(0.0);
    mem_px_len_global = MAX(((ortho_extent.max_x-ortho_extent.min_x)/GetWidth()),
                            ((ortho_extent.max_y-ortho_extent.min_y)/GetHeight()) );
    mem_px_len_min = mem_px_len_global/MIN_PIXEL_LEN;

    env->set_pixel_length(mem_px_len_global);
    env->set_xy(ortho_extent.min_x, ortho_extent.max_y);
    env->set_screen_xy(0, 0);

    unsigned int StyleCount = ArchSwapLE32(read_uint(p));
    p += 4;
    std::vector<const char *> Styles;
    for (unsigned int i = 0; i < StyleCount; i++)
    {
        Styles.push_back(p);
        p = p + strlen(p) +1;
    }

    unsigned int LayerCount = ArchSwapLE32(read_uint(p));
    p += 4;
    char *layer_name;
    unsigned int offset, size;
    char *data;
    int id = 1;
    for (unsigned int i = 0; i < LayerCount; i++)
    {
        layer_name = p;
        p = p + strlen(p) +1;
        offset = ArchSwapLE32(read_uint(p));
        p += 4;
        size = ArchSwapLE32(read_uint(p));
        p += 4;
        data = (char *)malloc(size);
        fseek(file, offset, SEEK_SET);
        fread(data, 1, size, file);
        MEMapLayer *maplayer  = new MEMapLayer(this, data, layer_name, id, Styles);
        tablayers.push_back(maplayer);
        free(data);
        id++;
    }
    UpdateScreenPoint();
    int feature_count = 0;
    for (vector<MEMapLayer *>::iterator i = tablayers.begin(); i != tablayers.end(); ++i)
        feature_count+= (*i)->GetFeatureCount();
    blockset = new MEFeatureBlockSet(ortho_extent.min_x, ortho_extent.max_x,
                                            ortho_extent.min_y, ortho_extent.max_y, feature_count, this);
    for (vector<MEMapLayer *>::iterator i = tablayers.begin(); i != tablayers.end(); ++i)
        blockset->AddLayer(*i);
    map_load = true;
}

void MEMap::LoadMegMap(const char *filename)
{
    struct stat stats;
    if (stat ((char*)filename, &stats) == -1)
        return;
    FILE *file = fopen(filename, "rb");
    if (!file)
        return;
    char head[12];
    fread(head, 1, 12, file);
    if (memcmp(head, "MEAGLE 1.0.0", 12)!=0)
    {
        fclose(file);
        return;
    }
    unsigned int tmp;
    unsigned int FileSize;
    fread(&tmp, 1, 4, file);
    FileSize = ArchSwapLE32(tmp);
    if (FileSize != stats.st_size)
    {
        fclose(file);
        return;
    }

    fseek(file, -6, SEEK_END);
    char end[6];
    fread(end, 1, 6, file);
    if (memcmp(end, "MapEnd", 6)!=0)
    {
        fclose(file);
        return;
    }

    fseek(file, 16, SEEK_SET);
    unsigned int InfoBlockOffset = 0;
    unsigned int InfoBlockSize = 0;
    unsigned int LayersBlockOffset = 0;
    unsigned int LayersBlockSize = 0;
    char a;
    while (true)
    {
        fread(&a, 1, 1, file);
        if (a == '\0')
            break;
        if (a == 'i')
        {
            fread(&tmp, 1, 4, file);
            InfoBlockOffset = ArchSwapLE32(tmp);
            fread(&tmp, 1, 4, file);
            InfoBlockSize = ArchSwapLE32(tmp);
        }
        else if (a == 'l')
        {
            fread(&tmp, 1, 4, file);
            LayersBlockOffset = ArchSwapLE32(tmp);
            fread(&tmp, 1, 4, file);
            LayersBlockSize = ArchSwapLE32(tmp);
        }
        else
        {
            fseek(file, 8, SEEK_CUR);
        }
    }
    char *LayersBlock = (char *)malloc(LayersBlockSize);
    fseek(file, LayersBlockOffset, SEEK_SET);
    fread(LayersBlock, 1, LayersBlockSize, file);
    load_meg_layers_block(LayersBlock, file);
    free(LayersBlock);

    fclose(file);
}

void MEMap::UnloadMap(void)
{

    vector<MEMapLayer *>::iterator i;
    for (i = tablayers.begin(); i != tablayers.end(); i++)
        delete *i;

    tablayers.clear();

    if (blockset) {
        delete blockset;
        blockset = NULL;
    }

    map_load = false;

    UpdateEyes(true);
    m_view->Refresh();

    METoolPool::ClearAll(); // remove all the style in pool

    if (poDS) 
    {
        OGRDataSource::DestroyDataSource( poDS );
        poDS = NULL;
    }   
}

struct PosValue{
    unsigned int pos;
    unsigned int offset;
    unsigned int size;
};

void MEMap::write_geom(MEGeometry *geom, FILE *file)
{
    unsigned int tmp;
    if (geom == NULL)
    {
        tmp = ArchSwapLE32(GEO_NONE);
        fwrite(&tmp, 1, 4, file);
        return;
    }
    int type = geom->GetType();
    tmp = ArchSwapLE32(type);
    fwrite(&tmp, 1, 4, file);
    double lltmp;
    MEExtent *ext;
    ext = geom->GetExtent();
    lltmp = ArchSwapLE64(ext->min_x);
    fwrite(&lltmp, 1, 8, file);
    lltmp = ArchSwapLE64(ext->max_x);
    fwrite(&lltmp, 1, 8, file);
    lltmp = ArchSwapLE64(ext->min_y);
    fwrite(&lltmp, 1, 8, file);
    lltmp = ArchSwapLE64(ext->max_y);
    fwrite(&lltmp, 1, 8, file);
    ext = geom->GetOrthoExtent();
    lltmp = ArchSwapLE64(ext->min_x);
    fwrite(&lltmp, 1, 8, file);
    lltmp = ArchSwapLE64(ext->max_x);
    fwrite(&lltmp, 1, 8, file);
    lltmp = ArchSwapLE64(ext->min_y);
    fwrite(&lltmp, 1, 8, file);
    lltmp = ArchSwapLE64(ext->max_y);
    fwrite(&lltmp, 1, 8, file);
    int size;
    switch (type)
    {
        case GEO_POINT:
            lltmp = ArchSwapLE64(((MEGeomPoint *)geom)->ll_point.x);
            fwrite(&lltmp, 1, 8, file);
            lltmp = ArchSwapLE64(((MEGeomPoint *)geom)->ll_point.y);
            fwrite(&lltmp, 1, 8, file);
            lltmp = ArchSwapLE64(((MEGeomPoint *)geom)->ortho_point.x);
            fwrite(&lltmp, 1, 8, file);
            lltmp = ArchSwapLE64(((MEGeomPoint *)geom)->ortho_point.y);
            fwrite(&lltmp, 1, 8, file);
            break;
        case GEO_LINE:
        {
            size = ((MEGeomLine *)geom)->line_points.size();
            tmp = ArchSwapLE32(size);
            fwrite(&tmp, 1, 4, file);
            MEPointInfo *point;
            for (int i = 0; i< size; i++)
            {
                point = ((MEGeomLine *)geom)->line_points[i];
                lltmp = ArchSwapLE64(point->ll_point.x);
                fwrite(&lltmp, 1, 8, file);
                lltmp = ArchSwapLE64(point->ll_point.y);
                fwrite(&lltmp, 1, 8, file);
                lltmp = ArchSwapLE64(point->ortho_point.x);
                fwrite(&lltmp, 1, 8, file);
                lltmp = ArchSwapLE64(point->ortho_point.y);
                fwrite(&lltmp, 1, 8, file);
            }
            break;
        }
        case GEO_POLYGON:
        {
            size = ((MEGeomPolygon *)geom)->polygon_points.size();
            tmp = ArchSwapLE32(size);
            fwrite(&tmp, 1, 4, file);
            MEPointInfo *point;
            for (int i = 0; i< size; i++)
            {
                point = ((MEGeomPolygon *)geom)->polygon_points[i];
                lltmp = ArchSwapLE64(point->ll_point.x);
                fwrite(&lltmp, 1, 8, file);
                lltmp = ArchSwapLE64(point->ll_point.y);
                fwrite(&lltmp, 1, 8, file);
                lltmp = ArchSwapLE64(point->ortho_point.x);
                fwrite(&lltmp, 1, 8, file);
                lltmp = ArchSwapLE64(point->ortho_point.y);
                fwrite(&lltmp, 1, 8, file);
            }
            break;
        }
        case GEO_MULIT:
            size = ((MEGeomMulitiple*)geom)->geom_set.size();
            tmp = ArchSwapLE32(size);
            fwrite(&tmp, 1, 4, file);
            for (int i = 0; i< size; i++)
            {
                write_geom(((MEGeomMulitiple*)geom)->geom_set[i], file);
            }
            break;
    };
}

void MEMap::SaveMap(const char *filename)
{
    if (!map_load)
        return;
    FILE *file;

    file = fopen(filename, "wb");


    if (file)
    {
        unsigned int tmp;
        fwrite("MEAGLE 1.0.0", 1, 12, file);
        fwrite(&tmp, 1, 4, file);
        fwrite("i", 1, 1, file);
        int InfoBlockOffset_pos = ftell(file);
        fwrite(&tmp, 1, 4, file);
        fwrite(&tmp, 1, 4, file);
        fwrite("l", 1, 1, file);
        int LayersBlockOffset_pos = ftell(file);
        fwrite(&tmp, 1, 4, file);
        fwrite(&tmp, 1, 4, file);
        fwrite("", 1, 1, file);
        int InfoBlockOffset = ftell(file);
        std::string info;
        info = "charset=";
        info += GetSysCharset(FALSE);
        info += "\nwcharset=";
        info += GetSysCharset(TRUE);
        info += '\n';
        fwrite(info.c_str(), 1, info.length()+1, file);
        int InfoBlockSize = ftell(file) - InfoBlockOffset;
        int LayersBlockOffset = ftell(file);

        char *pszWKT = NULL;
        env->get_latlong_spatial()->exportToWkt( &pszWKT );
        fwrite(pszWKT, 1, strlen(pszWKT)+1, file);
        if (pszWKT != NULL)
            free (pszWKT);

        double lltmp;
        lltmp = ArchSwapLE64(ortho_extent.min_x);
        fwrite(&lltmp, 1, 8, file);
        lltmp = ArchSwapLE64(ortho_extent.max_x);
        fwrite(&lltmp, 1, 8, file);
        lltmp = ArchSwapLE64(ortho_extent.min_y);
        fwrite(&lltmp, 1, 8, file);
        lltmp = ArchSwapLE64(ortho_extent.max_y);
        fwrite(&lltmp, 1, 8, file);

        std::vector<const char *> Styles;
        std::vector<MEStyle *> StylePts;
        METoolPool::GetStyleInfo(Styles, StylePts);
        tmp = ArchSwapLE32(Styles.size());
        fwrite(&tmp, 1, 4, file);
        for (std::vector<const char *>::iterator i = Styles.begin(); i != Styles.end(); ++i)
        {
            fwrite(*i, 1, strlen(*i)+1, file);
        }

        tmp = ArchSwapLE32(tablayers.size());
        fwrite(&tmp, 1, 4, file);
        struct PosValue *pos_value;
        std::vector<struct PosValue *> pos_value_list;
        const char *str;
        for (unsigned int i = 0; i < tablayers.size(); ++i)
        {
            str = (tablayers[i])->GetName();
            fwrite(str, 1, strlen(str)+1, file);
            pos_value = new PosValue();
            pos_value->pos = ftell(file);
            pos_value_list.push_back(pos_value);
            fwrite(&tmp, 1, 4, file);
            fwrite(&tmp, 1, 4, file);
        }
        int LayersBlockSize = ftell(file) - LayersBlockOffset;
        for (unsigned int i = 0; i < tablayers.size(); ++i)
        {
            pos_value_list[i]->offset = ftell(file);
            MEMapLayer *layer = tablayers[i];
            MEFont *font = layer->GetLayerFont();
            std::string font_str;
            font_str = "FontName=";
            font_str += font->GetFontName();
            font_str += "\nCharSet=";
            font_str += font->GetCharSet();
            font_str += "\nFontSize=";
            char tmpstr[256];
            sprintf(tmpstr, "%d", font->GetFontSize());
            font_str += tmpstr;
            font_str += "\nRotatAngle=";
            sprintf(tmpstr, "%d", font->GetRotatAngle());
            font_str += tmpstr;
            font_str += '\n';
            fwrite(font_str.c_str(), 1, font_str.length()+1, file);
            tmp = ArchSwapLE32(layer->GetFeatureCount());
            fwrite(&tmp, 1, 4, file);
            layer->FeatureRewind();
            MEMapFeature *feature = layer->GetNextFeature();
            while (feature != NULL)
            {
                tmp = ArchSwapLE32(feature->GetFid());
                fwrite(&tmp, 1, 4, file);
                str = feature->GetName();
                if (str)
                    fwrite(str, 1, strlen(str)+1, file);
                else
                    fwrite("", 1, 1, file);
                MEStyle *style = feature->GetStyle();
                unsigned int j;
                for (j = 0; j < StylePts.size(); j++)
                {
                    if (style == StylePts[j])
                        break;
                }
                tmp = ArchSwapLE32(j);
                fwrite(&tmp, 1, 4, file);
                write_geom(feature->GetGeometry(), file);
                feature = layer->GetNextFeature();
            }
            pos_value_list[i]->size = ftell(file) - pos_value_list[i]->offset;
        }
        fwrite("MapEnd", 1, sizeof("MapEnd")-1, file);
        long FileSize = ftell(file);
        for (std::vector<struct PosValue *>::iterator i = pos_value_list.begin(); i != pos_value_list.end(); ++i)
        {
            fseek(file, (*i)->pos, SEEK_SET);
            tmp = ArchSwapLE32((*i)->offset);
            fwrite(&tmp, 1, 4, file);
            tmp = ArchSwapLE32((*i)->size);
            fwrite(&tmp, 1, 4, file);
            delete *i;
        }
        fseek(file, InfoBlockOffset_pos, SEEK_SET);
        tmp = ArchSwapLE32(InfoBlockOffset);
        fwrite(&tmp, 1, 4, file);
        tmp = ArchSwapLE32(InfoBlockSize);
        fwrite(&tmp, 1, 4, file);
        fseek(file, LayersBlockOffset_pos, SEEK_SET);
        tmp = ArchSwapLE32(LayersBlockOffset);
        fwrite(&tmp, 1, 4, file);
        tmp = ArchSwapLE32(LayersBlockSize);
        fwrite(&tmp, 1, 4, file);
        tmp = ArchSwapLE32(FileSize);
        fseek(file, 12, SEEK_SET);
        fwrite(&tmp, 1, 4, file);
        fclose(file);
    }
}

void MEMap::SetMapScale(double pixel_length)
{
    if (!map_load)
        return;


    /* limit zoom out*/
    if (pixel_length > mem_px_len_global)
        pixel_length = mem_px_len_global;

    /* limit zoom in*/
    if(pixel_length < mem_px_len_min)
        pixel_length = mem_px_len_min;

    //printf("pl=%f\n",pixel_length);

    env->set_pixel_length(pixel_length);
    UpdateScreenPoint();
}

double MEMap::GetMapScale(void)
{
    if (!map_load)
        return 1;
    
    return env->get_pixel_length();
}

void MEMap::DrawMap()
{
 
    dc->fillRect(0, 0, win_width, win_height);

    if (blockset) {
        blockset->Draw(dc, draw_age);
        draw_age++;
    }

    if (resultlayer)
        resultlayer->Draw(dc);

    if (textlayer)
        textlayer->Draw(dc);
   
    if (map_load) {
        std::list<MEEye *>::iterator i;
        for (i = eyes.begin(); i != eyes.end(); ++i){
            if ((*i)->GetDrawStatus()){
                DrawEye(*i);
                (*i)->SetDrawStatus(false);
            }
        }
    }
    
    if (textlayer){
        textlayer->Reset();
        textlayer->ResultReset();
    }

}
void MEMap::DrawEye(MEEye *eye)
{
    HDC hdc, eye_hdc;
    int eye_width, eye_height;
    double x0, y0, pixel_len, angle;
    double eye_x0, eye_y0, eye_pixel_len;
    
    x0 = env->get_x0();
    y0 = env->get_y0();
    int x0_s = env->get_screen_x0();
    int y0_s = env->get_screen_y0();
    pixel_len = env->get_pixel_length();
    angle = env->get_rotate_angle();
    hdc = dc->getDC();
    
    eye_width = eye->GetWidth();
    eye_height = eye->GetHeight();

    eye_hdc = eye->GetBackDC();
    dc->setDC(eye_hdc);
    dc->setMiniDC(true);
    dc->fillRect(0, 0, eye_width, eye_height);
    
    eye->GetEyeOrigin(&eye_x0, &eye_y0);
    eye_pixel_len = eye->GetEyePixelLength();
    
    env->set_pixel_length(eye_pixel_len);
    UpdateScreenPoint();
    env->set_xy(ortho_extent.min_x, ortho_extent.max_y);
    int x, y;
    env->get_cs()->ortho_to_window_without_rotate(eye_x0, eye_y0, x, y);
    env->set_screen_xy(x, y);
    env->set_xy(eye_x0, eye_y0);
    
    env->set_rotate_angle(0.0);
    if (blockset) {
        blockset->Draw(dc, draw_age);
        draw_age++;
    }
   
    dc->setDC(hdc);
    dc->setMiniDC(false);
    env->set_xy(x0, y0);
    env->set_screen_xy(x0_s, y0_s);
    env->set_pixel_length(pixel_len);
    UpdateScreenPoint();
    env->set_rotate_angle(angle);
}

int MEMap::AddResult(const RESULT *draw_key_result)
{
    if (!map_load)
        return -1;

    long id = draw_key_result->id;
    const char* name = draw_key_result->feature_name;
    double latitude = 0.0;
    double longitude = 0.0;
    int win_x = 0;
    int win_y = 0;
    int size = 0;
    int width = 0;
    int height = 0;
    MEResultFeature* feature = NULL;
    switch (draw_key_result->geom_type)
    {
        case CIRCLE:
        case CIRCLEFILLED:
        case TRIANGLE:
        case TRIANGLEFILLED:
        case SQUARE:
        case SQUAREFILLED:
            {
                if (draw_key_result->GEOMDATA.CIRCLE.isll)
                {
                    latitude = draw_key_result->GEOMDATA.CIRCLE.ll.latitude;
                    longitude = draw_key_result->GEOMDATA.CIRCLE.ll.longitude;
                }
                else
                {
                    win_x = draw_key_result->GEOMDATA.CIRCLE.win.x;
                    win_y = draw_key_result->GEOMDATA.CIRCLE.win.y;
                }
                size = draw_key_result->GEOMDATA.CIRCLE.size;
                const char *color = draw_key_result->GEOMDATA.CIRCLE.color;
                switch(draw_key_result->geom_type)
                {
                    case CIRCLE:
                        if (draw_key_result->GEOMDATA.CIRCLE.isll)
                            feature = new CircleFeature(resultlayer, latitude,longitude,size,color,id,name);
                        else
                            feature = new CircleFeature(resultlayer, win_x,win_y,size,color,id,name);

                        feature->SetFilled(false);
                        break;
                    case CIRCLEFILLED:
                        if (draw_key_result->GEOMDATA.CIRCLE.isll)
                            feature = new CircleFeature(resultlayer, latitude, longitude,size,color,id,name);
                        else
                            feature = new CircleFeature(resultlayer, win_x,win_y,size,color,id, name);
                        break;
                    case TRIANGLE:
                        feature = new TriangleFeature(resultlayer, latitude,longitude,size,color,id,name);

                        feature->SetFilled(false);
                        break;
                    case TRIANGLEFILLED:
                        feature = new TriangleFeature(resultlayer, latitude,longitude,size,color,id,name);
                        break;
                    case SQUARE:
                        feature = new SquareFeature(resultlayer, latitude,longitude,size,color,id,name);

                        feature->SetFilled(false);
                        break;
                    case SQUAREFILLED:
                        feature = new SquareFeature(resultlayer, latitude,longitude,size,color,id,name);
                        break;
                }
                break;
            }
        case PICTURE:
            width = draw_key_result->GEOMDATA.PICTURE.width;
            height = draw_key_result->GEOMDATA.PICTURE.height;

            if (draw_key_result->GEOMDATA.PICTURE.isll){
                latitude = draw_key_result->GEOMDATA.PICTURE.ll.latitude;
                longitude = draw_key_result->GEOMDATA.PICTURE.ll.longitude;
                feature = new BitmapFeature(resultlayer, latitude, longitude, size, width, height, id, 
                        draw_key_result->GEOMDATA.PICTURE.pBitmap, name);
            }else{
                win_x = draw_key_result->GEOMDATA.PICTURE.win.x;
                win_y = draw_key_result->GEOMDATA.PICTURE.win.y;
                feature = new BitmapFeature(resultlayer, win_x, win_y, size, width, height, id, 
                        draw_key_result->GEOMDATA.PICTURE.pBitmap, name);
            }

            break;
        case LINES:
        case POLYGON:
            {
                size = draw_key_result->GEOMDATA.LINES.width;
                const char *color = draw_key_result->GEOMDATA.LINES.color;
                if (draw_key_result->geom_type == LINES)
                    feature = new LinesFeature(resultlayer, color, id, name, size);
                else
                    feature = new PolygonFeature(resultlayer, color, id, name, size);

                for (MEDoublePoint* p = (MEDoublePoint*)draw_key_result->GEOMDATA.LINES.points;
                        p < (MEDoublePoint*)(draw_key_result->GEOMDATA.LINES.points + draw_key_result->GEOMDATA.LINES.num);
                        p++)
                {
                    ((LinesFeature*)feature)->AddPoint( p->x, p->y );
                }
                break;
            }
        case BIGCIRCLE:
            {
                double x = draw_key_result->GEOMDATA.BIGCIRCLE.latitude;
                double y = draw_key_result->GEOMDATA.BIGCIRCLE.longitude;
                double r = draw_key_result->GEOMDATA.BIGCIRCLE.r;
                int pen_width = draw_key_result->GEOMDATA.BIGCIRCLE.pen_width;
                const char *color = draw_key_result->GEOMDATA.BIGCIRCLE.color;
                feature = new BigCircleFeature(resultlayer, x, y, r, color, id, name, pen_width);
            }
            break;
        default:
            break;
    }
    return resultlayer->AddFeature(feature);
}

void MEMap::RemoveResult(int frid)
{
    resultlayer->RemoveFeature(frid);
}

void MEMap::ClearResultFeatures()
{
    resultlayer->ClearFeature();
}

void MEMap::ResetNorth(void)
{
    if (!map_load)
        return ;

    env->set_rotate_angle(0.0);
}

void MEMap::SetMapGlobal()
{
    if (!map_load)
        return ;

    if (env->get_pixel_length() != mem_px_len_global)
    {
        env->set_pixel_length(mem_px_len_global);
        UpdateScreenPoint();
    }
    env->set_xy(ortho_extent.min_x, ortho_extent.max_y);
    env->set_screen_xy(0, 0);
    env->set_rotate_angle(0.0);

}

double MEMap::GetDistance(int win_x1, int win_y1, int win_x2, int win_y2)
{
    if (!map_load)
        return 0.0;

    double start_x, start_y;
    double end_x, end_y;
    env->get_cs()->window_to_ortho(
		   	win_x1, win_y1, start_x, start_y);
    env->get_cs()->window_to_ortho(
		   	win_x2, win_y2, end_x, end_y);

	return sqrt((end_x-start_x)*(end_x-start_x)+(end_y-start_y)*(end_y-start_y));
}

void MEMap::MoveMapLat (double lat_x_offset, double lat_y_offset)
{
    if (!map_load)
        return;

    double old_x0, old_y0;
    double old_x, old_y;

    old_x0 = env->get_x0();
    old_y0 = env->get_y0();
    env->get_cs()->ortho_to_latlong(old_x0, old_y0, old_x, old_y);

    double new_x, new_y;
    double ortho_x0, ortho_y0;
    new_x = old_x+lat_x_offset;
    new_y = old_y+lat_y_offset;
    env->get_cs()->latlong_to_ortho(new_x, new_y, ortho_x0, ortho_y0);

    env->set_xy(ortho_extent.min_x, ortho_extent.max_y);
    int x, y;
    env->get_cs()->ortho_to_window_without_rotate(ortho_x0, ortho_y0, x, y);
    env->set_screen_xy(x, y);
    env->set_xy(ortho_x0, ortho_y0);
    UpdateEyes();
}

void MEMap::MoveMap (int win_x_offset, int win_y_offset)
{
    if (!map_load)
        return ;

    double old_x0, old_y0;
    int old_x, old_y;

    old_x0 = env->get_x0();
    old_y0 = env->get_y0();
    env->get_cs()->ortho_to_window(old_x0, old_y0, old_x, old_y);

    int new_x, new_y;
    double ortho_x0, ortho_y0;
    new_x = old_x+win_x_offset;
    new_y = old_y+win_y_offset;
    env->get_cs()->window_to_ortho(new_x, new_y, ortho_x0, ortho_y0);

    env->set_xy(ortho_extent.min_x, ortho_extent.max_y);
    int x, y;
    env->get_cs()->ortho_to_window_without_rotate(ortho_x0, ortho_y0, x, y);
    env->set_screen_xy(x, y);
    env->set_xy(ortho_x0, ortho_y0);
    UpdateEyes();
}

void MEMap::DrawTextLat (double lat_x, double lat_y, const char * sText,
                                const MEColor * text_color, const MEColor * bg_color) 
{
    if (!map_load)
        return ;

    int x , y;
    if (textlayer) {
        METextFeature *text = new METextFeature(textlayer, sText, text_color, bg_color);

        env->get_cs()->latlong_to_window(lat_x, lat_y, x , y);
        text->SetTextPos (x , y);

        textlayer->AddResultText(text);
    }
} 

void MEMap::SetRotateAngle (double angle)
{
    if (!map_load)
        return ;

    env->set_rotate_angle(angle);
}

double MEMap::GetRotateAngle (void)
{
    if (!map_load)
        return  0.0;

    return env->get_rotate_angle();
}

void MEMap::SetCenterOrtho (double ortho_center_x, double ortho_center_y)
{
    if (!map_load)
        return ;

    double new_ortho_x0, new_ortho_y0;

    env->set_xy(ortho_center_x, ortho_center_y);
    env->get_cs()->window_to_ortho_without_rotate(-win_width/2, -win_height/2, new_ortho_x0, new_ortho_y0);

    env->set_xy(ortho_extent.min_x, ortho_extent.max_y);
    int x, y;
    env->get_cs()->ortho_to_window_without_rotate(new_ortho_x0, new_ortho_y0, x, y);
    env->set_screen_xy(x, y);

    env->set_xy(new_ortho_x0, new_ortho_y0);
    UpdateEyes();
}

void MEMap::SetCenterLat (double lat_center_x, double lat_center_y)
{
    if (!map_load)
        return ;

    double ortho_x, ortho_y;
    env->get_cs()->latlong_to_ortho(lat_center_x, lat_center_y, ortho_x, ortho_y);
    SetCenterOrtho (ortho_x, ortho_y);
}

void MEMap::SetCenterWin (int win_center_x, int win_center_y)
{
    if (!map_load)
        return ;

    double ortho_x, ortho_y;
    env->get_cs()->window_to_ortho(win_center_x, win_center_y, ortho_x, ortho_y);
    SetCenterOrtho (ortho_x, ortho_y);
}

void MEMap::GetCenterOrtho (double *ortho_center_x, double *ortho_center_y)
{
    if (!map_load)
        return ;

    env->get_cs()->window_to_ortho(win_width/2, win_height/2, *ortho_center_x, *ortho_center_y);
}

void MEMap::GetCenterLat (double *lat_center_x, double *lat_center_y)
{
    if (!map_load)
        return ;

    env->get_cs()->window_to_latlong(win_width/2, win_height/2, *lat_center_x, *lat_center_y);
}

void MEMap::GetCenterWin (int *win_center_x, int *win_center_y)
{
    if (!map_load)
        return ;

    *win_center_x = win_width/2;
    *win_center_y = win_height/2;
}

 /*get layer name*/   
char* MEMap::GetLayerName (unsigned int index)
{   
    if (index >tablayers.size())
        return NULL;

    vector<MEMapLayer *>::iterator it;
    it = tablayers.begin()+index;
    return (*it)->GetName();
}
 /*get layer id*/   
int MEMap::GetLayerId (unsigned int index)
{   
    if (index >tablayers.size())
        return -1;

    vector<MEMapLayer *>::iterator it;
    it = tablayers.begin()+index;
    return (*it)->GetLayerId();
}

/*get layer index , 0 last*/
int MEMap::GetLayerIndex(const char* layer_name)
{
    vector<MEMapLayer *>::iterator it;
    int i=0;
    for(it= tablayers.begin(); it!=tablayers.end(); it++){
        if (strcmp((*it)->GetName(),layer_name) == 0)
            return i;
        i++;
    }
    return -1;
}

/*set index*/
void MEMap::SetLayerIndex(const char* layer_name, unsigned int index)
{
    vector<MEMapLayer *>::iterator it;
    for(it = tablayers.begin(); it != tablayers.end(); it++)
        if(strcmp((*it)->GetName(), layer_name) == 0) {
            MEMapLayer* maplayer = *it;
            tablayers.erase(it);
            tablayers.insert(tablayers.begin()+index, maplayer);
        }
}

/*set layer visible*/
void MEMap::SetLayerVisible (const char* layer_name, bool visible)
{
    vector<MEMapLayer *>::iterator it;
    for(it = tablayers.begin(); it != tablayers.end(); it++)
        if(strcmp((*it)->GetName(), layer_name) == 0) {
            MEMapLayer* maplayer = *it;
            maplayer->SetLayerVisible(visible);
        }
}

bool MEMap::GetLayerVisible (unsigned int index)
{
    vector<MEMapLayer *>::iterator it;
    it = tablayers.begin()+index;
    return (*it)->GetLayerVisible();
}

void MEMap::SetLayerVisible (unsigned int index, bool visible)
{
    vector<MEMapLayer *>::iterator it;
    it = tablayers.begin()+index;
    (*it)->SetLayerVisible(visible);
}

bool MEMap::SetLayerFont (const char * layer_name, MEFont * font)
{
    vector<MEMapLayer *>::iterator it;
    for(it = tablayers.begin(); it != tablayers.end(); it++)
        if(strcmp((*it)->GetName(), layer_name) == 0) {
            MELayer* layer = *it;
            layer->SetLayerFont(font);
	    return true;
        }
    return false;
}

MEFont * MEMap::GetLayerFont (const char * layer_name)
{
    vector<MEMapLayer *>::iterator it;
    for(it = tablayers.begin(); it != tablayers.end(); it++)
        if(strcmp((*it)->GetName(), layer_name) == 0) {
            MELayer* layer = *it;
            return layer->GetLayerFont();
        }
    return NULL;
}
