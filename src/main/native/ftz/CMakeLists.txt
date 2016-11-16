#---------------------------------------------------------------------
# ftz
#---------------------------------------------------------------------
set(TARGET gkl_ftz)

add_library(${TARGET} SHARED ftz.cc)
install(TARGETS ${TARGET} DESTINATION ${CMAKE_BINARY_DIR})
