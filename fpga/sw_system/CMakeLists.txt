# Check version
cmake_minimum_required (VERSION 3.5.1)

# Set paths
set(XILINX_SDK_PATH "/opt/Xilinx/SDK/2015.2/")
set(SDK_WORKSPACE_PATH "${CMAKE_SOURCE_DIR}/sdk_workspace")
set(XILINX_COMPILERS_PATH "${XILINX_SDK_PATH}/gnu/microblaze/lin/bin/")
set(XILINX_XSDK "${XILINX_SDK_PATH}/bin/xsdk")
set(XILINX_XSDB "${XILINX_SDK_PATH}/bin/xsdb")
set(XILINX_DATA2MEM "${XILINX_SDK_PATH}/bin/data2mem")

# Compiler suite specification
set(CMAKE_AR "${XILINX_COMPILERS_PATH}/mb-ar")
set(CMAKE_C_COMPILER "${XILINX_COMPILERS_PATH}/mb-cc")
set(CMAKE_CXX_COMPILER "${XILINX_COMPILERS_PATH}/mb-c++")
set(CMAKE_OBJCOPY "${XILINX_COMPILERS_PATH}/mb-objcopy")
set(CMAKE_OBJDUMP "${XILINX_COMPILERS_PATH}/mb-objdump")
set(CMAKE_RANLIB "${XILINX_COMPILERS_PATH}/mb-ranlib")
set(CMAKE_LINKER "${XILINX_COMPILERS_PATH}/mb-ld")


# Set compiler and linker flags
#set(CMAKE_C_FLAGS "-Wl,-T -Wl,../src/lscript.ld")
#set(CMAKE_CXX_FLAGS "-Wl,-T -Wl,../src/lscript.ld")

# Compiler flags
add_compile_options(-Wall -O2 -c -fmessage-length=0 -mlittle-endian -mcpu=v9.5 -mxl-soft-mul -Wl,--no-relax -MMD -MP)

# Define linker script
set(CMAKE_LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/scripts/lscript.ld")

# Define Block Memory Map file
set(BLOCK_MEMORY_MAP "${CMAKE_SOURCE_DIR}/../hw_system/hw_system/hw_system.srcs/sources_1/bd/system/system.bmm")

#define Bitfile
set(BITFILE "${CMAKE_SOURCE_DIR}/../hw_system/output/hw_system.bit")

#define Bootloader
set(BOOTLOADER_ELF "${CMAKE_SOURCE_DIR}/sdk_workspace/srec_spi_bootloader/Debug/srec_spi_bootloader.elf")

# Set Linker arguments
set(CMAKE_LINKER_FLAGS "-Wl,-T -Wl,${CMAKE_LINKER_SCRIPT}")
set(CMAKE_LINKER_LIB_DIR "-L${SDK_WORKSPACE_PATH}/bsp_0/microblaze_0/lib")
set(CMAKE_LINKER_OPTIONS "-mlittle-endian -mcpu=v9.5 -mxl-soft-mul -Wl,--no-relax")
set(CMAKE_LINKER_LIBS "-Wl,--start-group,-lxil,-lgcc,-lc,--end-group -Wl,--start-group,-lxil,-lgcc,-lc,--end-group")

# Set compiler options
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

# Add subdirectories
add_subdirectory(src/platform)

project (sw_system C )

# Add headers as source
file(GLOB BSP_HEADERS "${CMAKE_SOURCE_DIR}/sdk_workspace/bsp_0/microblaze_0/include/*.h")

# Add sources
add_executable(sw_system
		src/main.c
		${BSP_HEADERS}
		src/main.c)

# Directories
include_directories(sw_system
		sdk_workspace/bsp_0/microblaze_0/include
		src/platform
		src
	)

# Add libraries
target_link_libraries(sw_system sw_platform)
link_directories(sw_system sdk_workspace/bsp_0/microblaze_0/lib)

# Setup linker target
set(CMAKE_C_LINK_EXECUTABLE "${CMAKE_C_COMPILER} ${CMAKE_LINKER_FLAGS} ${CMAKE_LINKER_LIB_DIR} ${CMAKE_LINKER_OPTIONS} <OBJECTS> -o <TARGET> ${CMAKE_LINKER_LIBS} <LINK_LIBRARIES>")

# Setup download command
add_custom_target(download ${XILINX_XSDB} -s ${CMAKE_SOURCE_DIR}/scripts/download_bitfile.tcl ${BITFILE})

# Run
add_custom_target(run ${XILINX_XSDB} -s ${CMAKE_SOURCE_DIR}/scripts/run_elf.tcl ${CMAKE_BINARY_DIR}/sw_system)

# Permanent
add_custom_target(permanent ${CMAKE_SOURCE_DIR}/scripts/write_rom.sh ${XILINX_DATA2MEM} ${BLOCK_MEMORY_MAP} ${BOOTLOADER_ELF} ${BITFILE})

