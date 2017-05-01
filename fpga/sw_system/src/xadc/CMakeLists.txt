# Check version
cmake_minimum_required (VERSION 3.5.1)

# Create Project
project (xadc C )

# Add sources
add_library(xadc
		xadc.c
		xadc.h)

# Include Directories
include_directories(xadc ../../sdk_workspace/bsp_0/microblaze_0/include)
include_directories(xadc ../scheduler)
include_directories(xadc ../config)
include_directories(xadc ../platform)
