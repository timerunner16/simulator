cmake_minimum_required(VERSION 4.0)
project(simulator)

set(CMAKE_CXX_STANDARD 23)

# VENDORED LIBS
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
if(LINUX)
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	find_package(Threads REQUIRED)
	find_package(Vulkan REQUIRED)
	find_package(X11 REQUIRED)
	target_link_libraries(sokol INTERFACE
		X11::X11
		X11::Xi
		X11::Xcursor
		Vulkan::Vulkan
		dl
		m)
	target_link_libraries(sokol PUBLIC Threads::Threads)
endif()
target_include_directories(sokol INTERFACE sokol)
target_link_libraries(sokol PUBLIC imgui)
if (EMSCRIPTEN)
	target_link_options(sokol PRIVATE
		-sMAX_WEBGL_VERSION=2
		-sMIN_WEBGL_VERSION=2
	)
endif()

add_subdirectory(lua)

add_library(sol2 INTERFACE)
target_include_directories(sol2 BEFORE INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/sol2/include)
target_link_libraries(sol2 INTERFACE lua)
add_library(sol2::sol2 ALIAS sol2)
target_compile_definitions(sol2 INTERFACE SOL_USING_CXX_LUA=0)

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
target_link_libraries(render PUBLIC core)
target_link_libraries(render PUBLIC sokol)
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

target_link_libraries(simulator PRIVATE
	sokol
	lua
	sol2::sol2
	core
	render
	input
	fileaccess
	audio
	entity
	resources)

set(shadergen_src ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders)
set(shadergen_out ${CMAKE_CURRENT_BINARY_DIR}/shaders_generated)
set(shadergen_langs glsl300es:spirv_vk)
add_custom_command(
	OUTPUT ${shadergen_out}/test.h
	COMMAND
		sokol-shdc
		-i ${shadergen_src}/test.glsl
		-o ${shadergen_out}/test.h
		-l ${shadergen_langs}
	DEPENDS ${shadergen_src}/test.glsl
	VERBATIM)

add_library(shadergen INTERFACE)
target_sources(shadergen INTERFACE ${shadergen_out}/test.h)
target_include_directories(shadergen INTERFACE ${shadergen_out})

target_link_libraries(simulator PRIVATE shadergen)

if(EMSCRIPTEN)
	set_target_properties(simulator PROPERTIES
		OUTPUT_NAME "index"
		SUFFIX ".html"
	)
	target_link_options(simulator PRIVATE
		--shell-file ${PROJECT_SOURCE_DIR}/web/index.html)
	target_link_options(simulator PRIVATE
		-sMAX_WEBGL_VERSION=2
		-sMIN_WEBGL_VERSION=2
	)
	target_link_options(simulator PRIVATE
		-sMALLOC=emmalloc --closure=1)
endif()

if (CMAKE_C_COMPILER_ID MATCHES "Clang" AND
		NOT EMSCRIPTEN)
	target_link_options(simulator PRIVATE LINKER:-dead_strip)
endif()
