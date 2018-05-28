INCLUDES = -I../include -I../ogr -I../ogr/port -I../ogr/ogrsf_frmts  

lib_LTLIBRARIES=libmeagle.la
libmeagle_la_SOURCES= ogr.h mstring.h\
    meagle.cpp resource.h\
    view.h view.cpp \
    view_state.cpp \
    map.h map.cpp \
    env.h env.cpp \
    layer.h layer.cpp maplayer.cpp textlayer.cpp resultlayer.cpp \
    searcher.h searcher.cpp \
    filter.h filter.cpp \
    utils.h simplify.h simplify.cpp symbolstipplebits.h \
    dc.h dc.cpp font.cpp\
	style.h style.cpp pool.cpp styletool.cpp\
	pen.cpp brush.cpp symbol.cpp label.cpp \
    key_searcher.cpp area_searcher.cpp \
	feature.h feature.cpp mapfeature.cpp textfeature.cpp \
	mapfield.h mapfield.cpp \
    resultfeature.h resultfeature.cpp \
    block.h block.cpp blockset.cpp blocklayer.cpp \
	geometry.h geometry.cpp point.cpp line.cpp polygon.cpp geommulit.cpp \
    meagleeye.cpp eyeview.h eyeview.cpp eye.h eye.cpp \
    spatialref.h spatialref.cpp spatialnode.cpp spatialct.cpp\
    spatialreference.cpp spatialproj4.cpp


libmeagle_la_LIBADD = ../ogr/libogr.la 
