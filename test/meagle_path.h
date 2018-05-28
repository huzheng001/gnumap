#ifndef _MEAGLE_PATH_H_
#define _MEAGLE_PATH_H_


typedef struct _PathPoint
{
    DOUBLE_POINT dpoint;
    POINT point;
    int type;
    char* velocity;
    char * name;
} PATH_POINT;

enum
{
    PT_FROM=200,
    PT_END,
    PT_MONITOR,
    PT_GAS,
    PT_SPEED,
};

typedef struct _PathTrac
{
    PATH_POINT **points;
    DOUBLE_POINT cpoint;
    
    double pixel_len;
    int total_distance;
    int* len; /*length of each part*/
    int *part, *part_offset;
    double *off_lat, *off_lon;
    double* alpha;
    int perdistance;
}PATHTRAC;
#endif
