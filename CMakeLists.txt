# Project name. Should be the same as the project directory name and
# will become the name of the binary target.
SET( PROJECT_NAME "wiiDemo")

# Project source code list
SET( PROJECT_SOURCES
  # Add all the cpp source files here
  main.cpp
)

# Include needed to use SDL under Mac OS X
# IF(APPLE)
#   SET(PROJECT_SOURCES ${PROJECT_SOURCES}  ${SDL_MAIN_FOR_MAC})
# ENDIF(APPLE)

# Project executable
ADD_EXECUTABLE(${PROJECT_NAME}
  ${PROJECT_SOURCES}
)


SET (PROJECT_LIBRARIES 
  # Core library dependencies
  OpenEngine_Core
  OpenEngine_Logging
  OpenEngine_Display
  OpenEngine_Devices
  OpenEngine_Scene
  # Extension dependencies
  Extensions_TGAResource
  Extensions_OBJResource
  Extensions_AccelerationStructures
)


IF(WII)
  INCLUDE(FindOgc.cmake)
  IF (OGC_FOUND)       
    #SET_TARGET_PROPERTIES (${PROJECT_NAME} PROPERTIES LINK_FLAGS "-logc")
    SET (PROJECT_LIBRARIES ${PROJECT_LIBRARIES} ${OGC_LIBRARIES}) 
  ELSE (OGC_FOUND)
    MESSAGE ("WARNING: Could not find libogc - depending targets will be disabled.")
  ENDIF (OGC_FOUND)
  MAKE_DOL(${PROJECT_NAME})
ENDIF(WII)

# Project dependencies
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
  ${PROJECT_LIBRARIES}
  )

