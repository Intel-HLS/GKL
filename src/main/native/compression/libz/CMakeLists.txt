project(GKL)
cmake_minimum_required(VERSION 2.8)


# common compiler and linker settings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3 -mavx")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
if(APPLE)
  set(CMAKE_SHARED_LINKER_FLAGS "-static-libstdc++")
elseif(UNIX)
  set(CMAKE_SHARED_LINKER_FLAGS "-static-libstdc++ -static-libgcc")
endif()


include(ExternalProject)

#---------------------------------------------------------------------
# isa-l
#---------------------------------------------------------------------

ExternalProject_Add(
   isalExternal
   URL ${CMAKE_CURRENT_SOURCE_DIR}/../isa-l-master
   UPDATE_COMMAND <SOURCE_DIR>/autogen.sh
   PATCH_COMMAND patch < ${CMAKE_CURRENT_SOURCE_DIR}/isa-l.patch
   CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> 
   BUILD_COMMAND make
   INSTALL_COMMAND make install
)
add_library(isal STATIC IMPORTED GLOBAL)
set_target_properties(isal PROPERTIES IMPORTED_LOCATION
  ${CMAKE_CURRENT_BINARY_DIR}/isalExternal-prefix/lib/libisal.a
)

ADD_CUSTOM_COMMAND(
      TARGET isalExternal
      PRE_BUILD
      COMMAND cp
      ARGS ${CMAKE_CURRENT_BINARY_DIR}/../isa-l-master/include/igzip_lib.h  ${CMAKE_CURRENT_BINARY_DIR}/../otc_zlib/igzip_lib.h)

ADD_CUSTOM_COMMAND(
      TARGET isalExternal
      PRE_BUILD
      COMMAND cp
      ARGS ${CMAKE_CURRENT_BINARY_DIR}/../isa-l-master/include/types.h  ${CMAKE_CURRENT_BINARY_DIR}/../otc_zlib/types.h)

ADD_CUSTOM_COMMAND(
      TARGET isalExternal
      PRE_BUILD
      COMMAND cp
      ARGS ${CMAKE_CURRENT_BINARY_DIR}/isalExternal-prefix/lib/libisal.a  ${CMAKE_CURRENT_BINARY_DIR}/zlib-1.2.8devn-jtkv5External-prefix/src/zlib-1.2.8devn-jtkv5External-build/libisal.a)

#---------------------------------------------------------------------
# otc_zlib
#---------------------------------------------------------------------
#add_subdirectory(otc_zlib EXCLUDE_FROM_ALL)
#add_subdirectory(zlib-1.2.8devn-jtkv5 EXCLUDE_FROM_ALL)

ExternalProject_Add(
   zlib-1.2.8devn-jtkv5External
   URL ${CMAKE_CURRENT_SOURCE_DIR}/../otc_zlib
   PATCH_COMMAND patch < ${CMAKE_CURRENT_SOURCE_DIR}/zlib.patch
   CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>
   BUILD_COMMAND make
   INSTALL_COMMAND make install
)



