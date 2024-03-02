cmake_minimum_required(VERSION 3.15)
project(GameEngine VERSION 0.1.0 LANGUAGES C CXX)

# include(CTest)
# enable_testing()

find_package(SDL2 REQUIRED CONFIG)
find_package(GLEW REQUIRED)
find_package(opengl_system REQUIRED CONFIG)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} SDL2::SDL2main GLEW::GLEW)

# Link SDL
# https://github.com/conan-io/conan-center-index/blob/master/recipes/sdl/all/test_package/CMakeLists.txt#L8
if(TARGET SDL2::SDL2-static)
    target_link_libraries(${PROJECT_NAME} SDL2::SDL2-static)
else()
    target_link_libraries(${PROJECT_NAME} SDL2::SDL2)
endif()

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
