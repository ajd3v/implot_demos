cmake_minimum_required(VERSION 3.12)
project(TradingInterface LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add executable and include directory for local headers
add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE .)

# --- ImGui Integration ---
# Assumes ImGui source code is located in the 'libs/imgui' subdirectory.
# If ImGui has its own CMakeLists.txt, using add_subdirectory(libs/imgui) might be an option,
# but this example explicitly lists sources for clarity and control.
set(IMGUI_SOURCES
    libs/imgui/imgui.cpp
    libs/imgui/imgui_draw.cpp
    libs/imgui/imgui_tables.cpp
    libs/imgui/imgui_widgets.cpp
    # Backend files: GLFW and OpenGL3
    # Ensure these paths match your ImGui backend file locations.
    libs/imgui/backends/imgui_impl_glfw.cpp
    libs/imgui/backends/imgui_impl_opengl3.cpp
)

add_library(imgui_lib STATIC ${IMGUI_SOURCES})
target_include_directories(imgui_lib PUBLIC 
    libs/imgui # For imgui.h
    libs/imgui/backends # For imgui_impl_glfw.h, imgui_impl_opengl3.h
)
# Link ImGui to the main project
target_link_libraries(${PROJECT_NAME} PRIVATE imgui_lib)


# --- ImPlot Integration ---
# Assumes ImPlot source code is located in the 'libs/implot' subdirectory.
set(IMPLOT_SOURCES
    libs/implot/implot.cpp
    libs/implot/implot_items.cpp
    # implot_demo.cpp is typically not needed for the library itself
    # Add implot_widgets.cpp if it exists and is used in your version of ImPlot
)

add_library(implot_lib STATIC ${IMPLOT_SOURCES})
target_include_directories(implot_lib PUBLIC libs/implot) # For implot.h

# ImPlot depends on ImGui. Ensure ImPlot's library links against ImGui's library.
# This is important if building them as separate static libraries as done here.
target_link_libraries(implot_lib PUBLIC imgui_lib)

# Link ImPlot to the main project
target_link_libraries(${PROJECT_NAME} PRIVATE implot_lib)


# --- System Dependencies (GLFW and OpenGL) ---
# These are common dependencies for ImGui applications.
# You may need to install these libraries on your system.
# For example, on Debian/Ubuntu: sudo apt-get install libglfw3-dev libopengl-dev
# On macOS (using Homebrew): brew install glfw

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)

# Some ImGui OpenGL3 backends might use an OpenGL loader like GLAD or GLEW.
# If your imgui_impl_opengl3.cpp directly includes glad/glad.h or GL/glew.h,
# you might need to include and link it here.
# Often, simple examples bundle a small GL loader (like glad generated from a web service)
# or the ImGui backend handles it. If not, you might need:
# find_package(GLEW REQUIRED) # If using GLEW
# target_link_libraries(${PROJECT_NAME} PRIVATE GLEW::GLEW)
# Or for GLAD, you would add its source file to your project or ImGui library.

target_link_libraries(${PROJECT_NAME} PRIVATE glfw OpenGL::GL)
# Note: On some systems, OpenGL might be linked differently, e.g., just '-lGL' via CMAKE_EXE_LINKER_FLAGS
# or through specific variables from find_package(OpenGL). 'OpenGL::GL' is common with modern CMake.

# --- Final Comments ---
# Review all paths, especially for ImGui and ImPlot sources and includes.
# Ensure system dependencies (GLFW, OpenGL, and any GL loader) are correctly installed and found.
# This CMakeLists.txt provides a basic structure. More complex projects might require
# further organization, options, or platform-specific configurations.
