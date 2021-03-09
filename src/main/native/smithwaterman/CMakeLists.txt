#---------------------------------------------------------------------
# common
#---------------------------------------------------------------------
if(NOT APPLE)
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-as-needed")
endif()
set_property(SOURCE avx2_impl.cc APPEND_STRING PROPERTY COMPILE_FLAGS " -mavx -mavx2 ")
set_property(SOURCE avx512_impl.cc APPEND_STRING PROPERTY COMPILE_FLAGS " -mavx -mavx2 -mavx512f -mavx512dq -mavx512vl -mavx512bw ")


#---------------------------------------------------------------------
# common includes
#---------------------------------------------------------------------
include_directories(../common)


#---------------------------------------------------------------------
# smithwaterman
#---------------------------------------------------------------------
set(TARGET gkl_smithwaterman)


add_library(${TARGET} SHARED
    IntelSmithWaterman.cc avx2_impl.cc avx512_impl.cc smithwaterman_common.cc)

install(TARGETS ${TARGET} DESTINATION ${CMAKE_BINARY_DIR})

