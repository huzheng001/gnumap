SUBDIRS = ogrsf_frmts 

INCLUDES = -I./ -I../include -I./ogrsf_frmts -I./port

noinst_LTLIBRARIES = libogr.la

libogr_la_SOURCES= ogr_api.h ogr_core.h ogr_feature.h ogr_featurestyle.h ogr_geometry.h \
				  ogr_geos.h ogr_p.h ogr_spatialref.h ogr_srs_api.h \
				  swq.h swq.cpp gml2ogrgeometry.cpp ogr2gmlgeometry.cpp ogr_api.cpp \
				  ograssemblepolygon.cpp ogrct.cpp ogrcurve.cpp ogrfeature.cpp ogrfeaturedefn.cpp \
				  ogrfeaturequery.cpp ogrfeaturestyle.cpp ogrfielddefn.cpp ogr_fromepsg.cpp \
				  ogrgeometrycollection.cpp ogrgeometry.cpp ogrgeometryfactory.cpp \
				  ogr_geos_convert.cpp ogrlinearring.cpp ogrlinestring.cpp ogrmultilinestring.cpp \
                  ogrmultipoint.cpp ogrmultipolygon.cpp ogr_opt.cpp ogrpoint.cpp ogrpolygon.cpp \
				  ogrspatialreference.cpp ogr_srs_dict.cpp ogr_srs_esri.cpp ogr_srsnode.cpp \
				  ogr_srs_panorama.cpp ogr_srs_pci.cpp ogr_srs_proj4.cpp ogr_srs_usgs.cpp \
				  ogr_srs_validate.cpp ogr_srs_xml.cpp ogrsurface.cpp ogrutils.cpp \
				  \
				  port/cpl_config.h port/cpl_conv.h port/cpl_csv.h port/cpl_error.h port/cpl_list.h port/cpl_minixml.h \
				  port/cpl_multiproc.h port/cpl_port.h port/cpl_string.h port/cpl_vsi.h port/cpl_vsi_private.h \
				  port/cpl_conv.cpp port/cpl_csv.cpp port/cpl_dir.cpp port/cpl_error.cpp port/cpl_findfile.cpp \
				  port/cpl_getexecpath.cpp port/cplgetsymbol.cpp port/cpl_list.cpp port/cpl_minixml.cpp \
				  port/cpl_multiproc.cpp port/cpl_path.cpp \
				  port/cpl_string.cpp port/cplstring.cpp port/cpl_strtod.cpp port/cpl_vsil.cpp \
				  port/cpl_vsil_unix_stdio_64.cpp \
				  port/cpl_vsi_mem.cpp port/cpl_vsisimple.cpp port/xmlreformat.cpp

libogr_la_LIBADD = ogrsf_frmts/libsf.la 

