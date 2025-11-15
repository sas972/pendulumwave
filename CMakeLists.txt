cmake_minimum_required(VERSION 3.17)

project(PendulumWaveSim3 CXX)

# --- C++20 Configuration ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Apple M1/Silicon Specifics ---
if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
    # SFML 3 usually installs to standard paths, but if you installed 
    # via a specific prefix, ensure CMAKE_PREFIX_PATH is set.
endif()

# --- Find SFML 3.0 ---
# SFML 3 is strict about components.
find_package(SFML 3.0 COMPONENTS Graphics Window System REQUIRED)

# --- Build Target ---
add_executable(PendulumSim3 main.cpp)

# --- Link Libraries ---
# SFML 3 uses Namespaced targets (SFML::Graphics, etc.)
target_link_libraries(PendulumSim3 PRIVATE SFML::Graphics SFML::Window SFML::System)

# --- macOS .app Bundle ---
if(APPLE)
set_target_properties(PendulumSim3 PROPERTIES
    MACOSX_BUNDLE TRUE
    OUTPUT_NAME "Pendulum Wave 3"
)
endif()

# Copy resources if you had them (none needed for this procedurally generated demo)
# But we output to a clean bin folder
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")