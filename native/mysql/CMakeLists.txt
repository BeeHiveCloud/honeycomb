# honeycomb - mysql adapter

CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

# mysql adapter
FILE(GLOB SRC_MYSQL *.c *.h)
ADD_LIBRARY(mysqladapter STATIC ${SRC_MYSQL})
IF (UNIX)
  TARGET_LINK_LIBRARIES(mysqladapter mysqlclient_r)
ELSE ()
  TARGET_LINK_LIBRARIES(mysqladapter libmysql.lib)
ENDIF ()
