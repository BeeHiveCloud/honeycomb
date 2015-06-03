# CMake build script for honeycomb

PROJECT(honeycomb)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# set CMAKE_BUILD_TYPE
SET(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)

# override STATIC_CRT
IF (MSVC_IDE)
  SET(STATIC_CRT OFF CACHE BOOL "Link the dynamic CRT libraries" FORCE)
ENDIF ()

# Set build directory
SET(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Override libgit2 build options
SET(BUILD_CLAR	OFF CACHE BOOL "Do not build clar test")
#SET(BUILD_SHARED_LIBS	OFF CACHE BOOL "build static library")

# Add libgit2 project
#ADD_SUBDIRECTORY(vendor/libgit2)

# Load cmake modules
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#INCLUDE(FindThrift)
INCLUDE(FindR)
#INCLUDE(FindMysql)
#INCLUDE(FindPostgres)
#INCLUDE(FindMongo)
#INCLUDE(FindRedis)
INCLUDE(FindNode)

# Find packages

#FIND_PACKAGE(Boost)

# Add libgit2 include
INCLUDE_DIRECTORIES("vendor/libgit2/include")

# postgresql
#FILE(GLOB SRC_PQ native/postgresql/*.c native/postgresql/*.h)
#INCLUDE_DIRECTORIES(${LIBPQ_INCLUDE_DIR})

#ADD_LIBRARY(postgresql SHARED ${SRC_PQ})
#ADD_DEPENDENCIES(postgresql git2)
#TARGET_LINK_LIBRARIES(postgresql ${LIBPQ_LIBRARY}/libpq)
#TARGET_LINK_LIBRARIES(postgresql git2)

# mysql
#FILE(GLOB SRC_MYSQL native/mysql/*.c native/mysql/*.h)
#INCLUDE_DIRECTORIES(${LIBMYSQL_INCLUDE_DIR})

#LINK_DIRECTORIES(${LIBMYSQL_LIBRARY})
#ADD_LIBRARY(mysql SHARED ${SRC_MYSQL})
#ADD_DEPENDENCIES(mysql git2)
#IF (MSVC_IDE)
#  TARGET_LINK_LIBRARIES(mysql libmysql)
#ELSE ()
#  TARGET_LINK_LIBRARIES(mysql mysqlclient_r)
#ENDIF ()
#TARGET_LINK_LIBRARIES(mysql git2)

# mongodb
#FILE(GLOB SRC_MONGO native/mongodb/*.c native/mongodb/*.h)
#INCLUDE_DIRECTORIES(${LIBMONGO_INCLUDE_DIR})

#ADD_LIBRARY(mongodb SHARED ${SRC_MONGO})
#TARGET_LINK_LIBRARIES(mongodb ${LIBMONGO_LIBRARY}/mongoc-1.0)
#TARGET_LINK_LIBRARIES(mongodb ${LIBMONGO_LIBRARY}/bson-1.0)

# redis
#FILE(GLOB SRC_REDIS native/redis/*.c native/redis/*.h)
#INCLUDE_DIRECTORIES(${LIBREDIS_INCLUDE_DIR})

#ADD_LIBRARY(redis SHARED ${SRC_REDIS})
#ADD_DEPENDENCIES(redis git2)
#TARGET_LINK_LIBRARIES(redis git2)
#TARGET_LINK_LIBRARIES(redis ${LIBREDIS_LIBRARY}/hiredis)
#IF (MSVC_IDE)
#    TARGET_LINK_LIBRARIES(redis ${LIBREDIS_LIBRARY}/Win32_Interop)
#ENDIF ()

# roger
INCLUDE_DIRECTORIES(${LIBR_INCLUDE})
FILE(GLOB SRC_ROGER native/R/*.h native/R/*.c)
LINK_DIRECTORIES(${LIBR_LIBRARY})
ADD_LIBRARY(roger SHARED ${SRC_ROGER})
TARGET_LINK_LIBRARIES(roger R)
TARGET_LINK_LIBRARIES(roger Rgraphapp)

# HBase
#INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
#INCLUDE_DIRECTORIES(${LIBTHRIFT_INCLUDE_DIR})
#FILE(GLOB SRC_HBASE native/hbase/*.h native/hbase/*.cpp)
#LINK_DIRECTORIES(${LIBTHRIFTL_LIBRARY})
#ADD_LIBRARY(hbase SHARED ${SRC_HBASE})
#TARGET_LINK_LIBRARIES(hbase libthrift)

# HIVE
#FILE(GLOB SRC_HIVE native/hive/*.h native/hive/*.cpp)
#ADD_LIBRARY(hive SHARED ${SRC_HIVE})
#TARGET_LINK_LIBRARIES(hive libthrift)

# node native addon
INCLUDE_DIRECTORIES(${NODE_GYP_DIR}/src)
INCLUDE_DIRECTORIES(${NODE_GYP_DIR}/deps/uv/include)
INCLUDE_DIRECTORIES(${NODE_GYP_DIR}/deps/v8/include)
INCLUDE_DIRECTORIES(node_modules/nan)
FILE(GLOB SRC_NODE native/addon/rconsole.h native/addon/rconsole.cc native/addon/addon.cc)

ADD_LIBRARY(addon SHARED ${SRC_NODE})
#ADD_DEPENDENCIES(addon mysql)
#TARGET_LINK_LIBRARIES(addon mysql)
#TARGET_LINK_LIBRARIES(addon hbase)
TARGET_LINK_LIBRARIES(addon roger)
IF (MSVC_IDE)
  TARGET_LINK_LIBRARIES(addon ${NODE_GYP_DIR}/Release/iojs)
ENDIF ()
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  SET (CMAKE_SHARED_LINKER_FLAGS "-undefined dynamic_lookup")
ENDIF()
#SET_TARGET_PROPERTIES(addon PROPERTIES CXX_STANDARD 11)
SET_TARGET_PROPERTIES(addon PROPERTIES SUFFIX ".node")
SET_TARGET_PROPERTIES(addon PROPERTIES PREFIX "")
