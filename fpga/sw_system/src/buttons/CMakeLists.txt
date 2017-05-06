# Check version
cmake_minimum_required (VERSION 3.5.1)

# Create Project
project (buttons C )

# Add sources
add_library(buttons
		buttons.c
		buttons.h)

# Include Directories
include_directories(buttons ../../sdk_workspace/bsp_0/microblaze_0/include)
include_directories(buttons ../scheduler)
include_directories(buttons ../config)
include_directories(buttons ../platform)
include_directories(buttons ../dinouts)