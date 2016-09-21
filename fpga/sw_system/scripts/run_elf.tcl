# 0) Check arduments
if {$argc != 1} {
    puts "ERROR! Wrong number of arguments!"
    exit
}

# 1) Connect to Hardware server
connect

# 2) Select target
target 3

# 3) Stop running
stop

# 4) Download executable
dow $argv

# 5) Start running
con

# 6) Disconnect HW
disconnect

# 7) Exit
exit