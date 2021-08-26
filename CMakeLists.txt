cmake_minimum_required(VERSION 3.0.0)
project(Vulkan_Test_1 VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 20)

include(CTest)
enable_testing()

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(Vulkan_Test_1 main.cpp ${SOURCES})
target_include_directories(Vulkan_Test_1 PUBLIC "src" "external")
target_include_directories(Vulkan_Test_1 PUBLIC "C:/VulkanSDK/1.2.182.0/Include")
file(GLOB_RECURSE LIBS "C:/VulkanSDK/1.2.182.0/Lib/*.lib")
target_link_libraries(Vulkan_Test_1 ${LIBS})
# target_link_libraries(Vulkan_Test_1 "glfw3.lib")

add_custom_target(myCustomTarget COMMAND cd ../shaders && @echo on && compile.bat && echo Compiled Shaders.)
add_dependencies(Vulkan_Test_1 myCustomTarget)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)