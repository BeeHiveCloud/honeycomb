# CMake build script for honeycomb

PROJECT(honeycomb)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# set CMAKE_BUILD_TYPE
SET(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)

# override STATIC_CRT
SET(STATIC_CRT OFF CACHE BOOL "Link the dynamic CRT libraries" FORCE)

# Set build directory
SET(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Override libgit2 build options
SET(BUILD_CLAR	OFF CACHE BOOL "Do not build clar test")

# Add libgit2 project
ADD_SUBDIRECTORY(vendor/libgit2)

# Load cmake modules
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

INCLUDE(FindMysql)
#INCLUDE(FindPostgres)
#INCLUDE(FindMongo)
#INCLUDE(FindRedis)
INCLUDE(FindNode)

# Add libgit2 include
INCLUDE_DIRECTORIES("vendor/libgit2/include")

# postgresql
#FILE(GLOB SRC_PQ native/postgresql/*.c native/postgresql/*.h)
#INCLUDE_DIRECTORIES(${LIBPQ_INCLUDE_DIR})

#ADD_LIBRARY(git2pq ${SRC_PQ})
#ADD_DEPENDENCIES(git2pq git2)
#TARGET_LINK_LIBRARIES(git2pq ${LIBPQ_LIBRARY}/libpq)
#TARGET_LINK_LIBRARIES(git2pq git2)
#TARGET_OS_LIBRARIES(git2pq)

# mysql
FILE(GLOB SRC_MYSQL native/mysql/*.c native/mysql/*.h)
INCLUDE_DIRECTORIES(${LIBMYSQL_INCLUDE_DIR})

ADD_LIBRARY(git2mysql ${SRC_MYSQL})
ADD_DEPENDENCIES(git2mysql git2)
TARGET_LINK_LIBRARIES(git2mysql ${LIBMYSQL_LIBRARY}/libmysql)
TARGET_LINK_LIBRARIES(git2mysql git2)
TARGET_OS_LIBRARIES(git2mysql)

# mongodb
#FILE(GLOB SRC_MONGO native/mongodb/*.c native/mongodb/*.h)
#INCLUDE_DIRECTORIES(${LIBMONGO_INCLUDE_DIR})

#ADD_LIBRARY(git2mongodb ${SRC_MONGO})
#TARGET_LINK_LIBRARIES(git2mongodb ${LIBMONGO_LIBRARY}/mongoc-1.0)
#TARGET_LINK_LIBRARIES(git2mongodb ${LIBMONGO_LIBRARY}/bson-1.0)
#TARGET_OS_LIBRARIES(git2mongodb)

# redis
#FILE(GLOB SRC_REDIS native/redis/*.c native/redis/*.h)
#INCLUDE_DIRECTORIES(${LIBREDIS_INCLUDE_DIR})

#ADD_LIBRARY(git2redis ${SRC_REDIS})
#ADD_DEPENDENCIES(git2redis git2)
#TARGET_LINK_LIBRARIES(git2redis git2)
#TARGET_LINK_LIBRARIES(git2redis ${LIBREDIS_LIBRARY}/hiredis)
#IF (MSVC_IDE)
#    TARGET_LINK_LIBRARIES(git2redis ${LIBREDIS_LIBRARY}/Win32_Interop)
#ENDIF ()
#TARGET_OS_LIBRARIES(git2redis)

# node native addon

INCLUDE_DIRECTORIES(${LIBNODE_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${LIBUV_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${LIBV8_INCLUDE_DIR})
INCLUDE_DIRECTORIES(node_modules/nan)
FILE(GLOB SRC_NODE native/addon/*.h native/addon/*.cc)

ADD_LIBRARY(addon ${SRC_NODE})
TARGET_LINK_LIBRARIES(addon git2mysql)
#TARGET_LINK_LIBRARIES(addon git2pq)
TARGET_LINK_LIBRARIES(addon ${LIBNODE_LIBRARY}/node)
TARGET_OS_LIBRARIES(addon)

SET_TARGET_PROPERTIES(addon PROPERTIES SUFFIX ".node")
