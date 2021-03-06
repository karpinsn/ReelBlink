#	---------------------------------------------------------------------------------------
# 	2013 Sam Ennis
#	
#
#	ReelBlink Library for C++
#	---------------------------------------------------------------------------------------

set(NAME ReelBlink) 

#	---------------------------------------------------------------------------------------
#	List all of the sources to be compiled
#	---------------------------------------------------------------------------------------



set(SOURCES 
    IProjector.h
)

set(REELBLINK_MOCS
    IProjector.h)
#   ---------------------------------------------------------------------------
#   Configure Dependencies
#   ---------------------------------------------------------------------------

#   ----------------------------------------------
#   Allow the option to compile with LightCommander
#   ----------------------------------------------
option(REELBLINK_QTFULLSCREEN "Build with QT fullscreen" OFF)
if(REELBLINK_QTFULLSCREEN)
    SET(USE_QTFULLSCREEN ON)
    message(STATUS "Building with QTFullscreen Projector Functionality")
	
	LIST(APPEND REELBLINK_MOCS
		QTFullscreen.h)
	
	LIST(APPEND SOURCES
		QTFullscreen.h
		QTFullscreen.cpp
	)
	
else()
    SET(USE_QTFULLSCREEN OFF)
endif()


#   ----------------------------------------------
#   Allow the option to compile with LightCommander
#   ----------------------------------------------
option(REELBLINK_LIGHTCOMMANDER_PROJECTOR "Build with LightCommander Projector" OFF)
if(REELBLINK_LIGHTCOMMANDER_PROJECTOR)
    SET(USE_LIGHTCOMMANDER_PROJECTOR ON)
    message(STATUS "Building with LightCommander Projector Functionality")

    find_package(LightCommander REQUIRED)
    if(LIGHTCOMMANDER_FOUND)
        include_directories(${LIGHTCOMMANDER_INCLUDE_DIR})
        LIST(APPEND LIBS ${LIGHTCOMMANDER_LIBRARIES})

        SET(REELBLINK_INCLUDE_DIRS ${REELBLINK_INCLUDE_DIRS} ${LIGHTCOMMANDER_INCLUDE_DIR})
        SET(REELBLINK_LIBRARIES ${REELBLINK_LIBRARIES} ${LIGHTCOMMANDER_LIBRARIES})

        LIST(APPEND REELBLINK_MOCS
            LightCommanderProjector.h)

        LIST(APPEND SOURCES
            LightCommanderProjector.h
            LightCommanderProjector.cpp
        )

    endif(LIGHTCOMMANDER_FOUND)
else()
    SET(USE_LIGHTCOMMANDER_PROJECTOR OFF)
endif()


#   ----------------------------------------------
#   Allow the option to compile with LightCrafter
#   ----------------------------------------------

option(REELBLINK_LIGHTCRAFTER_PROJECTOR "Build with LightCrafter Projector" OFF)
if(REELBLINK_LIGHTCRAFTER_PROJECTOR)
    SET(USE_LIGHTCRAFTER_PROJECTOR ON)
    message(STATUS "Building with LightCrafter Projector Functionality")
    
	LIST(APPEND SOURCES
		LCR_Common.h
		LightCrafter.cpp
		LightCrafter.h
		Tcp.cpp
		Tcp.h)

    if(WIN32)
        LIST(APPEND REELBLINK_LIBRARIES ws2_32)
        #target_link_libraries(MyExe wsock32 ws2_32)
    else()
        message(ERROR "Lightcrafter not currently supported on this system")
    endif()
    
	
else()
    SET(USE_LIGHTCRAFTER_PROJECTOR OFF)
endif()

#   ----------------------------------------------
#   Generate the config file for the code
#   ----------------------------------------------
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config-reelblink.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config-reelblink.h)
LIST(APPEND SOURCES config-reelblink.h)

#	----------------------------------------------
#	Group the source files
#	----------------------------------------------
source_group("ReelBlink" FILES ${SOURCES})
include_directories(..)

#	---------------------------------------------------------------------------------------
#	Generate Documentation
#	---------------------------------------------------------------------------------------
option(REELBLINK_GENERATE_DOC "Build Doxygen Documentation" OFF)
if(REELBLINK_GENERATE_DOC)
	add_subdirectory(../../doc ${PROJECT_BINARY_DIR}/doc)
endif(REELBLINK_GENERATE_DOC)

#   ---------------------------------------------------------------------------------------
#   Qt Generation
#   ---------------------------------------------------------------------------------------
qt4_wrap_cpp(GENERATED_SOURCES ${REELBLINK_MOCS})
source_group("Generated Sources - Do Not Edit" FILES ${GENERATED_SOURCES})

#	---------------------------------------------------------------------------------------
#	Build the library
#	---------------------------------------------------------------------------------------

set(REELBLINK_INCLUDE_DIRS ${REELBLINK_INCLUDE_DIRS} PARENT_SCOPE)
set(REELBLINK_LIBRARIES ${REELBLINK_LIBRARIES} PARENT_SCOPE)

add_library(${NAME} ${SOURCES} ${GENERATED_SOURCES} ${LENS_MOCS})

