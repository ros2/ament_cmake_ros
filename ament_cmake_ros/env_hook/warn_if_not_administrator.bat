net session >nul 2>&1
if NOT %errorLevel% == 0 (
    echo =======
    echo Warning: ROS 2 executables require administrator permissions on Windows.
    echo Programs may appear to work correctly, but might not be able communicate with other nodes.
    echo =======
)
