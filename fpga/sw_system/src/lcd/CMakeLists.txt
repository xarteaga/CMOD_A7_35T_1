# Check version
cmake_minimum_required (VERSION 3.5.1)

# Create Project
project (lcd C )

# Add sources
add_library(lcd
		lcd.c
		lcd.h)

# Include Directories
include_directories(lcd ../../sdk_workspace/bsp_0/microblaze_0/include)
include_directories(lcd ../config)
include_directories(lcd ../platform)
include_directories(lcd ../scheduler)
include_directories(lcd ../rtc)

