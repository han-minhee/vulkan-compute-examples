cmake_minimum_required(VERSION 3.10)
project(VulkanCompute)

find_package(Vulkan REQUIRED)

# custom command to compile the shader

set(SHADER_SRC ${CMAKE_SOURCE_DIR}/vector_add.comp)
set(SHADER_SPV ${CMAKE_BINARY_DIR}/vector_add.comp.spv)
add_custom_command(
    OUTPUT ${SHADER_SPV}
    COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} -V ${SHADER_SRC} -o ${SHADER_SPV}
    DEPENDS ${SHADER_SRC}
    COMMENT "Compiling ${SHADER_SRC} to SPIR-V"
)

add_custom_target(
    compile_shaders ALL
    DEPENDS ${SHADER_SPV}
)

add_executable(VulkanCompute main.cpp)

add_dependencies(VulkanCompute compile_shaders)

target_include_directories(VulkanCompute PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(VulkanCompute PRIVATE ${Vulkan_LIBRARIES})
