#---------------------------------------------------------------------
# otc_zlib
#---------------------------------------------------------------------
add_subdirectory(otc_zlib EXCLUDE_FROM_ALL)

#---------------------------------------------------------------------
# isa-l
#---------------------------------------------------------------------
if(APPLE)
   set(CONFIG_OPT "-target=darwin")
endif()

include(ExternalProject)

ExternalProject_Add(
   isalExternal
   URL ${CMAKE_CURRENT_SOURCE_DIR}/isa-l-master
   UPDATE_COMMAND <SOURCE_DIR>/autogen.sh
   PATCH_COMMAND patch < ${CMAKE_CURRENT_SOURCE_DIR}/isa-l.patch
   CONFIGURE_COMMAND CFLAGS=-fPIC <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> ${CONFIG_OPT}
   BUILD_COMMAND make
   INSTALL_COMMAND make install
)

add_library(isal STATIC IMPORTED GLOBAL)
set_target_properties(isal PROPERTIES IMPORTED_LOCATION
  ${CMAKE_CURRENT_BINARY_DIR}/isalExternal-prefix/lib/libisal.a
)

#---------------------------------------------------------------------
# gkl_compression
#---------------------------------------------------------------------
set(TARGET gkl_compression)

include_directories(isa-l-master/include)

add_library(${TARGET} SHARED 
  IntelDeflater.cc
  IntelInflater.cc
  )
add_dependencies(${TARGET} isalExternal)
target_link_libraries(${TARGET} zlibstatic isal)
install(TARGETS ${TARGET} DESTINATION ${CMAKE_BINARY_DIR})
