cmake_minimum_required(VERSION 3.24.2)
project(gba_extract)

set(CMAKE_CXX_STANDARD 17)

add_executable(gba_extract
	include/audio.hpp
	include/camera.hpp 			source/camera.cpp
	include/controller.hpp		source/controller.cpp
	include/game.hpp 			source/game.cpp
	include/graphics.hpp 		source/graphics.cpp
	include/main.hpp 			source/main.cpp
	include/scrollState.hpp		source/scrollState.cpp
	include/renderBuffer.hpp 	source/renderBuffer.cpp
	include/renderProgram.hpp 	source/renderProgram.cpp
	include/window.hpp 			source/window.cpp
	dependencies/glad/src/glad.c
)

#set_target_properties(gba_extract PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ../ RUNTIME_OUTPUT_DIRECTORY_RELEASE ../)

target_include_directories(gba_extract PRIVATE include PUBLIC dependencies PUBLIC dependencies/glm-0.9.9.8/glm PUBLIC dependencies/glad/include PUBLIC dependencies/SDL2-2.26.4/include PUBLIC dependencies/rapidjson-1.1.0/include)
target_link_directories(gba_extract PUBLIC dependencies/SDL2-2.26.4/lib)
target_link_libraries(gba_extract SDL2main SDL2 psapi)

target_compile_definitions(gba_extract PUBLIC USE_OPENGL_4_3)