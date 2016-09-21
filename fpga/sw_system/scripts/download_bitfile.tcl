# 0) Check arduments
if {$argc != 1} {
    puts "ERROR! Wrong number of arguments!"
    exit
}

# 1) Connect to Hardware server
connect

# 2) Select target
target 1

# 3) Download FPGA
fpga $argv

# 4) Disconnect HW
disconnect

# 5) Exit
exit