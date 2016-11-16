#---------------------------------------------------------------------
# pairhmm
#---------------------------------------------------------------------
set(TARGET gkl_pairhmm)

add_library(${TARGET} SHARED IntelPairHmm.cc)
install(TARGETS ${TARGET} DESTINATION ${CMAKE_BINARY_DIR})

#---------------------------------------------------------------------
# pairhmm_omp
#---------------------------------------------------------------------
set(TARGET gkl_pairhmm_omp)

find_package(OpenMP)

if(OPENMP_FOUND)
  add_library(${TARGET} SHARED IntelPairHmm.cc)
  set_target_properties(${TARGET} PROPERTIES COMPILE_OPTIONS ${OpenMP_CXX_FLAGS})
  set_target_properties(${TARGET} PROPERTIES LINK_FLAGS ${OpenMP_CXX_FLAGS})
  install(TARGETS ${TARGET} DESTINATION ${CMAKE_BINARY_DIR})
endif()
