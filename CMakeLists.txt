cmake_minimum_required(VERSION 3.10)
project(rocket)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)

# Find Vulkan and GLFW
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)


 #Add imgui library target
add_library(imgui STATIC
        imgui/imgui.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_tables.cpp
        imgui/backend/imgui_impl_glfw.cpp
        imgui/backend/imgui_impl_vulkan.cpp
        )

target_include_directories(imgui PUBLIC ./imgui ./imgui/backend)

# Add include directories
include_directories(${VULKAN_INCLUDE_DIR} ${GLFW_INCLUDE_DIR})

#file(GLOB SOURCES "*.cpp" "*.hpp" "./imgui/*.cpp" "./imgui/*.h" "./imgui/backend/*.cpp" "./imgui/backend/*.h")
file(GLOB SOURCES "*.cpp" "*.hpp")

# Link against Vulkan and GLFW
target_link_libraries(imgui PUBLIC ${Vulkan_LIBRARIES} glfw vulkan)


# Add executable
add_executable(rocket ${SOURCES})

# Link against Vulkan and GLFW
target_link_libraries(rocket ${Vulkan_LIBRARIES} vulkan glfw imgui)



# Set compiler flags
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic")
