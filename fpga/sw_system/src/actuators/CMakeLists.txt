# Check version
cmake_minimum_required (VERSION 3.5.1)

# Create Project
project (actuators C )

# Add sources
add_library(actuators
		actuators.c
		actuators.h)

# Include Directories
include_directories(actuators ../../sdk_workspace/bsp_0/microblaze_0/include)
include_directories(actuators ../scheduler)
include_directories(actuators ../config)
include_directories(actuators ../platform)
include_directories(actuators ../dinouts)