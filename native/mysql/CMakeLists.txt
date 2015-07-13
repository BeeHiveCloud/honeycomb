# honeycomb - mysql module

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# git mysql
FILE(GLOB SRC_MYSQL *.c *.h)
ADD_LIBRARY(mysql STATIC ${SRC_MYSQL})
IF (MSVC_IDE)
  TARGET_LINK_LIBRARIES(mysql libmysql)
ELSE ()
  TARGET_LINK_LIBRARIES(mysql mysqlclient_r)
ENDIF ()
