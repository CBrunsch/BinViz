INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_BINVIZ binviz)

FIND_PATH(
    BINVIZ_INCLUDE_DIRS
    NAMES binviz/api.h
    HINTS $ENV{BINVIZ_DIR}/include
        ${PC_BINVIZ_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    BINVIZ_LIBRARIES
    NAMES gnuradio-binviz
    HINTS $ENV{BINVIZ_DIR}/lib
        ${PC_BINVIZ_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(BINVIZ DEFAULT_MSG BINVIZ_LIBRARIES BINVIZ_INCLUDE_DIRS)
MARK_AS_ADVANCED(BINVIZ_LIBRARIES BINVIZ_INCLUDE_DIRS)

