
INCLUDES = -I../../include -I../../ogr -I../../ogr/port -I../../ogr/ogrsf_frmts
noinst_PROGRAMS=style_test dc_test

style_test_SOURCES= style_test.cpp
style_test_LDADD= ../../src/libmeagle.la -lminigui -lmgext -lstdc++ -lm -lproj -lz $(LIBS)

dc_test_SOURCES = dc_test.cpp
dc_test_LDADD = ../../src/libmeagle.la -lminigui -lmgext -lstdc++ -lm -lproj -lz $(LIBS)
