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
  set_property(SOURCE avx_impl.cc APPEND_STRING PROPERTY COMPILE_FLAGS " -mavx ")
  set_property(SOURCE avx512_impl.cc APPEND_STRING PROPERTY COMPILE_FLAGS " -mavx -mavx2 -mavx512f -mavx512dq -mavx512vl ")
elseif (${ARCH} STREQUAL "aarch64")
  set_property(SOURCE neon_impl.cc APPEND_STRING PROPERTY COMPILE_FLAGS " -mcpu=native ")
endif()

#---------------------------------------------------------------------
# common includes
#---------------------------------------------------------------------
include_directories(../common)

#---------------------------------------------------------------------
# pairhmm_shacc (stub version)
#---------------------------------------------------------------------
set(TARGET gkl_pairhmm_shacc)
add_library(${TARGET} SHARED shacc_pairhmm_stub.cc)
install(TARGETS ${TARGET} DESTINATION ${CMAKE_BINARY_DIR})

#---------------------------------------------------------------------
# pairhmm
#---------------------------------------------------------------------
set(TARGET gkl_pairhmm)
if (${ARCH} STREQUAL "x86_64")
  add_library(${TARGET} SHARED IntelPairHmm.cc avx_impl.cc avx512_impl.cc pairhmm_common.cc)
  if (APPLE)
    target_link_libraries(${TARGET} gkl_pairhmm_shacc)
  endif()
elseif (${ARCH} STREQUAL "aarch64")
  add_library(${TARGET} SHARED IntelPairHmm.cc neon_impl.cc pairhmm_common.cc)
endif()
install(TARGETS ${TARGET} DESTINATION ${CMAKE_BINARY_DIR})

#---------------------------------------------------------------------
# pairhmm_omp
#---------------------------------------------------------------------
find_package(OpenMP)

if(OPENMP_FOUND)
  set(TARGET gkl_pairhmm_omp)
  if (${ARCH} STREQUAL "x86_64")
    add_library(${TARGET} SHARED IntelPairHmm.cc avx_impl.cc avx512_impl.cc pairhmm_common.cc)
  elseif (${ARCH} STREQUAL "aarch64")
    add_library(${TARGET} SHARED IntelPairHmm.cc neon_impl.cc pairhmm_common.cc)
  endif()
  set_target_properties(${TARGET} PROPERTIES COMPILE_OPTIONS ${OpenMP_CXX_FLAGS})
  target_link_libraries(${TARGET} ${OpenMP_CXX_FLAGS})
  install(TARGETS ${TARGET} DESTINATION ${CMAKE_BINARY_DIR})
endif()

#---------------------------------------------------------------------
# pairhmm_fpga
#---------------------------------------------------------------------
set(TARGET gkl_pairhmm_fpga)
if (${ARCH} STREQUAL "x86_64")
  add_library(${TARGET} SHARED IntelPairHmm.cc avx_impl.cc avx512_impl.cc pairhmm_common.cc)
elseif (${ARCH} STREQUAL "aarch64")
  add_library(${TARGET} SHARED IntelPairHmm.cc neon_impl.cc pairhmm_common.cc)
endif()
target_link_libraries(${TARGET} gkl_pairhmm_shacc)
install(TARGETS ${TARGET} DESTINATION ${CMAKE_BINARY_DIR})
