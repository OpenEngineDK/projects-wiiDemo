# - Try to find libogc
# Once done this will define
#
#  OGC_FOUND - system has libogc
#  -- not used right now -- OGC_INCLUDE_DIR - the libogc include directory
#  OGC_LIBRARIES - Link these to use libogc
#

FIND_LIBRARY(LIBOGC
  NAMES 
  ogc
  PATHS
  /libogc/lib/wii
  ${PROJECT_SOURCE_DIR}/libraries/libogc/lib/wii
  ENV LD_LIBRARY_PATH
  ENV LIBRARY_PATH
  /usr/lib
  /usr/local/lib
  /opt/local/lib
  NO_DEFAULT_PATH
)

SET (OGC_LIBRARIES
  ${LIBOGC} 
)

IF(OGC_LIBRARIES)
   SET(OGC_FOUND TRUE)
   MARK_AS_ADVANCED(OGC_LIBRARIES)
ENDIF(OGC_LIBRARIES)



