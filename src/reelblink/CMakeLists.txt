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

