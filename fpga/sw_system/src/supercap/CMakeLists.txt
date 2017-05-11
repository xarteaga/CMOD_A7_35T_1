# Check version
cmake_minimum_required (VERSION 3.5.1)

# Create Project
project (supercap C )

# Add sources
add_library(supercap
		supercap.c
		supercap.h)

# Include Directories
include_directories(supercap ../../sdk_workspace/bsp_0/microblaze_0/include)
include_directories(supercap ../scheduler)
include_directories(supercap ../config)
include_directories(supercap ../leds)
include_directories(supercap ../platform)
include_directories(supercap ../xadc)
