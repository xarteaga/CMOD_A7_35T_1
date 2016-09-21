#!/usr/bin/tclsh
# 0) Delete previous existance of workspace
file delete -force ../sw_system/sdk_workspace

# 1) Set Workspace
sdk set_workspace ../sw_system/sdk_workspace

# 2) Create HW project
sdk create_hw_project -name hw_0 -hwspec output/hw_system.hdf

# 3) Create System BSP
sdk create_bsp_project -name bsp_0 -hwproject hw_0 -mss src/system.mss

# 4) Create Bootloader BSP
sdk create_bsp_project -name srec_spi_bootloader_bsp -hwproject hw_0 -mss src/bootloader_bsp.mss

# 5) Create Hello World Project
sdk create_app_project -name helloworld -hwproject hw_0 -proc microblaze_0 -os standalone -lang C -app {Hello World} -bsp bsp_0

# 6) Create Bootloader application
sdk create_app_project -name srec_spi_bootloader -hwproject hw_0 -proc microblaze_0 -os standalone -lang C -app {SREC SPI Bootloader} -bsp srec_spi_bootloader_bsp

# 7) Modify bootloader pointer
exec echo "#undef FLASH_IMAGE_BASEADDR" >> ../sw_system/sdk_workspace/srec_spi_bootloader/src/blconfig.h
exec echo "#define FLASH_IMAGE_BASEADDR 0x400000" >> ../sw_system/sdk_workspace/srec_spi_bootloader/src/blconfig.h

# 8) Compile all projects
sdk build_project bsp_0
sdk build_project srec_spi_bootloader_bsp
sdk build_project helloworld
sdk build_project srec_spi_bootloader
exit
