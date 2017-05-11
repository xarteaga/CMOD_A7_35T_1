# Check version
cmake_minimum_required (VERSION 3.5.1)

# Create Project
project (happypills C )

# Add sources
add_library(happypills
		happypills.c
		happypills.h happypills_hmi.c happypills_hmi.h happypills_controller.c happypills_controller.h)

# Include Directories
include_directories(happypills ../../sdk_workspace/bsp_0/microblaze_0/include)
include_directories(happypills ../actuators)
include_directories(happypills ../buttons)
include_directories(happypills ../config)
include_directories(happypills ../dinouts)
include_directories(happypills ../irproximity)
include_directories(happypills ../lcd)
include_directories(happypills ../platform)
include_directories(happypills ../rtc)
include_directories(happypills ../scheduler)
