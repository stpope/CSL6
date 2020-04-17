# Install script for directory: /Content/Code/CSL_Libs/taglib-1.7/taglib

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
   "/usr/local/lib/libtag.1.7.0.dylib;/usr/local/lib/libtag.1.dylib;/usr/local/lib/libtag.dylib")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/lib" TYPE SHARED_LIBRARY FILES
    "/Content/Code/CSL_Libs/taglib-1.7/taglib/libtag.1.7.0.dylib"
    "/Content/Code/CSL_Libs/taglib-1.7/taglib/libtag.1.dylib"
    "/Content/Code/CSL_Libs/taglib-1.7/taglib/libtag.dylib"
    )
  foreach(file
      "$ENV{DESTDIR}/usr/local/lib/libtag.1.7.0.dylib"
      "$ENV{DESTDIR}/usr/local/lib/libtag.1.dylib"
      "$ENV{DESTDIR}/usr/local/lib/libtag.dylib"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      execute_process(COMMAND "/usr/bin/install_name_tool"
        -id "/usr/local/lib/libtag.1.dylib"
        "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/include/taglib/tag.h;/usr/local/include/taglib/fileref.h;/usr/local/include/taglib/audioproperties.h;/usr/local/include/taglib/taglib_export.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/include/taglib" TYPE FILE FILES
    "/Content/Code/CSL_Libs/taglib-1.7/taglib/tag.h"
    "/Content/Code/CSL_Libs/taglib-1.7/taglib/fileref.h"
    "/Content/Code/CSL_Libs/taglib-1.7/taglib/audioproperties.h"
    "/Content/Code/CSL_Libs/taglib-1.7/taglib/taglib_export.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Content/Code/CSL_Libs/taglib-1.7/taglib/toolkit/cmake_install.cmake")
  include("/Content/Code/CSL_Libs/taglib-1.7/taglib/asf/cmake_install.cmake")
  include("/Content/Code/CSL_Libs/taglib-1.7/taglib/mpeg/cmake_install.cmake")
  include("/Content/Code/CSL_Libs/taglib-1.7/taglib/ogg/cmake_install.cmake")
  include("/Content/Code/CSL_Libs/taglib-1.7/taglib/flac/cmake_install.cmake")
  include("/Content/Code/CSL_Libs/taglib-1.7/taglib/ape/cmake_install.cmake")
  include("/Content/Code/CSL_Libs/taglib-1.7/taglib/mpc/cmake_install.cmake")
  include("/Content/Code/CSL_Libs/taglib-1.7/taglib/mp4/cmake_install.cmake")
  include("/Content/Code/CSL_Libs/taglib-1.7/taglib/wavpack/cmake_install.cmake")
  include("/Content/Code/CSL_Libs/taglib-1.7/taglib/trueaudio/cmake_install.cmake")
  include("/Content/Code/CSL_Libs/taglib-1.7/taglib/riff/cmake_install.cmake")

endif()

