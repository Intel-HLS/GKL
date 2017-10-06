#---------------------------------------------------------------------
# smithwaterman
#---------------------------------------------------------------------
set(TARGET gkl_smithwaterman)

add_compile_options(-mavx2)

add_library(${TARGET} SHARED
    IntelSmithWaterman.cc
    PairWiseSW.cpp)

install(TARGETS ${TARGET} DESTINATION ${CMAKE_BINARY_DIR})

