PROJECT(cornu)

FILE(GLOB HEADERS *.h)

SET(SOURCES
	bezctx.c
	bezctx_xtrkcad.c
	spiroentrypoints.c
	spiro.c
	)
	
SET(HEADERS 
	spiro.h
	)


INCLUDE_DIRECTORIES(${XTrkCAD_BINARY_DIR})
INCLUDE_DIRECTORIES(${XTrkCAD_SOURCE_DIR})
INCLUDE_DIRECTORIES(${wlib_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${help_BINARY_DIR})

ADD_LIBRARY(xtrkcad-cornu ${HEADERS} ${SOURCES})





