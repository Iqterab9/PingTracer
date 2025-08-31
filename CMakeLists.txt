cmake_minimum_required(VERSION 3.16)
project(PingTracer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

# Set Qt6 specific settings
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Include directories
include_directories(src)

# Source files
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/pingtracer.cpp
    src/networktester.cpp
    src/thememanager.cpp
    src/exportmanager.cpp
)

# Header files
set(HEADERS
    src/mainwindow.h
    src/pingtracer.h
    src/networktester.h
    src/thememanager.h
    src/exportmanager.h
)

# UI files
set(UI_FILES
    ui/mainwindow.ui
)

# Resource files
set(RESOURCES
    resources/resources.qrc
)

# Create executable
add_executable(PingTracer ${SOURCES} ${HEADERS} ${UI_FILES} ${RESOURCES})

# Link Qt6 libraries
target_link_libraries(PingTracer Qt6::Core Qt6::Widgets Qt6::Network)

# Platform specific libraries
if(WIN32)
    target_link_libraries(PingTracer ws2_32 iphlpapi)
elseif(UNIX)
    target_link_libraries(PingTracer pthread)
endif()

# Set application properties
set_target_properties(PingTracer PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# Installation
install(TARGETS PingTracer
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)