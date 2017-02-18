# Check version
cmake_minimum_required (VERSION 3.5.1)

# Create Project
project (sd C )

# Add sources
add_library(sd
		sd_spi.h
		sd_spi.c
		)

# Include Directories
include_directories(sd ../../sdk_workspace/bsp_0/microblaze_0/include)
