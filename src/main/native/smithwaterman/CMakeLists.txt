#---------------------------------------------------------------------
# common
#---------------------------------------------------------------------
if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
  set (ARCH "aarch64")
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
  set (ARCH "x85_64")
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "amd64")
  set (ARCH "x86_64")
endif ()

if (${ARCH} STREQUAL "x86_64")
  if(NOT APPLE)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-as-needed")
  endif()
  set_property(SOURCE avx2_impl.cc APPEND_STRING PROPERTY COMPILE_FLAGS " -mavx -mavx2 ")
  set_property(SOURCE avx512_impl.cc APPEND_STRING PROPERTY COMPILE_FLAGS " -mavx -mavx2 -mavx512f -mavx512dq -mavx512vl -mavx512bw ")
elseif (${ARCH} STREQUAL "aarch64")
  set_property(SOURCE neon_impl.cc APPEND_STRING PROPERTY COMPILE_FLAGS " -mcpu=native ")
endif()


#---------------------------------------------------------------------
# common includes
#---------------------------------------------------------------------
include_directories(../common)


#---------------------------------------------------------------------
# smithwaterman
#---------------------------------------------------------------------
set(TARGET gkl_smithwaterman)


if (${ARCH} STREQUAL "x86_64")
  add_library(${TARGET} SHARED IntelSmithWaterman.cc avx2_impl.cc avx512_impl.cc)
elseif (${ARCH} STREQUAL "aarch64")
  add_library(${TARGET} SHARED IntelSmithWaterman.cc neon_impl.cc)
endif()

install(TARGETS ${TARGET} DESTINATION ${CMAKE_BINARY_DIR})

