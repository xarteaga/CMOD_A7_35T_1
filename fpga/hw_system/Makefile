########################################################################################################################
#                                                XILINX'S EXECS
########################################################################################################################
VIVADO = vivado -mode batch
XSDK = xsdk -batch

# Paths
LOG_PATH = output/logs

########################################################################################################################
#                                              VIVADO / XSDK SCRIPTS
########################################################################################################################
CREATE_HW_SYSTEM = scripts/create_hw_system.tcl
IMPLEMENT_HW_SYSTEM = scripts/implement_hw_system.tcl
EXPORT_HW_SYSTEM = scripts/export_hw_system.tcl
CREATE_WORKSPACE = scripts/create_workspace.tcl
ADD_BOOTLOADER = scripts/add_bootloader.tcl

########################################################################################################################
#                                               MAIN TARGETS PATHS
########################################################################################################################
HW_PROJECT = hw_system/hw_system.xpr
HW_BITSTREAM = output/hw_system.bit
HW_DEF_SDK = output/hw_system.hdf
HW_BOOTLOADER = ../sw_system/sdk_workspace/srec_spi_bootloader/Release/srec_spi_bootloader.elf
HW_FINAL_BITSTREAM = output/hw_system_bootloader.bit

########################################################################################################################
#                                             MAIN TARGETS DEFINITIONS
########################################################################################################################
# 0) Create ouput directory
$(LOG_PATH):
	mkdir -p output/logs

# 1 ) Create HW system
$(HW_PROJECT): $(CREATE_HW_SYSTEM) src/hw_system_bd.tcl src/system.xdc
	@rm -f *.jou *.log *.str
	@rm -f -r hw_system
	@$(VIVADO) -source $(CREATE_HW_SYSTEM) -log $(LOG_PATH)/$(basename $(CREATE_HW_SYSTEM)).log

# 2) Add Bootloader and implement
$(HW_BITSTREAM): $(HW_PROJECT) $(IMPLEMENT_HW_SYSTEM)
	@rm -f *.jou *.log *.str
	@$(VIVADO) -source $(IMPLEMENT_HW_SYSTEM) -log $(LOG_PATH)/$(basename $(IMPLEMENT_HW_SYSTEM)).log

# 3 ) Export Hardware
$(HW_DEF_SDK): $(HW_BITSTREAM) $(EXPORT_HW_SYSTEM)
	@rm -f *.jou *.log *.str
	@$(VIVADO) -source $(EXPORT_HW_SYSTEM) -log $(LOG_PATH)/$(basename $(EXPORT_HW_SYSTEM)).log

# 4 ) Create SDK Workspace and Bootloader
$(HW_BOOTLOADER): $(HW_DEF_SDK) $(CREATE_WORKSPACE)
	@rm -f *.jou *.log *.str
	@$(XSDK) -source $(CREATE_WORKSPACE) -log $(LOG_PATH)/$(basename $(CREATE_WORKSPACE)).log

# 5 ) Associate BOOTLOADER executable to microblaze and implement again
$(HW_FINAL_BITSTREAM): $(HW_BOOTLOADER) $(ADD_BOOTLOADER)
	@rm -f *.jou *.log *.str
	@$(XSDK) -source $(ADD_BOOTLOADER) -log $(LOG_PATH)/$(basename $(ADD_BOOTLOADER)).log

########################################################################################################################
#                                           TARGETS ALIAS DEFINITIONS
########################################################################################################################
all: $(HW_FINAL_BITSTREAM)
	@echo 'Full System implemented!'

create_system: $(HW_PROJECT)

implement_system: $(HW_BITSREAM)

create_workspace: $(HW_BOOTLOADER)

create_folders: $(LOG_PATH)

export_hardware: $(HW_DEF_SDK)


clean:
	@rm -f -r hw_system $(LOG_PATH) output *.jou *.log
	@echo "Project Cleaned"
