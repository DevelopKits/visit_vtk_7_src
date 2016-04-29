# Configure for Qt5..
IF(NOT DEFINED VISIT_QT_DIR)
    MESSAGE(FATAL_ERROR "Qt5 installation directory not specified")
ENDIF()

set(QT_MOC_EXECUTABLE ${VISIT_QT_DIR}/bin/moc)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(QT5_INCLUDE_DIRS "")
set(QT5_LIBRARIES "")

set(visit_qt_modules Core Gui Widgets OpenGL Network PrintSupport Xml UiTools)

if(LINUX)
    set (visit_qt_modules ${visit_qt_modules} X11Extras)
endif()

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${VISIT_QT_DIR}/lib/cmake)
find_package (Qt5 REQUIRED ${visit_qt_modules})


foreach(mod ${visit_qt_modules})
  string(TOUPPER ${mod} upper_mod)
  if(NOT VISIT_QT_SKIP_INSTALL)
    if(WIN32 AND EXISTS ${VISIT_QT_DIR}/lib/Qt5${mod}.lib)
      THIRD_PARTY_INSTALL_LIBRARY(${VISIT_QT_DIR}/lib/Qt5${mod}.lib)
    endif()
    # headers
    foreach(H ${Qt5${mod}_INCLUDE_DIRS})
      if(${H} MATCHES "/include/Qt")
        INSTALL(DIRECTORY ${H}
                DESTINATION ${VISIT_INSTALLED_VERSION_INCLUDE}/qt/include
                FILE_PERMISSIONS OWNER_WRITE OWNER_READ
                                   GROUP_WRITE GROUP_READ
                                   WORLD_READ
                DIRECTORY_PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE
                                        GROUP_WRITE GROUP_READ GROUP_EXECUTE
                                        WORLD_READ WORLD_EXECUTE
                PATTERN ".svn" EXCLUDE
        )
      endif()
    endforeach()
  endif()
endforeach()


set(QT_QTUITOOLS_LIBRARY ${Qt5UiTools_LIBRARIES})
set(QT_QTOPENGL_LIBRARY ${Qt5OpenGL_LIBRARIES})

# if/when we drop support for qt 4, perhaps leave these split and
# add Widgets or PrintSupport only where needed
set(QT_QTGUI_LIBRARY ${Qt5Gui_LIBRARIES} 
                     ${Qt5Widgets_LIBRARIES} 
                     ${Qt5PrintSupport_LIBRARIES})
set(QT_QTNETWORK_LIBRARY ${Qt5Network_LIBRARIES})
set(QT_QTXML_LIBRARY ${Qt5Xml_LIBRARIES})

# why is core not named the same as the others?
set(QT_CORE_LIBRARY ${Qt5Core_LIBRARIES})

if (LINUX)
    set(QT_QTX11EXTRAS_LIBRARY ${Qt5X11Extras_LIBRARIES})
endif()

if(NOT VISIT_QT_SKIP_INSTALL)
  # moc
  get_target_property(moc_location Qt5::moc LOCATION)
  MESSAGE(STATUS "moc location: ${moc_location}")
  install(PROGRAMS ${moc_location}
          DESTINATION ${VISIT_INSTALLED_VERSION_BIN}
          PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE
          GROUP_WRITE GROUP_READ GROUP_EXECUTE
          WORLD_READ WORLD_EXECUTE
  )

  set(qt_libs_install
        Qt5::Core
        Qt5::Gui
        Qt5::Network
        Qt5::OpenGL
        Qt5::PrintSupport
        Qt5::Widgets
        Qt5::Xml
  )
  if(LINUX)
      set(qt_libs_install ${qt_libs_install} Qt5::X11Extras)
  endif()

  foreach(qtlib ${qt_libs_install})
      get_target_property(qtlib_location ${qtlib} LOCATION)
      # On Linux, the library names are Qt5xxx.so.${QT_VERSION}
      # We need to remove the version part so that THIRD_PARTY_INSTALL_LIBRARY
      # will work correctly.
      if (LINUX)
          string(REPLACE ".${Qt5Core_VERSION}" ""
                 qtlib_location ${qtlib_location})
      endif()
      if (APPLE)
          string(CONCAT qtlib_location ${qtlib_location} ".la")                 
      endif()      
      THIRD_PARTY_INSTALL_LIBRARY(${qtlib_location})
  endforeach()


  # We need a qt.conf file telling qt where to find the plugins
  if(WIN32)
      file(WRITE ${VISIT_BINARY_DIR}/qt.conf "[Paths]\nPlugins=./qtplugins\n")
  else()
      file(WRITE ${VISIT_BINARY_DIR}/qt.conf "[Paths]\nPlugins=../lib/qtplugins\n")
  endif()

  install(FILES ${VISIT_BINARY_DIR}/qt.conf
          DESTINATION ${VISIT_INSTALLED_VERSION_BIN}
          PERMISSIONS OWNER_READ OWNER_WRITE
                      GROUP_READ GROUP_WRITE
                      WORLD_READ)

  # Platform plugins
  if (WIN32)
      install(DIRECTORY ${VISIT_QT_DIR}/plugins/platforms
              DESTINATION ${VISIT_INSTALLED_VERSION_BIN}/qtplugins)

      # We also need the platforms and the qt.conf in the build dir.
      file(COPY ${VISIT_QT_DIR}/plugins/platforms
           DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ThirdParty/qtplugins
           FILE_PERMISSIONS OWNER_READ OWNER_WRITE
                            GROUP_READ GROUP_WRITE
                            WORLD_READ
      )
      foreach(CFG ${CMAKE_CONFIGURATION_TYPES})
          file(WRITE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CFG}/qt.conf 
               "[Paths]\nPlugins=../ThirdParty/qtplugins\n")
      endforeach()
  else()
      install(DIRECTORY ${VISIT_QT_DIR}/plugins/platforms
              DESTINATION ${VISIT_INSTALLED_VERSION_LIB}/qtplugins)
  endif()

  if (LINUX)
      # Xcb related plugins
      install(DIRECTORY ${VISIT_QT_DIR}/plugins/xcbglintegrations
              DESTINATION ${VISIT_INSTALLED_VERSION_LIB}/qtplugins)

          # there is also a platform-plugin related library that
          # needs to be installed, but there doesn't seem to be
          # a way to find this via Qt's cmake mechanisms, hence this
          # hard-coded extra step
          THIRD_PARTY_INSTALL_LIBRARY(${VISIT_QT_DIR}/lib/libQt5XcbQpa.so)
  endif()
endif()
