CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

#
SET(UTILS_SRCS FileLoader.hpp)

# ===================== build
if(TARGET utils)
  MESSAGE("utils has already been defined")
else()
  # -o libioutils.la *.hpp
  ADD_LIBRARY(utils INTERFACE)
  # -ljsoncpp
  TARGET_LINK_LIBRARIES(utils INTERFACE jsoncpp)
endif()

