# Check version
cmake_minimum_required (VERSION 3.5.1)

# Create Project
project (irproximity C )

# Add sources
add_library(irproximity
		irproximity.c
		irproximity.h)

# Include Directories
include_directories(irproximity ../../sdk_workspace/bsp_0/microblaze_0/include)
include_directories(irproximity ../config)
include_directories(irproximity ../dinouts)
include_directories(irproximity ../platform)
include_directories(irproximity ../scheduler)
include_directories(dinouts ../xadc)
