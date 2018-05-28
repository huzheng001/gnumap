/**
 * $Id: meagle.h,v 1.41 2006-10-31 01:53:22 jpzhang Exp $ 
 * This file includes global and miscellaneous interfaces of mEagle.
 *
 \verbatim

    Copyright (C) 2002-2006 Feynman Software.

    All rights reserved by Feynman Software.

    This file is part of mEagle, a geographical information system 
    for real-time embedded systems base on MiniGUI.

 \endverbatim
 */


#ifndef __ME_EAGLE_H__
#define __ME_EAGLE_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef struct _DoublePoint
{
    double x;
    double y;
} DOUBLE_POINT;

typedef struct _KEYWORD_SEARCH
{
    double latitude;
    double longitude;
    char* feature_name;
}KEYWORD_SEARCH;

typedef struct _AREA_SEARCH
{
    double latitude;
    double longitude;
    int distance;
}AREA_SEARCH;

/* meagle message define */
enum
{
    ME_END_KEYSEARCH = MSG_USER + 200,
    ME_LOAD_MEAGLE_MAP,
    ME_SAVE_MEAGLE_MAP,
    ME_OPEN_MAP,
    ME_GET_MAPINFO,
    ME_LOAD_MAP,
    ME_CLOSE_MAP,
    ME_CHANGE_TOOL,
    ME_GET_LAYERCOUNT,
    ME_GET_LAYERNAME,
    ME_GET_LAYERVISIBLE,
    ME_SET_LAYERFONT,
    
    ME_CHANGE_LAYERINDEX,
    ME_CHANGE_LAYERVISIBLE,
    ME_ADD_EYE,
    ME_REMOVE_EYE,
    ME_UPDATE_EYE,

    ME_WIN_TO_LL,
    ME_LL_TO_WIN,

    ME_REFRESH,
    ME_GLOBAL,
    ME_CLEAR,
    ME_NORTH,
    ME_SET_SCALE,
    ME_GET_SCALE,
    ME_MOVE_X,
    ME_MOVE_Y,
    ME_ROTATE,
    ME_KEY_SEARCH,
    ME_AREA_SEARCH,
    ME_GET_KEYRESULTCOUNT,
    ME_GET_KEYRESULTITEM,
    ME_GET_AREARESULTCOUNT,
    ME_GET_AREARESULTITEM,
    ME_ADD_RESULT,
    ME_REMOVE_RESULT,
    ME_SETCENTER_LAT,
};

enum
{
    ME_STATUS_DRAG = 100,
    ME_STATUS_ZOOMOUT,
    ME_STATUS_ZOOMIN,
    ME_STATUS_ROTATE,    
    ME_STATUS_DISTANCE,
    ME_COORDINATE_WIN,
    ME_COORDINATE_LAT,
};



/**
  * Defined some geometry types for STRUCT RESULT .
  */
enum
{
    /**
      * A circle .
      */
    CIRCLE = 0,
    /**
      * A filled circle .
      */
    CIRCLEFILLED,
    /**
      * A triangle .
      */
    TRIANGLE,
    /**
      * A filled triangle .
      */
    TRIANGLEFILLED,
    /**
      * A square .
      */
    SQUARE,
    /**
      * A filled square .
      */
    SQUAREFILLED,
    /**
      * A picture .
      */
    PICTURE,
    /**
      * A line or more lines .
      */
    LINES,
    /**
      * A polygon .
      */
    POLYGON,
    /**
      * A big circle .
      */
    BIGCIRCLE
};

/**
  * Struct RESULT
  */
typedef struct _RESULT
{
    /**
      * The feature id .
      */
    long id;
    /**
      * The feature name .
      */
    const char* feature_name;
/**
  * \param geom_type  The type of the result which will display on the map .
  *     Can be one of the following values:
  *       - CIRCLE
  * It will be a circle;
  *       - CIRCLEFILLED
  * It will be a filled circle;
  *       - TRIANGLE
  * It will be a triangle;
  *       - TRIANGLEFILLED
  * It will be a filled triangle;
  *       - SQUARE
  * It will be a square;
  *       - SQUAREFILLED
  * It will be a filled square;
  *       - PICTURE
  * It will display a picture
  *       - LINES
  * It will be lines;
  *       - POLYGON
  * It will be polygon;
  *       - BIGCIRCLE
  * It will be big circle which the radius you can set;

  */
    int geom_type;
    /**
      * Union GEOMDATA
      */
    union _GEOMDATA
    {
        /**
          * Struct CIRCLE
          */
        struct _CIRCLE
        {
            /**
              * The size of the circle .
              */
            int size;
            /**
              * The pointer of the circle's color .
              */
            char* color;
            int isll;
            union {
                struct {
                    double latitude;
                    double longitude;
                } ll;
                struct {
                    int x;
                    int y;
                } win;
            };
        }CIRCLE;
        /**
          * Struct PICTURE
          */
        struct _PICTURE
        {
            /**
              * The width and height of the picture .
              */
            int width;
            int height;
            /**
              * The pointer to the picture filename .
              */
            PBITMAP pBitmap;
            int isll;
            union{
                struct{
                    /**
                     * The latitude of the picture feature painted on the map .
                     */
                    double latitude;
                    /**
                     * The longitude of the picture feature painted on the map .
                     */
                    double longitude;
                }ll;
                struct {
                    int x;
                    int y;
                }win;
            };
        }PICTURE;
        /**
          * Struct LINES
          */
        struct _LINES
        {
            /**
              * The width of the lines .
              */
            int width;
            /**
              * The pointer to the color of the lines .
              */
            char* color;
            /**
              * The num of the points on the lines .
              */
            int num;
            /**
              * The pointer to the points .
              */
            DOUBLE_POINT* points;
        }LINES;
        /**
          * Struct BIGCIRCLE
          */
        struct _BIGCIRCLE
        {
            /**
              * The radius of the bigcircle .
              */
            double r;
            /**
              * The width of the pen to draw the big circle .
              */
            int pen_width;
            /**
              * The pointer to the color of the big circle .
              */
            char* color;
            /**
              * The latitude of the big circle painted on the map .
              */
            double latitude;
            /**
              * The longitude of the big circle painted on the map .
              */
            double longitude;
        }BIGCIRCLE;
    }GEOMDATA;
}RESULT;

/**
 * The class name of meagle view control
 */
#define MEAGLE_CTRL   ("meagle")

BOOL RegisterMEagleControl (void);

void UnregisterMEagleControl (void);


/**
 * The class name of meagle eye view control
 */
#define MEAGLEEYE_CTRL   ("meagleeye")

BOOL RegisterMEagleEyeControl (void);

void UnregisterMEagleEyeControl (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __ME_EAGLE_H__*/

