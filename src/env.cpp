#include "env.h"

struct MEEnv::Fraction MEEnv::sin_table[128] = {{0,1},{87,5000},{87,2500},{87,2500},{523,10000},{697,10000},{697,10000},{871,10000},{209,2000},{609,5000},{609,5000},{1391,10000},{391,2500},{391,2500},{217,1250},{477,2500},{477,2500},{2079,10000},{2249,10000},{2419,10000},{2419,10000},{647,2500},{689,2500},{689,2500},{2923,10000},{309,1000},{309,1000},{651,2000},{171,500},{3583,10000},{3583,10000},{1873,5000},{3907,10000},{3907,10000},{4067,10000},{2113,5000},{4383,10000},{4383,10000},{4539,10000},{2347,5000},{2347,5000},{303,625},{4999,10000},{4999,10000},{103,200},{5299,10000},{2723,5000},{2723,5000},{5591,10000},{1147,2000},{1147,2000},{5877,10000},{3009,5000},{3009,5000},{1539,2500},{6293,10000},{6427,10000},{6427,10000},{82,125},{6691,10000},{6691,10000},{6819,10000},{3473,5000},{7071,10000},{7071,10000},{7193,10000},{7313,10000},{7313,10000},{7431,10000},{7547,10000},{7547,10000},{383,500},{7771,10000},{197,250},{197,250},{3993,5000},{809,1000},{809,1000},{8191,10000},{829,1000},{829,1000},{4193,5000},{106,125},{8571,10000},{8571,10000},{433,500},{4373,5000},{4373,5000},{8829,10000},{891,1000},{891,1000},{8987,10000},{9063,10000},{1827,2000},{1827,2000},{1841,2000},{9271,10000},{9271,10000},{1867,2000},{2349,2500},{1891,2000},{1891,2000},{951,1000},{9563,10000},{9563,10000},{2403,2500},{9659,10000},{9659,10000},{4851,5000},{9743,10000},{9781,10000},{9781,10000},{1227,1250},{1231,1250},{1231,1250},{2469,2500},{4951,5000},{4951,5000},{397,400},{1989,2000},{9961,10000},{9961,10000},{399,400},{4993,5000},{4993,5000},{9993,10000},{4999,5000},{1,1}};

MEEnv::MEEnv(double ll_long, double ll_lat, double win_x0, double win_y0, double win_pixel_length)
    : latlong_spatial(NULL), ortho_spatial(NULL), 
        x0(win_x0), y0(win_y0),
        pixel_length(win_pixel_length),
        rotate_angle(-1),
        cs(NULL)
{

    pixel_length_reciprocal = (pixel_length==0.0)?0:(1/pixel_length);
}

MEEnv::~MEEnv()
{
    if(ortho_spatial != NULL)
    {
        delete ortho_spatial;
        ortho_spatial = NULL;
    }

    if(latlong_spatial != NULL)
    {
        latlong_spatial->Release();
        latlong_spatial = NULL;
    }

    if (cs != NULL)
        delete cs;
}

void MEEnv::set_latlong_spatial(MESpatialReference* spatial)
{
    if(spatial == NULL)
    {
        if(latlong_spatial != NULL)
            delete latlong_spatial;
        latlong_spatial = NULL;
        return;
    }
    if(latlong_spatial != NULL)
    {
        if(spatial == latlong_spatial)
            return;
        delete latlong_spatial;
    }
    latlong_spatial = spatial->Clone();
    if(latlong_spatial != NULL && ortho_spatial != NULL)
    {
        if(cs != NULL)
            delete cs;
        cs = new MECS(this);
    }
}

void MEEnv::set_ortho_spatial(MESpatialReference* spatial)
{
    if(spatial == NULL)
    {
        if(ortho_spatial != NULL)
            delete ortho_spatial;
        ortho_spatial = NULL;
        return;
    }
    if(ortho_spatial != NULL)
    {
        if(spatial == ortho_spatial)
            return;
        delete ortho_spatial;
    }
    ortho_spatial = spatial->Clone();
    if(latlong_spatial != NULL && ortho_spatial != NULL)
    {
        if(cs != NULL)
            delete cs;
        cs = new MECS(this);
    }
}

void MEEnv::set_ortho_spatial(double ll_long, double ll_lat)
{
    if(latlong_spatial != NULL)
    {
        if(ortho_spatial != NULL)
            delete ortho_spatial;
        
        if (latlong_spatial->IsProjected())
            ortho_spatial = latlong_spatial->Clone();
        else if (latlong_spatial->IsGeographic())
        {
            ortho_spatial = latlong_spatial->Clone();
            ortho_spatial->SetGS(0, 0, 0);
        }
    }
    if(latlong_spatial != NULL && ortho_spatial != NULL)
    {
        if(cs != NULL)
            delete cs;
        cs = new MECS(this);
    }
}

MECS::MECS(MEEnv *env)
{
    m_env = env;
    trans_ll_ortho = MECreateCoordinateTransformation(m_env->get_latlong_spatial(), m_env->get_ortho_spatial());
    trans_ortho_ll = MECreateCoordinateTransformation(m_env->get_ortho_spatial(), m_env->get_latlong_spatial());
}

MECS::~MECS()
{
    delete trans_ll_ortho;
    delete trans_ortho_ll;
}

