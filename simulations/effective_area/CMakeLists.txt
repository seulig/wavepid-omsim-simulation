# Create lists of source files
file(GLOB EFFECTIVE_AREA_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc")

# Create an executable
add_executable(OMSim_effective_area 
    ${CMAKE_CURRENT_SOURCE_DIR}/OMSim_effective_area.cc 
    ${COMMON_SOURCES} 
    ${EFFECTIVE_AREA_SOURCES} 
)

# Specify include directories for this executable
target_include_directories(OMSim_effective_area PUBLIC
    ${PROJECT_SOURCE_DIR}/common/framework/include
    ${PROJECT_SOURCE_DIR}/common/geometry_construction/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link the libraries
target_link_libraries(OMSim_effective_area
    PRIVATE
       fmt::fmt
       spdlog::spdlog
       ${COMMON_LIBRARIES}
)
