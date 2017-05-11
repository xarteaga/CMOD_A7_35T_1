# Check version
cmake_minimum_required (VERSION 3.5.1)

# Create Project
project (rtc C )

# Add sources
add_library(rtc
		rtc_ds1302.h
		rtc_ds1302.c
        rtc.h
		rtc.c
		)

# Include Directories
include_directories(rtc ../../sdk_workspace/bsp_0/microblaze_0/include)
include_directories(rtc ../dinouts)
include_directories(rtc ../scheduler)
include_directories(rtc ../config)
include_directories(rtc ../platform)
