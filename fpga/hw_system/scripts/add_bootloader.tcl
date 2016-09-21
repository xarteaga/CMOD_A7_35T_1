# 1) Open Project
open_project hw_system/hw_system.xpr

# 2) Add bootloader executable
add_files ../sw_system/sdk_workspace/srec_spi_bootloader/Release/srec_spi_bootloader.elf

# 3) Associate ELF
set_property SCOPED_TO_REF system [get_files -all -of_objects [get_fileset sources_1] {../sw_system/sdk_workspace/srec_spi_bootloader/Release/srec_spi_bootloader.elf}]
set_property SCOPED_TO_CELLS { microblaze_0 } [get_files -all -of_objects [get_fileset sources_1] {../sw_system/sdk_workspace/srec_spi_bootloader/Release/srec_spi_bootloader.elf}]

# 4) Check the state of synthesis and synthetize if it is required
if {([get_property NEEDS_REFRESH [get_runs synth_1]] == "1") || ([get_property PROGRESS [get_runs synth_1]] != "100%") \
} then { \
	[reset_run synth_1]
	[launch_runs synth_1]
	[wait_on_run synth_1]}
if {([get_property NEEDS_REFRESH [get_runs synth_1]] == "1") || ([get_property PROGRESS [get_runs synth_1]] != "100%") \
} then { \
	puts "ERROR: Synthesis Failed."
	exit 1
}

# 5) Implementation
if {([get_property NEEDS_REFRESH [get_runs impl_1]] == "1") || ([get_property PROGRESS [get_runs impl_1]] != "100%") \
} then { \
	[launch_runs impl_1]
	[wait_on_run impl_1]}
if {([get_property NEEDS_REFRESH [get_runs impl_1]] == "1") || ([get_property PROGRESS [get_runs impl_1]] != "100%") \
} then { \
	puts "ERROR: Implementation Failed."
	exit 1
}

# 6) Open implementation and generate bitstream
open_run impl_1
write_bitstream -force -bin_file output/hw_system_bootloader.bit
