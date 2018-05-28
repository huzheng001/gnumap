/*
** $Id: symbolstipplebits.h,v 1.2 2006-08-30 09:48:47 jpzhang Exp $
**
** Stipple bitmap for MEDC
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Liu Peng.
**
** Create date: 2006/08/16
**
*/

#ifndef _STIPPLE_BITS_H_
#define _STIPPLE_BITS_H_


/*- 8*8 Cross -*/

static const unsigned char stipple_bits_cross8[] = "\x01\x01\x01\xFF\x01\x01\x01\x01";
static STIPPLE stipple_cross8 =
{
    8, 8, 1, 8,
    stipple_bits_cross8
};

/*- 16*16 Cross -*/
static const unsigned char stipple_bits_cross16[] = "\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\xFF\xFF\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80";
static STIPPLE stipple_cross16=
{
    16, 16, 2, 16,
    stipple_bits_cross16
};


/*- 8*8 Diagcross -*/

static const unsigned char stipple_bits_diagcross8[] = "\x81\x42\x24\x18\x18\x24\x42\x81";
static STIPPLE stipple_diagcross8 =
{
    8, 8, 1, 8,
    stipple_bits_diagcross8
};

/*- 16*16 Diagcross -*/
static const unsigned char stipple_bits_diagcross16[] = "\x80\x01\x40\x02\x20\x04\x10\x08\x08\x10\x04\x20\x02\x40\x01\x80\x01\x80\x02\x40\x04\x20\x08\x10\x10\x08\x20\x04\x40\x02\x80\x01";

static STIPPLE stipple_diagcross16 =
{
    16, 16, 2, 16,
     stipple_bits_diagcross16
};



/*- 8*8 Star Not filled -*/
static const unsigned char stipple_bits_star8[] = "\x10\x28\xEE\x44\x28\x38\x28\x44";
static STIPPLE stipple_star8 =
{
    8, 8, 1, 8,
    stipple_bits_star8
};

/*- 16*16 Star Not Filled-*/
static const unsigned char stipple_bits_star16[] = "\x01\x00\x01\x00\x01\x00\x02\x80\x02\x80\x04\x40\xFC\x7E\x60\x0C\x10\x10\x08\x20\x08\x20\x09\x20\x0A\xA0\x14\x50\x18\x30\x10\x10";
static STIPPLE stipple_star16=
{
    16, 16, 2, 16,
    stipple_bits_star16
};

/*- 8*8 Star Filled -*/
static const unsigned char stipple_bits_starfilled8[] = "\x10\x38\xFE\x7C\x38\x38\x28\x44";
static STIPPLE stipple_starfilled8 =
{
    8, 8, 1, 8,
    stipple_bits_starfilled8
};

/*- 16*16 Star Filled -*/
static const unsigned char stipple_bits_starfilled16[] = "\x01\x00\x01\x00\x01\x00\x03\x80\x03\x80\x07\xC0\xFF\xFE\x7F\xFC\x1F\xF0\x0F\xE0\x0F\xE0\x0F\xE0\x0E\xE0\x1C\x70\x18\x30\x10\x10";
static STIPPLE stipple_starfilled16=
{
    16, 16, 2, 16,
    stipple_bits_starfilled16
};

#endif   /*_STIPPLE_BITS_H_*/
