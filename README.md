# astro_hardware_interface

ROS 2 `ros2_control` hardware interface for the **Astro** differential-drive mobile robot.
Handles serial communication with a Teensy microcontroller and exposes velocity command and position/velocity state interfaces for both wheels.

---

## Prerequisites

- ROS 2 Humble
- `colcon` build tool
- `libserial-dev`

```bash
sudo apt install libserial-dev
```

Install ROS 2 dependencies:

```bash
sudo apt install \
  ros-humble-ros2-control \
  ros-humble-ros2-controllers \
  ros-humble-controller-manager \
  ros-humble-robot-state-publisher \
  ros-humble-joint-state-broadcaster \
  ros-humble-diff-drive-controller \
  ros-humble-rviz2
```

---

## Clone & Build

```bash
# Create workspace (skip if you already have one)
mkdir -p ~/astro_hw_ws/src
cd ~/astro_hw_ws/src

# Clone the repository
git clone <repository-url> astro_hardware_interface

# Install remaining ROS dependencies
cd ~/astro_hw_ws
rosdep install --from-paths src --ignore-src -r -y

# Build
colcon build --packages-select astro_hardware_interface

# Source the workspace
source install/setup.bash
```

---

## Hardware Setup

- Connect the Teensy to the PC via USB. The default serial device is `/dev/ttyACM0` at 115200 baud.
- To use a different port, edit `description/ros2_control/astro.ros2_control.xacro`.
- Grant serial port access if needed:

```bash
sudo usermod -aG dialout $USER
# Log out and back in for the change to take effect
```

---

## Usage

### Visualize the robot model (no hardware required)

```bash
ros2 launch astro_hardware_interface view_robot.launch.py
```

### Full bringup (hardware + controllers + RViz)

```bash
ros2 launch astro_hardware_interface astro_hardware.launch.py
```

This starts:
- `ros2_control_node` with the Astro hardware interface
- `robot_state_publisher`
- `joint_state_broadcaster`
- `astro_diff_drive_controller`
- RViz

### Verify hardware interfaces

```bash
ros2 control list_hardware_interfaces
```

Expected output:

```
command interfaces
      left_wheel_joint/velocity [available] [claimed]
      right_wheel_joint/velocity [available] [claimed]
state interfaces
      left_wheel_joint/position
      left_wheel_joint/velocity
      right_wheel_joint/position
      right_wheel_joint/velocity
```

### Verify controllers

```bash
ros2 control list_controllers
```

Expected output:

```
astro_diff_drive_controller[diff_drive_controller/DiffDriveController] active
joint_state_broadcaster[joint_state_broadcaster/JointStateBroadcaster] active
```

### Send velocity commands

```bash
ros2 topic pub --rate 30 /astro_diff_drive_controller/cmd_vel_unstamped geometry_msgs/msg/Twist "{
  linear: {x: 0.5, y: 0.0, z: 0.0},
  angular: {x: 0.0, y: 0.0, z: 0.5}
}"
```

The robot accepts:
- `linear.x` — forward/backward velocity (m/s), max ±1.0 m/s
- `angular.z` — rotational velocity (rad/s), max ±1.0 rad/s

---

## Package Structure

```
astro_hardware_interface/
├── hardware/
│   ├── astro_interface.cpp          # Hardware interface implementation
│   └── include/astro_hardware_interface/
│       ├── astro_interface.hpp      # AstroHardware class
│       ├── teensy_comms.hpp         # Serial communication with Teensy
│       └── wheel.hpp                # Wheel state/command data
├── bringup/
│   ├── launch/astro_hardware.launch.py
│   └── config/astro_controllers.yaml
├── description/
│   ├── launch/view_robot.launch.py
│   ├── urdf/                        # Robot URDF/xacro model
│   ├── ros2_control/                # ros2_control hardware config
│   └── rviz/                        # RViz configurations
└── astro_hardware_interface.xml     # pluginlib plugin description
```

## Robot Parameters

| Parameter          | Value               |
|--------------------|---------------------|
| Wheel separation   | 0.304 m             |
| Wheel radius       | 0.036 m             |
| Serial device      | `/dev/ttyACM0`      |
| Baud rate          | 115200              |
| Control loop rate  | 30 Hz               |
| Encoder counts/rev | 3436                |
