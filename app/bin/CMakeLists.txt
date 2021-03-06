ADD_EXECUTABLE(cnvdsgn cnvdsgn.c utility.c)
GET_TARGET_PROPERTY(cnvdsgn_EXE cnvdsgn LOCATION)
IF(NOT WIN32)
	TARGET_LINK_LIBRARIES(cnvdsgn m)
ENDIF(NOT WIN32)

MACRO(GENERATE_LIN lin_name)
	ADD_CUSTOM_COMMAND(
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${lin_name}.lin
		DEPENDS cnvdsgn ${CMAKE_CURRENT_SOURCE_DIR}/${lin_name}.src
		COMMAND ${cnvdsgn_EXE} < ${CMAKE_CURRENT_SOURCE_DIR}/${lin_name}.src > ${CMAKE_CURRENT_BINARY_DIR}/${lin_name}.lin
		)
ENDMACRO(GENERATE_LIN)

GENERATE_LIN(to3way)
GENERATE_LIN(tocrv)
GENERATE_LIN(tocrvsct)
GENERATE_LIN(todcross)
GENERATE_LIN(todslip)
GENERATE_LIN(tolcross)
GENERATE_LIN(torcross)
GENERATE_LIN(toreg)
GENERATE_LIN(tosslip)
GENERATE_LIN(tostrsct)
GENERATE_LIN(towye)
GENERATE_LIN(toxing)

SET(LIN_SOURCES
	${CMAKE_CURRENT_BINARY_DIR}/to3way.lin
	${CMAKE_CURRENT_BINARY_DIR}/tocrv.lin
	${CMAKE_CURRENT_BINARY_DIR}/tocrvsct.lin
	${CMAKE_CURRENT_BINARY_DIR}/todcross.lin
	${CMAKE_CURRENT_BINARY_DIR}/todslip.lin
	${CMAKE_CURRENT_BINARY_DIR}/tolcross.lin
	${CMAKE_CURRENT_BINARY_DIR}/torcross.lin
	${CMAKE_CURRENT_BINARY_DIR}/toreg.lin
	${CMAKE_CURRENT_BINARY_DIR}/tosslip.lin
	${CMAKE_CURRENT_BINARY_DIR}/tostrsct.lin
	${CMAKE_CURRENT_BINARY_DIR}/towye.lin
	${CMAKE_CURRENT_BINARY_DIR}/toxing.lin
	)

GET_TARGET_PROPERTY(genhelp_EXE genhelp LOCATION)

ADD_CUSTOM_COMMAND(
       OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/bllnhlp.c
       DEPENDS genhelp ${help_SOURCE_DIR}/genhelp.in
       COMMAND ${genhelp_EXE} ${GENHELP_OPTS} ${help_SOURCE_DIR}/genhelp.in ${CMAKE_CURRENT_BINARY_DIR}/bllnhlp.c
       )

SET(SOURCES
	${LIN_SOURCES}
	appdefaults.c
	bllnhlp.c
	cbezier.c
	cblock.c
	ccornu.c
	ccurve.c
	ccontrol.c
	cdraw.c
	celev.c
	cgroup.c
	chndldto.c
	chotbar.c
	cjoin.c
	cmisc.c
	cmodify.c
	cnote.c
	compound.c
	cparalle.c
	cprint.c
	cprofile.c
	cpull.c
	cruler.c
	cselect.c
	csensor.c
	csnap.c
	csplit.c
	csignal.c
	cstraigh.c
	cstruct.c
	cswitchmotor.c
	ctext.c
	ctodesgn.c
	ctrain.c
	cturnout.c
	cturntbl.c
	cundo.c
	custom.c
	dbench.c
	dbitmap.c
	dcar.c
	dcmpnd.c
	dcustmgm.c
	dcontmgm.c
	dease.c
	denum.c
	dlayer.c
	doption.c
	dpricels.c
	dprmfile.c
	draw.c
	drawgeom.c
	dxfformat.c
	dxfoutput.c
	elev.c
	fileio.c
	i18n.c
	layout.c
	lprintf.c
	macro.c
	misc2.c
	param.c
	paths.c
	shrtpath.c
	smalldlg.c
	tbezier.c
	tcornu.c
	tcurve.c
	tease.c
	track.c
	trkseg.c
	tstraigh.c
	utility.c
	)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${XTrkCAD_BINARY_DIR})
INCLUDE_DIRECTORIES(${help_BINARY_DIR})
INCLUDE_DIRECTORIES(${wlib_SOURCE_DIR}/include)

LINK_DIRECTORIES(${GTK_LIBRARY_DIRS})
LINK_DIRECTORIES(${GTK_WEBKIT_LIBRARY_DIRS})

ADD_LIBRARY(xtrkcad-lib ${SOURCES})

# This ensures that messages.h has been generated before we build xtrkcad-lib
ADD_DEPENDENCIES(xtrkcad-lib Help)

ADD_EXECUTABLE(xtrkcad WIN32
	misc.c
	xtrkcad.rc
	)
TARGET_LINK_LIBRARIES(xtrkcad xtrkcad-lib)
TARGET_LINK_LIBRARIES(xtrkcad xtrkcad-wlib)
TARGET_LINK_LIBRARIES(xtrkcad xtrkcad-cornu)
TARGET_LINK_LIBRARIES(xtrkcad dynstring)

ADD_EXECUTABLE(mkturnout
	${LIN_SOURCES}
	ctodesgn.c
	utility.c
	)
SET_TARGET_PROPERTIES(mkturnout PROPERTIES COMPILE_FLAGS -DMKTURNOUT)

IF(NOT WIN32)
	TARGET_LINK_LIBRARIES(mkturnout m)
	TARGET_LINK_LIBRARIES(xtrkcad m)
	
	# Link libintl for systems where it is a separate library
	find_library( INTL_LIBRARY intl )
	if(INTL_LIBRARY)
		target_link_libraries( mkturnout ${INTL_LIBRARY} )
	endif(INTL_LIBRARY)
ELSE(NOT WIN32)
	TARGET_LINK_LIBRARIES(mkturnout xtrkcad-wlib)
ENDIF(NOT WIN32)	

# for testing only, should be IF(APPLE) ...
IF(APPLE)
	ADD_EXECUTABLE( helphelper helphelper.c ) 
	FIND_LIBRARY(COREFOUNDATION_LIBRARY CoreFoundation)
	FIND_LIBRARY(CARBON_LIBRARY Carbon)	
	TARGET_lINK_LIBRARIES(helphelper ${COREFOUNDATION_LIBRARY} ${CARBON_LIBRARY})
	INSTALL(
		TARGETS helphelper
		RUNTIME DESTINATION ${XTRKCAD_BIN_INSTALL_DIR}
		)
ENDIF(APPLE)
	
INSTALL(
	TARGETS xtrkcad
	RUNTIME DESTINATION ${XTRKCAD_BIN_INSTALL_DIR}
	)
	
if(XTRKCAD_TESTING AND CMOCKA_FOUND)			
	add_subdirectory( unittest )
endif()  
