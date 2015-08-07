# honeycomb - test module

CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

FILE(GLOB SRC_TEST *.h *.c)
ADD_EXECUTABLE(ntest ${SRC_TEST})
ADD_DEPENDENCIES(ntest mysqladapter)
TARGET_LINK_LIBRARIES(ntest mysqladapter)
#TARGET_LINK_LIBRARIES(ntest mygit)
#TARGET_LINK_LIBRARIES(ntest git2)
