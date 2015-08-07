# honeycomb - mysql adapter

CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

# mysql adapter
FILE(GLOB SRC_MYSQL *.c *.h)
ADD_LIBRARY(mysqladapter STATIC ${SRC_MYSQL})
IF (MSVC_IDE)
  TARGET_LINK_LIBRARIES(mysqladapter libmysql)
ELSE ()
  TARGET_LINK_LIBRARIES(mysqladapter mysqlclient_r)
ENDIF ()
