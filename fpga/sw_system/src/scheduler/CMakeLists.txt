# Check version
cmake_minimum_required (VERSION 3.5.1)

# Create Project
project (scheduler C )

# Add sources
add_library(scheduler
		scheduler.h
		scheduler.c
		)

# Include Directories
include_directories(scheduler ../../sdk_workspace/bsp_0/microblaze_0/include
		../config)
