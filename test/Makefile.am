SUBDIRS = res

INCLUDES = -I../include -I../ogr -I../ogr/port -I../ogr/ogrsf_frmts

noinst_PROGRAMS=meagle meagle_mob

meagle_SOURCES= meagle_app.c meagle_app.h \
				meagle_search.c meagle_search.h \
				meagle_layer.c meagle_clayer.c \
				meagle_state.c meagle_state.h \
                meagle_path.c meagle_path.h 

meagle_LDADD= ../src/libmeagle.la -lminigui_ths -lmgutils -ldl -lpng -ljpeg -lstdc++ -lm -lproj -lz $(LIBS)


meagle_mob_SOURCES= meagle_mob.c \
                meagle_search.c meagle_search.h \
                meagle_layer.c meagle_clayer.c \
                meagle_state.c meagle_state.h \
                meagle_path_mob.c meagle_path.h 

meagle_mob_LDADD= ../src/libmeagle.la -lminigui_ths -lmgutils -ldl -lpng -ljpeg -lm -lstdc++ -lproj -lz $(LIBS)

