cmake_minimum_required(VERSION 3.15)
project(GameEngine VERSION 0.1.0 LANGUAGES C CXX)

# include(CTest)
# enable_testing()

find_package(opengl_system REQUIRED CONFIG)

find_package(SDL2 REQUIRED CONFIG)
find_package(GLEW REQUIRED CONFIG)

set(SOURCES main.cpp)

# https://github.com/conan-io/conan-center-index/blob/56a3277676b5f42ddcbcbc12f6ab97a31c30d321/recipes/opengl/all/test_package/CMakeLists.txt#L8
# if(WIN32)
#     list(APPEND SOURCES win.cpp)
# endif()

if(APPLE)
    list(APPEND SOURCES osx.mm)
    set_source_files_properties(osx.mm PROPERTIES COMPILE_FLAGS "-x objective-c++")

    list(APPEND PLATFORM_LIBS "objc")

    find_library(APPKIT_LIBRARY AppKit)
    find_library(FOUNDATION_LIBRARY Foundation)

    if(APPKIT_LIBRARY)
        list(APPEND PLATFORM_LIBS ${APPKIT_LIBRARY})
    endif()

    if(FOUNDATION_LIBRARY)
        list(APPEND PLATFORM_LIBS ${FOUNDATION_LIBRARY})
    endif()
endif()

# find_package(OpenGL)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} SDL2::SDL2main GLEW::GLEW)
if(TARGET SDL2::SDL2-static)
    target_link_libraries(${PROJECT_NAME} SDL2::SDL2-static)
else()
    target_link_libraries(${PROJECT_NAME} SDL2::SDL2)
endif()

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
