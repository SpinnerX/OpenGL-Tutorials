cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME OpenGLTutorials)
set(CMAKE_CXX_STANDARD 20)

project(${PROJECT_NAME} VERSION 1.0)
# set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# add_compile_options(-fno-tree-vectorize)

file(COPY basics DESTINATION ${CMAKE_BINARY_DIR})

if(UNIX)
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)
endif(UNIX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "") # works (in creating the compile_commands.json file)

add_executable(${PROJECT_NAME} Application.cpp)

# if (NOT BUILD_EXECUTABLES)
#    set_target_properties(Application.cpp PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
# endif()

if(WIN32)
include(cmake/win32.cmake)
elseif(UNIX)
include(cmake/unix.cmake)
endif()