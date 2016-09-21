# 1) Open Project
open_project hw_system/hw_system.xpr

# 2) Check the state of synthesis and synthetize if it is required
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

# 3) Implementation
if {([get_property NEEDS_REFRESH [get_runs impl_1]] == "1") || ([get_property PROGRESS [get_runs impl_1]] != "100%") \
} then { \
	[launch_runs impl_1]
	[wait_on_run impl_1]}
if {([get_property NEEDS_REFRESH [get_runs impl_1]] == "1") || ([get_property PROGRESS [get_runs impl_1]] != "100%") \
} then { \
	puts "ERROR: Implementation Failed."
	exit 1
}

# 4) Open implementation and generate bitstream
open_run impl_1
write_bitstream -force -bin_file output/hw_system.bit
