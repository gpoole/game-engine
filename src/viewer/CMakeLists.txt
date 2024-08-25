project(viewer VERSION 0.1.0 LANGUAGES C CXX)

set(viewer_SOURCES
    main.cpp
)

set(IMGUI_DIR ../vendor/imgui)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    # Doesn't appear to be necessary...
    # ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

add_executable(viewer ${viewer_SOURCES} ${IMGUI_SOURCES})

target_link_libraries(viewer
    core
    ${SDL2_LBRARIES}
    ${SDL2_image_LIBRARIES}
    ${glew_LIBRARIES}
    ${glm_LIBRARIES}
)

target_include_directories(viewer PUBLIC ../)
target_include_directories(viewer PUBLIC ../vendor/imgui ../vendor/imgui/backends)
