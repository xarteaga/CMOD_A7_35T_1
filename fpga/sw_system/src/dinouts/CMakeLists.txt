# Check version
cmake_minimum_required (VERSION 3.5.1)

# Create Project
project (dinouts C )

# Add sources
add_library(dinouts
		dinouts.c
		dinouts.h)

# Include Directories
include_directories(dinouts ../../sdk_workspace/bsp_0/microblaze_0/include)
include_directories(dinouts ../scheduler)
include_directories(dinouts ../config)
include_directories(dinouts ../platform)
