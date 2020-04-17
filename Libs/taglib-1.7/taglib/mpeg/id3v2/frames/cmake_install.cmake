# Install script for directory: /Content/Code/CSL_Libs/taglib-1.7/taglib/mpeg/id3v2/frames

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/include/taglib/attachedpictureframe.h;/usr/local/include/taglib/commentsframe.h;/usr/local/include/taglib/generalencapsulatedobjectframe.h;/usr/local/include/taglib/popularimeterframe.h;/usr/local/include/taglib/privateframe.h;/usr/local/include/taglib/relativevolumeframe.h;/usr/local/include/taglib/textidentificationframe.h;/usr/local/include/taglib/uniquefileidentifierframe.h;/usr/local/include/taglib/unknownframe.h;/usr/local/include/taglib/unsynchronizedlyricsframe.h;/usr/local/include/taglib/urllinkframe.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/include/taglib" TYPE FILE FILES
    "/Content/Code/CSL_Libs/taglib-1.7/taglib/mpeg/id3v2/frames/attachedpictureframe.h"
    "/Content/Code/CSL_Libs/taglib-1.7/taglib/mpeg/id3v2/frames/commentsframe.h"
    "/Content/Code/CSL_Libs/taglib-1.7/taglib/mpeg/id3v2/frames/generalencapsulatedobjectframe.h"
    "/Content/Code/CSL_Libs/taglib-1.7/taglib/mpeg/id3v2/frames/popularimeterframe.h"
    "/Content/Code/CSL_Libs/taglib-1.7/taglib/mpeg/id3v2/frames/privateframe.h"
    "/Content/Code/CSL_Libs/taglib-1.7/taglib/mpeg/id3v2/frames/relativevolumeframe.h"
    "/Content/Code/CSL_Libs/taglib-1.7/taglib/mpeg/id3v2/frames/textidentificationframe.h"
    "/Content/Code/CSL_Libs/taglib-1.7/taglib/mpeg/id3v2/frames/uniquefileidentifierframe.h"
    "/Content/Code/CSL_Libs/taglib-1.7/taglib/mpeg/id3v2/frames/unknownframe.h"
    "/Content/Code/CSL_Libs/taglib-1.7/taglib/mpeg/id3v2/frames/unsynchronizedlyricsframe.h"
    "/Content/Code/CSL_Libs/taglib-1.7/taglib/mpeg/id3v2/frames/urllinkframe.h"
    )
endif()

