/*
** $Id: mapfield.cpp,v 1.4 2006-11-14 05:00:08 jpzhang Exp $
**
** Classes implement for Mapfeature field
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Zhang Ji Peng.
**
** Create date: 2006/08/09
*/

#include "dc.h"
#include "mapfield.h"
#include "mstring.h"

MEMapField * MEMapField::GetMapField(OGRFeature *ogrfeature, int field_index)
{
    OGRFieldDefn * fd = ogrfeature->GetFieldDefnRef(field_index);

    switch (fd->GetType())
    {
        case OFTInteger:
            {
                int i = ogrfeature->GetFieldIndex(fd->GetNameRef());
                return new MEMapField(MFT_Integer, fd->GetNameRef(), (void*)ogrfeature->GetFieldAsInteger(i));
            }
            break;
        case OFTIntegerList:
            break;
        case OFTReal:
            {
                int i = ogrfeature->GetFieldIndex(fd->GetNameRef());
                double val = ogrfeature->GetFieldAsDouble(i); 
                return new MEMapField(MFT_Real, fd->GetNameRef(), (void*)&val);
            }
            break;            
        case OFTRealList:
            break;            
        case OFTString:
            {
                int i = ogrfeature->GetFieldIndex(fd->GetNameRef());
                return new MEMapField(MFT_String, fd->GetNameRef(), (void*)ogrfeature->GetFieldAsString(i));
            }
            break;            
        case OFTStringList:
            break;       
        case OFTWideString:
            break;  
        case OFTWideStringList:
            break;            
        case OFTBinary:
            break;            
        case OFTDate:
            break;            
        case OFTTime:
            break;            
        case OFTDateTime:
            break;
    }

    return NULL;
}


MEMapField::MEMapField(MEFieldType type, const char * name, const void * data)
    :m_type(type)
{
    memset (&m_field, 0 , sizeof(m_field));

    m_name = strdup(name);

    switch (m_type) {
        case MFT_Integer:
            m_field.Integer = (int)data; 
            break;
        case MFT_Real:
            m_field.Real = *((double*)data);
            break;
        case MFT_String:
            m_field.String = strdup((char*)data); 
            break;
    }
}

MEMapField::~MEMapField()
{
    if (m_type == MFT_String)
        free (m_field.String);

    free (m_name);
}

MEFieldType MEMapField::GetType(void) const
{
    return m_type;
}

char * MEMapField::GetName(void) const
{
    return m_name;
}

int MEMapField::GetValueInteger(void)
{
    if (m_type != MFT_Integer)
        return 0;

    return m_field.Integer; 
}

double MEMapField::GetValueReal(void)
{
    if (m_type != MFT_Real)
        return 0.0;

    return m_field.Real;
}
char * MEMapField::GetValueString(void)
{
    if (m_type != MFT_String)
        return NULL;

    return m_field.String;
}
