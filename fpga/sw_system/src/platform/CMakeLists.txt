# Check version
cmake_minimum_required(VERSION 3.5.1)

# Create Project
project(platform C)

# Add sources
add_library(platform
        platform.c
        platform.h)

# Include Directories
include_directories(platform ../../sdk_workspace/bsp_0/microblaze_0/include)
include_directories(platform ../wifi_esp8266)
include_directories(platform ../scheduler)
include_directories(platform ../xadc)
