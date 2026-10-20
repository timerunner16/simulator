cmake_minimum_required(VERSION 4.0)
project(simulator)

set(CMAKE_CXX_STANDARD 23)

# EXTERNAL LIBS
add_library(imgui STATIC
	imgui/imgui.cpp
	imgui/imgui_demo.cpp
	imgui/imgui_draw.cpp
	imgui/imgui_tables.cpp
	imgui/imgui_widgets.cpp)
target_include_directories(imgui INTERFACE imgui)

add_library(sokol STATIC
	sokol/sokol.cxx
	sokol/sokol_app.h
	sokol/sokol_gfx.h
	sokol/sokol_glue.h
	sokol/sokol_log.h)
target_link_libraries(sokol PRIVATE GL dl m)
target_include_directories(sokol INTERFACE sokol)

target_link_libraries(sokol PUBLIC imgui)

add_subdirectory(lua)

set(SOL2_LUA_VERSION "5.4.8")
set(SOL2_BUILD_LUA FALSE)
add_subdirectory(sol2)

# ENGINE LIBS
add_library(core STATIC
	core/core.cxx
	core/vec2.cxx
	core/vec3.cxx
	core/vec4.cxx
	core/mat2.cxx
	core/mat3.cxx
	core/mat4.cxx
)
target_include_directories(core INTERFACE core)

add_library(render STATIC
	render/render.cxx)
target_include_directories(render INTERFACE render)

add_library(input STATIC
	input/input.cxx)
target_include_directories(input INTERFACE input)

add_library(fileaccess STATIC
	fileaccess/fileaccess.cxx)
target_include_directories(fileaccess INTERFACE fileaccess)

add_library(audio STATIC
	audio/audio.cxx)
target_include_directories(audio INTERFACE audio)

add_library(entity STATIC
	entity/entity.cxx)
target_include_directories(entity INTERFACE entity)

add_library(resources STATIC
	resources/resources.cxx)
target_include_directories(resources INTERFACE resources)

# GAME
add_executable(simulator
	src/main.cxx)

target_link_libraries(simulator PRIVATE sokol)
target_link_libraries(simulator PRIVATE lua)
target_link_libraries(simulator PRIVATE sol2::sol2)

target_link_libraries(simulator PRIVATE core)
target_link_libraries(simulator PRIVATE render)
target_link_libraries(simulator PRIVATE input)
target_link_libraries(simulator PRIVATE fileaccess)
target_link_libraries(simulator PRIVATE audio)
target_link_libraries(simulator PRIVATE entity)
target_link_libraries(simulator PRIVATE resources)

set_target_properties(simulator PROPERTIES
	OUTPUT_NAME "index"
	SUFFIX ".html"
)
target_link_options(simulator PRIVATE --shell-file ../web/index.html)
target_link_options(simulator PRIVATE -sUSE_WEBGL2=1)
