# Create lists of source files
file(GLOB WAVEPID_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc")

# Create an executable
add_executable(OMSim_wavePID 
    ${CMAKE_CURRENT_SOURCE_DIR}/OMSim_wavePID.cc 
    ${COMMON_SOURCES} 
    ${WAVEPID_SOURCES} 
)

# Specify include directories for this executable
target_include_directories(OMSim_wavePID PUBLIC
    ${PROJECT_SOURCE_DIR}/common/framework/include
    ${PROJECT_SOURCE_DIR}/common/geometry_construction/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link the libraries
target_link_libraries(OMSim_wavePID
    PRIVATE
       fmt::fmt
       spdlog::spdlog
       ${COMMON_LIBRARIES}
)
