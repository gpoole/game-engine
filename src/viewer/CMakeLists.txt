project(viewer VERSION 0.1.0 LANGUAGES C CXX)

find_package(imgui REQUIRED CONFIG)

set(viewer_SOURCES
    main.cpp
)

add_executable(viewer ${viewer_SOURCES})

target_link_libraries(viewer
    core
    ${imgui_LIBRARIES}
    ${SDL2_LBRARIES}
    ${SDL2_image_LIBRARIES}
    ${glew_LIBRARIES}
    ${glm_LIBRARIES}
)

target_include_directories(viewer PUBLIC ../)
