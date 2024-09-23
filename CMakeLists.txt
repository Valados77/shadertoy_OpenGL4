cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

set(PROJECT_NAME shadertoy_OpenGL4)
project(${PROJECT_NAME})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME} 
	src/main.cpp
	src/GLSLProgram.cpp	
	src/GLSLProgram.h
	src/vertexBuffer.cpp
	src/vertexBuffer.h
	src/indexBuffer.cpp
	src/indexBuffer.h
	src/vertexArray.cpp
	src/vertexArray.h
)

add_subdirectory(lib/glfw)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)

add_subdirectory(lib/glad)
target_link_libraries(${PROJECT_NAME} PUBLIC glad)

add_subdirectory(lib/glm)
target_link_libraries(${PROJECT_NAME} PUBLIC glm)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E  copy_directory
	${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res)
