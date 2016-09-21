
# Set the reference directory for source file relative paths (by default the value is script directory path)
set origin_dir "."

# Set the directory path for the original project from where this script was exported
set orig_proj_dir "[file normalize "$origin_dir"]"

# Create project
create_project hw_system -force ./hw_system -part xc7a35ticsg324-1L

# Set the directory path for the new project
set proj_dir [get_property directory [current_project]]

# Set project properties
set obj [get_projects hw_system]
set_property board_part digilentinc.com:Arty:1.1 [current_project]

# Set the custom IP repository folder location to ./ip_repo
set_param bd.enableIpSharedDirectory true
set repos_local "./ip_repo"
set_property ip_repo_paths  "${repos_local}" [current_fileset]
update_ip_catalog -rebuild

# Build the MicroBlaze processor system block design
source ./src/hw_system_bd.tcl
generate_target  {synthesis simulation implementation}  [get_files  ./hw_system/hw_system.srcs/sources_1/bd/design_1/design_1.bd]

# Generate the top-level HDL wrapper for the block design
make_wrapper -files [get_files ./hw_system/hw_system.srcs/sources_1/bd/system/system.bd] -top
add_files -norecurse ./hw_system/hw_system.srcs/sources_1/bd/system/hdl/system_wrapper.v
update_compile_order -fileset sources_1
update_compile_order -fileset sim_1

# Add the design_1.xdc constraints file to the design
add_files -fileset constrs_1 -norecurse ./src/system.xdc
import_files -fileset constrs_1 ./src/system.xdc

# Tell Vivado to work harder to meet the timing constraints
#set_property STEPS.PHYS_OPT_DESIGN.IS_ENABLED true [get_runs impl_1]
#set_property STEPS.POST_ROUTE_PHYS_OPT_DESIGN.IS_ENABLED true [get_runs impl_1]

#tpc reset_run synth_1
#tpc launch_runs synth_1
#tpc wait_on_run synth_1
#reset_run impl_1
#tpc launch_runs impl_1
#tpc wait_on_run impl_1
#tpc launch_runs impl_1 -to_step write_bitstream
#tpc wait_on_run impl_1
