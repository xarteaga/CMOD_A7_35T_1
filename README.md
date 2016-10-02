# CMOD_A7_35T_1
## Description
This repository creates a basic repository for the Digilent's CMOD A7 35T under Xilinx Vivado 2015.2 tools. The said
hardware generation should be performed quite straightforward using command line shell.

The TCL scripts generate a minimal design for a Microblaze softprocessor and a few peripherals such as USB debug
UART, Fixed Interval Timer and so on. In fact, all the hardware design is generated automatically using TCL scripts.

The HW design includes a extra UART peripheral for connecting a UART WiFi module (ESP8266) in the PMOD (JA). Indeed,
the Rx is connected to the pin JA0 and the Tx is connected to the JA1.

Note: I installed Xilinx's Vivado tools under '/opt/Xilinx/'. If you installed Xilinx's tools under another isntallation
path, you may have to change a few paths defined in the scripts.


## Software Requirements
- Ubuntu (currently running on 16.04 LTS)
- Xilinx Vivado 2015.2 and SDK 2015.2
- Valid Xilinx license
- All other required software (cmake, automake and so on)

## Generation Instructions
The following script indicates the steps of the complete flow for generating, compile all the FPGA hardware and software.
```
#!/bin/sh
# 1. Change directory to the hardware system creation:
git clone https://github.com/xarteaga/CMOD_A7_35T_1.git
cd fpga/hw_system/
# 2. Create hardware system, synthesize, implement and export
make
# 3. Create SDK workspace (BSP, HW and Hello world)
make create_workspace
# 4. Change directory to sowftare
cd ../sw_system
# 5. Create build folder and change directory
mkdir build
cd build
# 6. Generate compilation scripts
cmake ..
# 7. Compile sowftware
make
# 7. Download FPGA
make download
# 8. Run Software
make run
```

## Useful Links
- [Xilinx CMOD A7 35T board Overview](https://www.xilinx.com/products/boards-and-kits/1-f3zdsm.html)
- [Digilent CMOD A7 35T board documentation](https://reference.digilentinc.com/reference/programmable-logic/cmod-a7/start)
- [Vivado 2015.2 TCL Commands](http://www.xilinx.com/support/documentation/sw_manuals/xilinx2015_2/ug835-vivado-tcl-commands.pdf)
- [Xilinx System Debugger CLI (XSDB) commands](http://www.xilinx.com/support/documentation/sw_manuals/xilinx2014_3/SDK_Doc/concepts/sdk_c_xsd_xsdb_commands.htm#50655646_75465)
