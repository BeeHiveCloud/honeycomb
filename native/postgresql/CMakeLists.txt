# honeycomb - postgresql module

CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

FILE(GLOB SRC_PQ *.c *.h)
ADD_LIBRARY(postgresql STATIC ${SRC_PQ})
ADD_DEPENDENCIES(postgresql git2)
TARGET_LINK_LIBRARIES(postgresql ${LIBPQ_LIBRARY}/libpq)
