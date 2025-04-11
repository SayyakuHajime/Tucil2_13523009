cmake_minimum_required(VERSION 3.10)
project(Tucil2_13523009)

set(CMAKE_CXX_STANDARD 14)

# Find required packages
find_package(Threads REQUIRED)

# Set directories
set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/src/external)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/include)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

# Include directories
include_directories(${EXTERNAL_DIR})
include_directories(${INCLUDE_DIR})

# Source files
set(SOURCES
        ${SRC_DIR}/ErrorMeasurement.cpp
        ${SRC_DIR}/GifGenerator.cpp
        ${SRC_DIR}/ImageProcessor.cpp
        ${SRC_DIR}/main.cpp
        ${SRC_DIR}/QuadTree.cpp
        ${SRC_DIR}/QuadTreeCompressor.cpp
        ${SRC_DIR}/Utils.cpp
)

# Headers
set(HEADERS
        ${INCLUDE_DIR}/ErrorMeasurement.hpp
        ${INCLUDE_DIR}/GifGenerator.hpp
        ${INCLUDE_DIR}/ImageProcessor.hpp
        ${INCLUDE_DIR}/QuadTree.hpp
        ${INCLUDE_DIR}/QuadTreeCompressor.hpp
        ${INCLUDE_DIR}/Utils.hpp
        ${EXTERNAL_DIR}/gif.hpp
        ${EXTERNAL_DIR}/stb_image.h
        ${EXTERNAL_DIR}/stb_image_write.h
)

# Define the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

# Set output directory
set_target_properties(${PROJECT_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Installation
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# Copy test images to bin directory (optional, uncomment if needed)
# file(COPY ${CMAKE_SOURCE_DIR}/test DESTINATION ${CMAKE_BINARY_DIR}/bin)