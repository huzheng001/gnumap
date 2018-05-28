#ifndef _MEAGLE_APP_STATE_H_
#define _MEAGLE_APP_STATE_H_

extern int state;

extern void change_state(int new_state);

enum {
    STATE_PAN,
    STATE_ZOOMIN,
    STATE_ZOOMOUT,
    STATE_ROTATE,
    STATE_DISTANCE,
    STATE_MOVINGLOCALIZE,
    STATE_MOVINGREVERSE,
    STATE_PATHTRACING,
};

#endif
