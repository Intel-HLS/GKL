project(GKL)
cmake_minimum_required(VERSION 2.8)

# add jni include path
find_package(Java 1.8 REQUIRED)
find_package(JNI)
include_directories(${JNI_INCLUDE_DIRS})

# common compiler and linker settings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3 -mavx")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
if(APPLE)
  set(CMAKE_SHARED_LINKER_FLAGS "-static-libstdc++")
elseif(UNIX)
  set(CMAKE_SHARED_LINKER_FLAGS "-static-libstdc++ -static-libgcc")
endif()

# add CMakeLists.txt in subdirectories
set(NATIVE_DIR ${PROJECT_SOURCE_DIR}/src/main/native)

add_subdirectory("${NATIVE_DIR}/compression")
add_subdirectory("${NATIVE_DIR}/pairhmm")
add_subdirectory("${NATIVE_DIR}/ftz")
#add_subdirectory("${NATIVE_DIR}/smithwaterman")
