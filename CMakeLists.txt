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

# Add libgit2 project
ADD_SUBDIRECTORY(vendor/libgit2)

# Load cmake modules
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

INCLUDE(FindMysql)
#INCLUDE(FindPostgres)
#INCLUDE(FindMongo)
#INCLUDE(FindRedis)
INCLUDE(FindNode)
INCLUDE(FindHadoop)

# Add libgit2 include
INCLUDE_DIRECTORIES("vendor/libgit2/include")

# postgresql
#FILE(GLOB SRC_PQ native/postgresql/*.c native/postgresql/*.h)
#INCLUDE_DIRECTORIES(${LIBPQ_INCLUDE_DIR})

#ADD_LIBRARY(git2pq ${SRC_PQ})
#ADD_DEPENDENCIES(git2pq git2)
#TARGET_LINK_LIBRARIES(git2pq ${LIBPQ_LIBRARY}/libpq)
#TARGET_LINK_LIBRARIES(git2pq git2)

# mysql
FILE(GLOB SRC_MYSQL native/mysql/*.c native/mysql/*.h)
INCLUDE_DIRECTORIES(${LIBMYSQL_INCLUDE_DIR})

ADD_LIBRARY(git2mysql ${SRC_MYSQL})
ADD_DEPENDENCIES(git2mysql git2)
IF (MSVC_IDE)
  TARGET_LINK_LIBRARIES(git2mysql ${LIBMYSQL_LIBRARY}/libmysql)
ELSE ()
  TARGET_LINK_LIBRARIES(git2mysql ${LIBMYSQL_LIBRARY}/libmysqlclient.a)
ENDIF ()
TARGET_LINK_LIBRARIES(git2mysql git2)

# mongodb
#FILE(GLOB SRC_MONGO native/mongodb/*.c native/mongodb/*.h)
#INCLUDE_DIRECTORIES(${LIBMONGO_INCLUDE_DIR})

#ADD_LIBRARY(git2mongodb ${SRC_MONGO})
#TARGET_LINK_LIBRARIES(git2mongodb ${LIBMONGO_LIBRARY}/mongoc-1.0)
#TARGET_LINK_LIBRARIES(git2mongodb ${LIBMONGO_LIBRARY}/bson-1.0)

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

# hadoop

INCLUDE_DIRECTORIES(${HADOOP_INCLUDE_DIR})
FILE(GLOB SRC_HADOOP native/hadoop/*.h native/hadoop/*.hh native/hadoop/*.c native/hadoop/*.cc)
ADD_LIBRARY(hadoop ${SRC_HADOOP})
TARGET_LINK_LIBRARIES(hadoop ${HADOOP_LIBRARY}/libhdfs)
IF (MSVC_IDE)
  TARGET_LINK_LIBRARIES(hadoop ${HADOOP_LIBRARY}/hadoop)
ENDIF ()

# node native addon

INCLUDE_DIRECTORIES(${NODE_GYP_DIR}/src)
INCLUDE_DIRECTORIES(${NODE_GYP_DIR}/deps/uv/include)
INCLUDE_DIRECTORIES(${NODE_GYP_DIR}/deps/v8/include)
INCLUDE_DIRECTORIES(node_modules/nan)
FILE(GLOB SRC_NODE native/addon/*.h native/addon/*.cc)

ADD_LIBRARY(addon ${SRC_NODE})
ADD_DEPENDENCIES(addon git2mysql)
ADD_DEPENDENCIES(addon hadoop)
TARGET_LINK_LIBRARIES(addon git2mysql)
TARGET_LINK_LIBRARIES(addon hadoop)
IF (MSVC_IDE)
  TARGET_LINK_LIBRARIES(addon ${NODE_GYP_DIR}/Release/iojs)
ELSE ()
  SET (CMAKE_SHARED_LINKER_FLAGS "-undefined dynamic_lookup")
ENDIF ()

SET_TARGET_PROPERTIES(addon PROPERTIES SUFFIX ".node")
SET_TARGET_PROPERTIES(addon PROPERTIES PREFIX "")
