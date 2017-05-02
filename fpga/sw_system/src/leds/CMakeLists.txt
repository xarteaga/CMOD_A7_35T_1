# Check version
cmake_minimum_required (VERSION 3.5.1)

# Create Project
project (leds C )

# Add sources
add_library(leds
		leds.c
		leds.h)

# Include Directories
include_directories(leds ../../sdk_workspace/bsp_0/microblaze_0/include)
include_directories(leds ../scheduler)
include_directories(leds ../config)
include_directories(leds ../platform)