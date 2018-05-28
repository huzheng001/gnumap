/*
** $Id: mapfield.h,v 1.2 2006-09-08 08:29:53 jpzhang Exp $
**
** Classes define for Mapfeature field
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Zhang Ji Peng.
**
** Create date: 2006/08/09
*/

#ifndef _MAPFIELD_H_
#define _MAPFIELD_H_

#include "ogr.h"

typedef enum {
    MFT_Integer = 0,                 
    MFT_IntegerList = 1,        
    MFT_Real = 2,                        
    MFT_RealList = 3,                  
    MFT_String = 4,                       
    MFT_StringList = 5,       
    MFT_WideString = 6,     
    MFT_WideStringList = 7,                        
    MFT_Binary = 8,                                   
    MFT_Date = 9,                                   
    MFT_Time = 10,                          
    MFT_DateTime = 11,
} MEFieldType;


class MEMapField
{
public:
    MEMapField(MEFieldType type, const char * name, const void * data);
    ~MEMapField();

    MEFieldType GetType(void) const;
    char * GetName(void) const;

    static MEMapField * GetMapField(OGRFeature *ogrfeature, int field_index);

    int GetValueInteger(void);
    double GetValueReal(void);
    char * GetValueString(void);

private:
    MEMapField(const MEMapField & o);
    MEMapField & operator=(const MEMapField & o);
    
    MEFieldType m_type;
    char * m_name;

    union {
        int     Integer;
        double  Real;
        char *  String;
    }m_field;
};

#endif /* _MAPFIELD_H_ */
