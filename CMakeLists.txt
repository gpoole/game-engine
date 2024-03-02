cmake_minimum_required(VERSION 3.15)
project(GameEngine VERSION 0.1.0 LANGUAGES C CXX)

# include(CTest)
# enable_testing()

find_package(SDL2_image REQUIRED CONFIG)
find_package(SDL2 REQUIRED CONFIG)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED CONFIG)
find_package(opengl_system REQUIRED CONFIG)

add_executable(${PROJECT_NAME} src/main.cpp)

# FIXME: glm sample uses the PRIVATE keyword?
# https://github.com/conan-io/conan-center-index/blob/master/recipes/glm/all/test_package/CMakeLists.txt#L7
target_link_libraries(${PROJECT_NAME} SDL2::SDL2main GLEW::GLEW glm::glm)

# Link SDL
# https://github.com/conan-io/conan-center-index/blob/master/recipes/sdl/all/test_package/CMakeLists.txt#L8
if(TARGET SDL2::SDL2-static)
    target_link_libraries(${PROJECT_NAME} SDL2::SDL2-static)
else()
    target_link_libraries(${PROJECT_NAME} SDL2::SDL2)
endif()

# Link SDL_Image
# https://github.com/conan-io/conan-center-index/blob/master/recipes/sdl_image/all/test_package/CMakeLists.txt#L7C1-L11C8
# FIXME: as with glm above the sample uses the PRIVATE keyword
if(TARGET SDL2_image::SDL2_image-static)
    target_link_libraries(${PROJECT_NAME} SDL2_image::SDL2_image-static)
else()
    target_link_libraries(${PROJECT_NAME} SDL2_image::SDL2_image)
endif()

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