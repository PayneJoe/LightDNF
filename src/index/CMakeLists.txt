CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

#
SET(INDEX_SRCS InversedIndex.hpp Container.hpp)

# ===================== build
if(TARGET index)
  MESSAGE("index has already been defined")
else()
  # -o libioutils.la *.hpp
  ADD_LIBRARY(index INTERFACE)
  #
  TARGET_LINK_LIBRARIES(index INTERFACE jsoncpp)
endif()
