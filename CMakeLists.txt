cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(hf-ship-editor)

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_DEFAULT 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_C_LINKER_PREFERENCE_PROPAGATES OFF)
set(CMAKE_CXX_LINKER_PREFERENCE_PROPAGATES ON)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "" FORCE)
endif()

set(_userconfig "userconfig-${CMAKE_CXX_COMPILER_ID}.cmake")
if(EXISTS "${CMAKE_SOURCE_DIR}/${_userconfig}")
    include("${CMAKE_SOURCE_DIR}/${_userconfig}")
else()
    message(STATUS "user config '${_userconfig}' not found")
endif()

if(MSVC)
    add_compile_options(-EHsc)
endif()

if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_USE_MATH_DEFINES=1 -DNOMINMAX -DWIN32_LEAN_AND_MEAN)
endif()

file(GLOB sources  "*.cpp" "*.c" CONFIGURE_ARGS)
add_executable(hf-design "${sources}")

install(TARGETS hf-design RUNTIME DESTINATION bin)
