#---------------------------------------------------------------------
# ftz
#---------------------------------------------------------------------
set(TARGET gkl_utils)

add_library(${TARGET} SHARED utils.cc)
install(TARGETS ${TARGET} DESTINATION ${CMAKE_BINARY_DIR})
