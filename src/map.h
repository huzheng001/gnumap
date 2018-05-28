/*
** $Id: map.h,v 1.91 2007-07-03 10:42:47 jpzhang Exp $
**
** Map class define
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Zhang Ji Peng.
**
** Create date: 2006/08/09
*/


#ifndef _MEMAP_H_
#define _MEMAP_H_

#include <string>
#include <vector>
#include <list>
#include <map>

#include "block.h"
#include "env.h"
#include "meagle.h"
#include "ogr.h"

using std::vector;


typedef struct _FEATURE_STYLE
{
//perfect feature

}FEATURE_STYLE;


typedef struct _FEATURE
{
//perfect feature

}FEATURE;


typedef enum _RESULT_TYPE
{
//perfect feature

}RESULT_TYPE;

static inline double read_double (char * p)
{
#if defined (__arm__)
    double tmp = 0;
    char ctmp [9] = {0};
    memcpy (ctmp+4, p, 4);
    memcpy (ctmp, p+4, 4);
    memcpy (&tmp, ctmp, 8);
    return tmp;
#else
    double tmp = 0;
    memcpy (&tmp, p, 8);
    return tmp;
#endif
}
static inline unsigned int read_uint (char * p)
{
    unsigned int tmp = 0;
    memcpy (&tmp, p, 4);
    return tmp;
}

static inline int read_int (char * p)
{
    int tmp = 0;
    memcpy (&tmp, p, 4);
    return tmp;
}

const int DRAW_BACKGROUND = 1;
const int DRAW_TEXT = 2;
const int DRAW_RESULT = 4;



class MEMapLayer;
class METextLayer;
class MEResultLayer;
class MESearcher;
class MEView;
class MEGeometry;

class MEEye;

class MEMap 
{
public:
    MEMap(HDC dc, MEView * view, int width, int height, DWORD flags = DRAW_BACKGROUND | DRAW_RESULT | DRAW_TEXT);
    ~MEMap();

    void DrawMap(void);//draw map
    void DrawEye(MEEye *eye);//draweye 

    void SearchAllLayers(MESearcher* searcher);
    void SearchSepecialLayer(const char* layer_name, MESearcher* searcher);
    //map operation
    //measure
    double GetDistance(int win_x1, int win_y1, int win_x2, int win_y2);//measure distance
    //rotate map
    void SetRotateAngle (double angle);
    double GetRotateAngle (void);
    void ResetNorth(void); //set map north

    //text out
    void DrawTextLat (double lat_x, double lat_y, const char * sText, 
                                const MEColor * text_color = NULL, const MEColor * bg_color = NULL); 
    //zoom map
    void SetMapGlobal(void);//global map
    void SetMapScale(double pre_pixel_length);
    double GetMapScale(void);

    //move map
    void MoveMap (int win_x_offset, int win_y_offset);
    void MoveMapLat (double lat_x_offset, double lat_y_offset);

    //add/remove eagle eye
    bool AddEagleEye (MEEye * eye);
    void RemoveEagleEye (MEEye * eye);
    void UpdateEyes(bool redrawback=false);

    //layer operate
    int GetLayerCount(void);//get number of layers
    char* GetLayerName (unsigned int index);//get layer name
    int GetLayerId (unsigned int index);//get layer name
    int GetLayerIndex(const char* layer_name);//get layer index , 0 last
    void SetLayerIndex(const char* layer_name, unsigned int index);//set index
    void SetLayerVisible (const char* layer_name, bool visible);//set layer visible 
    void SetLayerVisible (unsigned int index, bool visible);//set layer visible 
    bool GetLayerVisible (unsigned int index);//set layer visible 
    bool SetLayerFont (const char * layer_name, MEFont * font);
    MEFont * GetLayerFont (const char * layer_name);

    //feature operate
    long GetFeatureByName(const char * name, long layer_handle=-1);
    bool SetFeatureStyle(long fid, const FEATURE_STYLE * new_style);
    bool GetFeatureStyle(long fid, FEATURE_STYLE * style);
    long AddFeature (const FEATURE * new_feature);
    void RemoveFeature(long fid);
    
    //result operate
    int AddResult(const RESULT *draw_key_result); 
    void RemoveResult (int frid);
    void ClearResultFeatures(void);

    //Load and unload map
    bool OpenMap(const char * datasource);
    int GetMapInfo(const char ***layers);
    bool LoadMap(const char ** layers);
    void UnloadMap(void);
    inline bool MapIsLoad(void) const;
    void SaveMap(const char *filename);
    void LoadMegMap(const char *filename);

    //attributes
    inline int GetWidth(void) const;
    inline int GetHeight(void) const;
    inline double GetGlobalPixelLength(void) const;
    inline double GetMinPixelLength(void) const;

    //set center
    void SetCenterOrtho (double ortho_center_x, double ortho_center_y);
    void SetCenterLat (double lat_center_x, double lat_center_y);
    void SetCenterWin (int win_center_x, int win_center_y);

    void GetCenterOrtho (double *ortho_center_x, double *ortho_center_y);
    void GetCenterLat (double *lat_center_x, double *lat_center_y);
    void GetCenterWin (int *win_center_x, int *win_center_y);
    //coordinate conversion
    inline void WindowToLatlong (int win_x, int win_y, double * lat_x, double * lat_y);
    inline void LatlongToWindow (double lat_x, double lat_y, int * win_x, int * win_y);
    inline void WindowToOrtho (int win_x, int win_y, double * ort_x, double * ort_y);
    inline void OrthoToWindow (double ort_x, double ort_y, int * win_x, int * win_y);
    inline void OrthoToWindowBatch (int num, const MEDoublePoint * in_array, MEPoint * out_array);
    inline void ScreenToWindow (int s_x, int s_y, int * win_x, int * win_y);
    inline void ScreenToWindowBatch (int num, const MEPoint * in_array, MEPoint * out_array);
    inline void LatlongToOrtho (double lat_x, double lat_y, double * ort_x, double * ort_y);
    inline void LatlongToOrthoBatch (int num, double *x, double *y);
    inline void OrthoToLatlong (double ort_x, double ort_y, double * lat_x, double * lat_y);
    
    MEEnv * GetEnv (void){return env;}//get env 
private:
    MEMap (const MEMap & o);
    MEMap & operator=(const MEMap & o);
    void UpdateScreenPoint();
    static void write_geom(MEGeometry *geom, FILE *file);
    void load_meg_layers_block(char *LayersBlock, FILE *file);
private:
    OGRDataSource *poDS;

    MEView * m_view;

    bool map_load;
    int draw_age;
    int win_width;
    int win_height;
    double mem_px_len_global;
    MEExtent ortho_extent;
    double mem_px_len_min;

    MEDC * dc;
    MEEnv * env;
    METextLayer* textlayer;
    MEResultLayer* resultlayer;
    MEFeatureBlockSet* blockset;

    vector<MEMapLayer *> tablayers;
    std::list<MEEye*> eyes;

    friend class MEMapFeature;
    friend class MEEye;
    friend class MEEyeView;
};

int MEMap::GetWidth(void) const
{ 
    return win_width; 
}

int MEMap::GetHeight(void) const
{ 
    return win_height;
}

double MEMap::GetGlobalPixelLength(void) const
{
    return mem_px_len_global;
}

double MEMap::GetMinPixelLength(void) const
{
    return mem_px_len_min;
}

bool MEMap::MapIsLoad(void) const
{
    return map_load;
}

void MEMap::WindowToLatlong (int win_x, int win_y, double *lat_x, double *lat_y)
{
    env->get_cs()->window_to_latlong(win_x, win_y, *lat_x, *lat_y);
}

void MEMap::LatlongToWindow (double lat_x, double lat_y, int * win_x, int * win_y)
{
    env->get_cs()->latlong_to_window(lat_x, lat_y, *win_x, *win_y);
}

void MEMap::WindowToOrtho (int win_x, int win_y, double * ort_x, double * ort_y)
{
    env->get_cs()->window_to_ortho(win_x, win_y, *ort_x, *ort_y);
}

void MEMap::OrthoToWindow (double ort_x, double ort_y, int * win_x, int * win_y)
{
    env->get_cs()->ortho_to_window(ort_x, ort_y, *win_x, *win_y);
}

void MEMap::LatlongToOrtho (double lat_x, double lat_y, double * ort_x, double * ort_y)
{
    env->get_cs()->latlong_to_ortho(lat_x, lat_y, *ort_x, *ort_y);
}

void MEMap::LatlongToOrthoBatch (int num, double *x, double *y)
{
    env->get_cs()->latlong_to_ortho(num, x, y);
}

void MEMap::OrthoToLatlong (double ort_x, double ort_y, double * lat_x, double * lat_y)
{
    env->get_cs()->ortho_to_latlong(ort_x, ort_y, *lat_x, *lat_y);
}

void MEMap::OrthoToWindowBatch (int num, const MEDoublePoint * in_array, MEPoint * out_array)
{
    env->get_cs()->ortho_to_window(num, in_array, out_array);
}

void MEMap::ScreenToWindow (int s_x, int s_y, int * win_x, int * win_y)
{
    env->get_cs()->screen_to_window(s_x, s_y, *win_x, *win_y);
}

void MEMap::ScreenToWindowBatch (int num, const MEPoint * in_array, MEPoint * out_array)
{
    env->get_cs()->screen_to_window(num, in_array, out_array);
}


#endif /* _MEMAP_H_ */
