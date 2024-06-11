

include(ExternalProject)

#---------------------------------------------------------------------
# otc_zlib
#---------------------------------------------------------------------
#add_subdirectory(otc_zlib EXCLUDE_FROM_ALL)
#add_subdirectory(zlib-1.2.8devn-jtkv5 EXCLUDE_FROM_ALL)

ExternalProject_Add(
   otc_zlibExternal
   URL ${CMAKE_CURRENT_SOURCE_DIR}/otc_zlib
   PATCH_COMMAND patch < ${CMAKE_CURRENT_SOURCE_DIR}/otc_zlib.patch
   CONFIGURE_COMMAND <SOURCE_DIR>/configure --static --prefix=<INSTALL_DIR>
   BUILD_COMMAND $(MAKE)
   INSTALL_COMMAND $(MAKE) install
)

add_library(zlibstatic STATIC IMPORTED GLOBAL)
set_target_properties(zlibstatic PROPERTIES IMPORTED_LOCATION
   ${CMAKE_CURRENT_BINARY_DIR}/otc_zlibExternal-prefix/lib/libz.a
)

#---------------------------------------------------------------------
# isa-l
#---------------------------------------------------------------------
if(APPLE)
   set(CONFIG_OPT "-target=darwin")
endif()


ExternalProject_Add(
   isalExternal
   URL ${CMAKE_CURRENT_SOURCE_DIR}/isa-l-master
   UPDATE_COMMAND <SOURCE_DIR>/autogen.sh
   CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --with-pic=yes ${CONFIG_OPT}
   BUILD_COMMAND $(MAKE)
   INSTALL_COMMAND $(MAKE) install
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
add_dependencies(${TARGET} isalExternal otc_zlibExternal)
target_link_libraries(${TARGET} zlibstatic isal)
install(TARGETS ${TARGET} DESTINATION ${CMAKE_BINARY_DIR})
