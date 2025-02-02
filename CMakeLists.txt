# Minimum version of CMake required
cmake_minimum_required(VERSION 3.15 FATAL_ERROR)  # Update to 3.15+ for modern CMake features

# Project name
project(OMSim)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_INSTALL_RPATH "/opt/homebrew/geant4-v11.2.2-install/lib")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

# Debug information
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

# Option to build example with Geant4 UI and Vis drivers
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)

# Find Geant4 and set required variables
if(WITH_GEANT4_UIVIS)
    find_package(Geant4 REQUIRED ui_all vis_all)
else()
    find_package(Geant4 REQUIRED)
endif()

# Add Boost global placeholder definition
add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

# Set RPATH for runtime linkage
set(CMAKE_INSTALL_RPATH "/opt/homebrew/geant4-v11.2.2-install/lib")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

# Prefer Homebrew for dependencies
list(APPEND CMAKE_PREFIX_PATH /opt/homebrew/)

# Find fmt and spdlog
find_package(fmt REQUIRED HINTS /opt/homebrew/lib/cmake/fmt)
find_package(spdlog REQUIRED)

# Ensure spdlog uses the external fmt library
add_definitions(-DSPDLOG_FMT_EXTERNAL)

# Find Boost library and specify which components to use
find_package(Boost REQUIRED COMPONENTS program_options)

# Set paths for OpenSSL
set(OPENSSL_ROOT_DIR /opt/homebrew/opt/openssl@3)
set(OPENSSL_LIBRARIES /opt/homebrew/opt/openssl@3/lib)
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

# Include Geant4 headers
include(${Geant4_USE_FILE})

# Find ROOT and include its headers and libraries
find_package(ROOT REQUIRED)
include_directories(${ROOT_INCLUDE_DIRS})
list(APPEND COMMON_LIBRARIES ${ROOT_LIBRARIES})

# Add Geant4 library path for runtime linkage
list(APPEND COMMON_LIBRARIES -Wl,-rpath,/opt/homebrew/geant4-v11.2.2-install/lib)

# Include ROOT macros and set them up
if(ROOT_FOUND)
    include(${ROOT_USE_FILE})
endif()

# Define a variable with all the libraries
set(COMMON_LIBRARIES 
    ${Geant4_LIBRARIES}
    ${ROOT_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    Boost::program_options
    spdlog::spdlog
    fmt::fmt
    /opt/homebrew/geant4-v11.2.2-install/lib/libG4RayTracer.dylib
    $<$<BOOL:${MINGW}>:ws2_32>
)

# Make this variable available to subdirectories
set(COMMON_LIBRARIES ${COMMON_LIBRARIES} CACHE INTERNAL "")

# Include the subdirectories
add_subdirectory(common)
add_subdirectory(simulations)
