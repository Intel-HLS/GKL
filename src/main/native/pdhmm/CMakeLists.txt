#---------------------------------------------------------------------
# common
#---------------------------------------------------------------------
if(NOT APPLE)
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-as-needed")
endif()
set_property(SOURCE MathUtils.cc APPEND_STRING PROPERTY COMPILE_FLAGS)
set_property(SOURCE pdhmm-serial.cc APPEND_STRING PROPERTY COMPILE_FLAGS)
set_property(SOURCE avx2_impl.cc APPEND_STRING PROPERTY COMPILE_FLAGS " -mavx -mavx2 -msse2 -msse4.1")
set_property(SOURCE avx512_impl.cc APPEND_STRING PROPERTY COMPILE_FLAGS " -mavx -mavx2 -mavx512f -mavx512dq -mavx512vl -mavx512bw")


#---------------------------------------------------------------------
# common includes
#---------------------------------------------------------------------
include_directories(../common)

find_package(OpenMP)

set(TARGET gkl_pdhmm)
add_library(${TARGET} SHARED IntelPDHMM.cc avx2_impl.cc avx512_impl.cc MathUtils.cc pdhmm-serial.cc)
if(OPENMP_FOUND)
  set_target_properties(${TARGET} PROPERTIES COMPILE_OPTIONS ${OpenMP_CXX_FLAGS})
  target_link_libraries(${TARGET} ${OpenMP_CXX_FLAGS})
endif()
install(TARGETS ${TARGET} DESTINATION ${CMAKE_BINARY_DIR})
