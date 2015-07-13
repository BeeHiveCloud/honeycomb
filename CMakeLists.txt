# CMake build script for honeycomb

PROJECT(honeycomb)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# set global CMAKE_BUILD_TYPE
SET(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)

# override STATIC_CRT
# TODO - this needs to be fixed
IF (MSVC_IDE)
  SET(STATIC_CRT OFF CACHE BOOL "Link the dynamic CRT libraries" FORCE)
ENDIF ()

# Set build directory
SET(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

IF (NOT MSVC_IDE)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
ENDIF ()

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  SET (CMAKE_SHARED_LINKER_FLAGS "-undefined dynamic_lookup")
ENDIF()

# Add libgit2 project
# Override libgit2 build options
SET(BUILD_CLAR	OFF CACHE BOOL "Do not build clar test")
SET(BUILD_SHARED_LIBS	OFF CACHE BOOL "build static library")
ADD_SUBDIRECTORY(vendor/libgit2)

# Load cmake modules
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#INCLUDE(FindThrift)
#INCLUDE(FindR)
INCLUDE(FindMysql)
#INCLUDE(FindPostgres)
#INCLUDE(FindMongo)
#INCLUDE(FindRedis)
INCLUDE(FindNode)

# Find packages
#FIND_PACKAGE(Boost)

# Add include/link directories
INCLUDE_DIRECTORIES("vendor/libgit2/include")
INCLUDE_DIRECTORIES(${LIBMYSQL_INCLUDE_DIR})
#INCLUDE_DIRECTORIES(${LIBREDIS_INCLUDE_DIR})
#INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
#INCLUDE_DIRECTORIES(${LIBTHRIFT_INCLUDE_DIR})
#INCLUDE_DIRECTORIES(${LIBMONGO_INCLUDE_DIR})
#INCLUDE_DIRECTORIES(${LIBPQ_INCLUDE_DIR})
#INCLUDE_DIRECTORIES(${LIBR_INCLUDE})
INCLUDE_DIRECTORIES(${NODE_GYP_DIR}/src)
INCLUDE_DIRECTORIES(${NODE_GYP_DIR}/deps/uv/include)
INCLUDE_DIRECTORIES(${NODE_GYP_DIR}/deps/v8/include)
INCLUDE_DIRECTORIES(node_modules/nan)
LINK_DIRECTORIES(${LIBMYSQL_LIBRARY})
#LINK_DIRECTORIES(${LIBTHRIFTL_LIBRARY})
#LINK_DIRECTORIES(${LIBR_LIBRARY})

# Add mysql project
ADD_SUBDIRECTORY(native/mysql)

# postgresql
#ADD_SUBDIRECTORY(native/postgresql)

# git mysql
ADD_SUBDIRECTORY(native/git/mysql)

# mongodb
#ADD_SUBDIRECTORY(native/mongodb)

# redis
#ADD_SUBDIRECTORY(native/redis)

# R
#ADD_SUBDIRECTORY(native/R)

# HBase
#ADD_SUBDIRECTORY(native/hbase)

# native test app
ADD_SUBDIRECTORY(native/test)

# addon
ADD_SUBDIRECTORY(native/addon)
