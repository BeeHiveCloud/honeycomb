#  NODE_GYP_DIR - where to find node.h, etc.
#  NODE_GYP_VER - version of node or iojs
#  NODE_GYP_FOUND       - True if node found.

#  Read environment variables to find mysql include/lib directories
SET(NODE_GYP_VER "0.12.7")
MESSAGE( STATUS "NODE_GYP_VER:         " ${NODE_GYP_VER} )
SET(NODE_GYP_DIR $ENV{NODE_GYP_DIR}/${NODE_GYP_VER})
MESSAGE( STATUS "NODE_GYP_DIR:         " ${NODE_GYP_DIR} )

FIND_PATH(NODE_GYP_DIR installVersion)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(NODE DEFAULT_MSG NODE_GYP_DIR)

MARK_AS_ADVANCED(NODE_GYP_DIR)
