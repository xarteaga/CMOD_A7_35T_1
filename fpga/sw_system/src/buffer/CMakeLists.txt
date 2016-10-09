# Check version
cmake_minimum_required (VERSION 3.5.1)

# Create Project
project (buffer C )

# Add sources
add_library(buffer
		buffer.h
		buffer.c)
		
# Include Directories
include_directories(buffer ../../sdk_workspace/bsp_0/microblaze_0/include)
include_directories(buffer ../scheduler)
include_directories(buffer ../config)
include_directories(buffer ../wifi_esp8266)
