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

set(SOURCES
    src/main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    core
    ${SDL2_LBRARIES}
    ${SDL2_image_LIBRARIES}
    ${glew_LIBRARIES}
    ${glm_LIBRARIES}
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

# Copy assets to build directory
# https://github.com/Gaetz/SDL-OpenGL/blob/master/CMakeLists.txt#L36
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_LIST_DIR}/assets
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    COMMENT "---- Copy Assets"
)
