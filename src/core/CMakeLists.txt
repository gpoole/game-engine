set(core_SOURCES
    Md2Model.cpp
    Texture.cpp
)

add_library(core ${core_SOURCES})

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
