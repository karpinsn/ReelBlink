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
	LightCommanderProjector.h
LightCommanderProjector.cpp
)


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

#	---------------------------------------------------------------------------------------
#	Build the library
#	---------------------------------------------------------------------------------------

set(REELBLINK_INCLUDE_DIRS ${REELBLINK_INCLUDE_DIRS} PARENT_SCOPE)
set(REELBLINK_LIBRARIES ${REELBLINK_LIBRARIES} PARENT_SCOPE)

add_library(${NAME} ${SOURCES})

