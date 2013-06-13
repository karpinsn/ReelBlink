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
		TiLightCrafter/BitmapCreator.cpp
		TiLightCrafter/BitmapCreator.h
		TiLightCrafter/LCR_Command_Packetizer.cpp
        TiLightCrafter/LCR_Command_Packetizer.h
		TiLightCrafter/LCR_Commander.cpp
		TiLightCrafter/LCR_Commander.h
		TiLightCrafter/LCR_Common.h
		TiLightCrafter/LightCrafter.cpp
		TiLightCrafter/LightCrafter.h
		TiLightCrafter/Tcp.cpp
		TiLightCrafter/Tcp.h)

    if(WIN32)
        LIST(APPEND REELBLINK_LIBRARIES ws2_32)
        #target_link_libraries(MyExe wsock32 ws2_32)
    else()
        message(ERROR "Lightcrafter not currently supported on this system")
    endif()
    
	
else()
    SET(USE_LIGHTCRAFTER_PROJECTOR OFF)
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

