# Check version
cmake_minimum_required (VERSION 3.5.1)

# Create Project
project (wifi_esp8266 C )

# Add sources
add_library(wifi_esp8266
		wifi_esp8266.c
		wifi_esp8266.h
		wifi_uart.c
		wifi_uart.h
		wifi_openat.c
		wifi_openat.h
		wifi_utils.c
		wifi_utils.h)

# Include Directories
include_directories(wifi_esp8266 ../../sdk_workspace/bsp_0/microblaze_0/include)
include_directories(wifi_esp8266 ../scheduler)
include_directories(wifi_esp8266 ../config)
include_directories(wifi_esp8266 ../buffer)
