
message("Running Win32 CMake Build...")

set(VULKAN_ROOT "C:\\VulkanSDK\\1.3.280.0")
set(VULKAN_INCLUDE "${VULKAN_ROOT}\\Include")

if (EXISTS ${VULKAN_ROOT})
include_directories(${VULKAN_INCLUDE})
list(APPEND CMAKE_PREFIX_PATH "C:\\VulkanSDK\\1.3.280.0\\Bin")
endif()

include(CMakeFindDependencyMacro)
find_package(Vulkan REQUIRED)


find_dependency(OpenGL REQUIRED )

# include_directories( ${OPENGL_INCLUDE_DIRS} external/ImGuizmo/include external/box2d/include
include_directories( ${OPENGL_INCLUDE_DIRS} external/box2d/include)

set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )
set( GLAD_GL "" )
add_subdirectory(external/glfw)

option( GLFW-CMAKE-STARTER-USE-GLFW-GLAD "Use GLAD from GLFW" ON )

if( GLFW-CMAKE-STARTER-USE-GLFW-GLAD )
    include_directories("${GLFW_SOURCE_DIR}/deps")
    set( GLAD_GL "${GLFW_SOURCE_DIR}/deps/glad/gl.h" )
endif()

if( MSVC )
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup" )
endif()

add_subdirectory(external/glm)
add_subdirectory(external/yaml-cpp)

add_subdirectory(external/Box2D-cmake)
# add_subdirectory(external/box2d)
# find_package(box2d REQUIRED)

include(FetchContent)
add_subdirectory(external/fmt)


add_subdirectory(external/spdlog)

# add_subdirectory(external/imgui)

add_subdirectory(external/glad)

# Setting our assimp dependencies here
# set(BUILD_SHARED_LIBS OFF)
# if(APPLE)
# set(BUILD_FRAMEWORK ON)
# endif()
# set(ASSIMP_DOUBLE_PRECISION ON)
# set(ASSIMP_BUILD_TESTS OFF)
# set(INJECT_DEBUG_POSTFIX ON)
# set(ASSIMP_INSTALL_PDB ON)
# add_definitions(-D__GLIBCXX_TYPE_INT_N_0=__int128)

# add_definitions(-D__GLIBCXX_BITSIZE_INT_N_0=128) 
add_subdirectory(external/assimp)
# include(FetchContent)
# set(FETCHCONTENT_BASE_DIR ${PROJECT_SOURCE_DIR}/external CACHE PATH "Missing description." FORCE)

# FetchContent_Declare(assimp
# GIT_REPOSITORY https://github.com/assimp/assimp.git
# GIT_TAG master)
# set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
# set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
# set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
# set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

# FetchContent_MakeAvailable(assimp)
# target_include_directories(${PROJECT_NAME} PRIVATE external/assimp-src/include)
# target_link_libraries(3d_renderer assimp)

target_link_libraries(
    ${PROJECT_NAME}
    assimp::assimp
    ${OPENGL_LIBRARIES}
    glfw
    ${Vulkan_LIBRARIES}
    fmt::fmt
    spdlog::spdlog
    glm::glm
    tobanteGaming::Box2D
    yaml-cpp::yaml-cpp
	glad
)