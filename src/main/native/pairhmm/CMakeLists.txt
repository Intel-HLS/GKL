#---------------------------------------------------------------------
# common
#---------------------------------------------------------------------
if(NOT APPLE)
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-as-needed")
endif()
set_property(SOURCE avx_impl.cc APPEND_STRING PROPERTY COMPILE_FLAGS " -mavx ")
set_property(SOURCE avx512_impl.cc APPEND_STRING PROPERTY COMPILE_FLAGS " -mavx -mavx2 -mavx512f -mavx512dq -mavx512vl ")

#---------------------------------------------------------------------
# common includes
#---------------------------------------------------------------------
include_directories(../common)

#---------------------------------------------------------------------
# pairhmm
#---------------------------------------------------------------------
set(TARGET gkl_pairhmm)
add_library(${TARGET} SHARED IntelPairHmm.cc avx_impl.cc avx512_impl.cc pairhmm_common.cc)
install(TARGETS ${TARGET} DESTINATION ${CMAKE_BINARY_DIR})

#---------------------------------------------------------------------
# pairhmm_omp
#---------------------------------------------------------------------
find_package(OpenMP)

if(OPENMP_FOUND)
  set(TARGET gkl_pairhmm_omp)
  add_library(${TARGET} SHARED IntelPairHmm.cc avx_impl.cc avx512_impl.cc pairhmm_common.cc)
  set_target_properties(${TARGET} PROPERTIES COMPILE_OPTIONS ${OpenMP_CXX_FLAGS})
  target_link_libraries(${TARGET} ${OpenMP_CXX_FLAGS})
  install(TARGETS ${TARGET} DESTINATION ${CMAKE_BINARY_DIR})
endif()
