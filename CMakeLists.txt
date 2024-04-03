cmake_minimum_required(VERSION 3.15)
project(GameEngine VERSION 0.1.0 LANGUAGES C CXX)

# include(CTest)
# enable_testing()

find_package(SDL2_image REQUIRED CONFIG)
find_package(SDL2 REQUIRED CONFIG)
find_package(glew REQUIRED CONFIG)
find_package(glm REQUIRED CONFIG)
find_package(opengl_system REQUIRED CONFIG)

include_directories(
    ${SDL2_image_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${glm_INCLUDE_DIRS}
)

add_subdirectory( src/core )

add_subdirectory( src/viewer )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
