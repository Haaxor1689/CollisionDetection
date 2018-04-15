cmake_minimum_required(VERSION 3.2)

include(ExternalProject)

project(MyEngine)

# MacOS library find
if(APPLE)
  find_library(COCOA_LIBRARY Cocoa)
  find_library(OPENGL_LIBRARY OpenGL)
  find_library(IOKIT_LIBRARY IOKit)
  find_library(COREVIDEO_LIBRARY CoreVideo)
endif()

if(UNIX)
    find_package(OpenGL REQUIRED)
    find_package(X11 REQUIRED)
    find_package(Threads REQUIRED)
endif()

# GLFW library
set(GLFW_INSTALL_DIR "${CMAKE_BINARY_DIR}/glfw")
set(GLFW_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw/include")
set(GLFW_LIBRARIES   "${GLFW_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}glfw3${CMAKE_STATIC_LIBRARY_SUFFIX}")
ExternalProject_Add(glfw
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw"

    DOWNLOAD_COMMAND ""
    UPDATE_COMMAND ""

    CMAKE_ARGS
        "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/glfw"
        "-DGLFW_BUILD_EXAMPLES=OFF"
        "-DGLFW_BUILD_TESTS=OFF"
        "-DGLFW_BUILD_DOCS=OFF"
        "-DBUILD_SHARED_LIBS=OFF"

    BUILD_BYPRODUCTS ${GLFW_LIBRARIES}
)

if(APPLE)
    set(GLFW_LIBRARIES ${GLFW_LIBRARIES} ${COCOA_LIBRARY} ${OPENGL_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY})
endif()

if(UNIX)
    set(GLFW_LIBRARIES ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES} ${X11_LIBRARIES} ${X11_Xrandr_LIB} ${X11_Xcursor_LIB} ${X11_Xinerama_LIB} ${X11_Xxf86vm_LIB} ${CMAKE_THREAD_LIBS_INIT})
endif()


# GLAD library
set(GLAD_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/glad/include")
add_library(glad "${CMAKE_CURRENT_SOURCE_DIR}/libs/glad/src/glad.c")
target_include_directories(glad PRIVATE "${GLAD_INCLUDE_DIR}")
set(GLAD_LIBARIES)
if(UNIX)
    set(GLAD_LIBRARIES "glad" ${CMAKE_DL_LIBS})
else()
    set(GLAD_LIBRARIES glad)
endif()


# Single header libraries
set(SINGLE_HEADER_LIBS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/single_header")

# Configure compiler flags for our project
if (CMAKE_CXX_COMPILER_ID MATCHES Clang OR ${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
  # using regular Clang or AppleClang or GCC
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  # using Visual Studio C++
  set(CMAKE_CXX_FLAGS "/std:c++14 /permissive- /W3 /EHsc")
endif()

# Framework
set(FRAMEWORK_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/framework/include")
set(FRAMEWORK_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/framework/src")
add_library(framework
	"${FRAMEWORK_SRC_DIR}/window.cpp"
	"${FRAMEWORK_SRC_DIR}/utility.cpp"
	"${FRAMEWORK_SRC_DIR}/shader.cpp"
	"${FRAMEWORK_SRC_DIR}/program.cpp"
	"${FRAMEWORK_SRC_DIR}/mesh.cpp"
    "${FRAMEWORK_SRC_DIR}/camera.cpp"
    "${FRAMEWORK_SRC_DIR}/texture.cpp"
)
add_dependencies(framework glfw glad)

set(GEOMETRY_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/geometry")

target_link_libraries(framework ${GLFW_LIBRARIES} ${GLAD_LIBRARIES})
target_include_directories(framework 
        PRIVATE ${GLFW_INCLUDE_DIR}
        PRIVATE ${GLAD_INCLUDE_DIR}
	    PRIVATE ${FRAMEWORK_INCLUDE_DIR}
	    PRIVATE ${SINGLE_HEADER_LIBS_INCLUDE_DIR}
        PRIVATE ${GEOMETRY_INCLUDE_DIR}
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/objects"
)

# Main
add_executable(Main Main.cpp Application.cpp)
target_link_libraries(Main ${GLFW_LIBRARIES} ${GLAD_LIBRARIES} framework)
target_include_directories(Main
        PRIVATE ${GLFW_INCLUDE_DIR}
        PRIVATE ${GLAD_INCLUDE_DIR}
        PRIVATE ${SINGLE_HEADER_LIBS_INCLUDE_DIR}
        PRIVATE ${FRAMEWORK_INCLUDE_DIR}
        PRIVATE ${GEOMETRY_INCLUDE_DIR}
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/objects"
)

# Tests
add_executable(Test TestsMain.cpp TestsGeometry.cpp TestsCollisions.cpp)
target_link_libraries(Test ${GLFW_LIBRARIES} ${GLAD_LIBRARIES} framework)
target_include_directories(Test
        PRIVATE ${GLFW_INCLUDE_DIR}
        PRIVATE ${GLAD_INCLUDE_DIR}
        PRIVATE ${SINGLE_HEADER_LIBS_INCLUDE_DIR}
        PRIVATE ${FRAMEWORK_INCLUDE_DIR}
        PRIVATE ${GEOMETRY_INCLUDE_DIR}
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/objects"
)