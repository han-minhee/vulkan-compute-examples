cmake_minimum_required(VERSION 3.10)
project(VulkanCompute)

find_package(Vulkan REQUIRED)

# custom command to compile the shader
set(SHADER_SRC ${CMAKE_SOURCE_DIR}/kernels/vector_add.comp)
set(SHADER_SPV ${CMAKE_BINARY_DIR}/kernels/vector_add.comp.spv)
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

# source files
set(SRC_FILES
    src/main.cpp
    src/vk_buffer.cpp
    src/vk_command.cpp
    src/vk_descriptor.cpp
    src/vk_device.cpp
    src/vk_instance.cpp
    src/vk_pipeline.cpp
    src/vk_utils.cpp
)

# header files
set(HEADER_FILES
    include/vk_buffer.hpp
    include/vk_command.hpp
    include/vk_descriptor.hpp
    include/vk_device.hpp
    include/vk_instance.hpp
    include/vk_pipeline.hpp
    include/vk_utils.hpp
)

add_executable(VulkanCompute ${SRC_FILES} ${HEADER_FILES})

add_dependencies(VulkanCompute compile_shaders)

target_include_directories(VulkanCompute PRIVATE ${Vulkan_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(VulkanCompute PRIVATE ${Vulkan_LIBRARIES})

