# Check version
cmake_minimum_required (VERSION 3.5.1)

# Create Project
project (http_client C )

# Add sources
add_library(http_client
		http_client.h
		http_client.c)
		
# Include Directories
include_directories(http_client ../../sdk_workspace/bsp_0/microblaze_0/include)
include_directories(http_client ../scheduler)
include_directories(http_client ../config)
include_directories(http_client ../wifi_esp8266)
include_directories(http_client ../buffer)
