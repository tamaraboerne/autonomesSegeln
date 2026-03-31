# Clifton Autopilot Module
### Directory: `autopilot/src/modules/sand_yacht_clifton_control`

This module implements the **autonomous control system** for the Clifton land‑sailing vehicle.  
It runs directly on the **STM32 microcontroller** and contains all logic required for:

- steering control  
- sail optimization  
- course regulation  
- mission navigation  
- safety monitoring  
- system state encoding  

It is executed in the embedded system’s main loop through the `Run()` method and forms the core of the vehicle’s autonomous behavior.

---

# 🧭 Overview

The **Sand Yacht Clifton Control Module** provides:

- **Waypoint navigation & route following**
- **Heading regulation**
- **Flap (rudder) actuator control**
- **Sail force optimization**
- **Autopilot mode/state machine**
- **Binary error & status codes**
- **Safety fallback behavior**
- **Integration with sensors & telemetry**

This module is designed for deterministic real‑time execution and interacts with lower-level HAL drivers and higher-level telemetry interfaces.

---

## 📁 Module Architecture

```text
sand_yacht_clifton_control/
│
├── control_manager.*           # High‑level autopilot mode handling
├── heading_controller.*        # Heading regulation logic
├── flap_controller.*           # Steering flap PWM/servo control
├── sail_optimizer.*            # Sail angle optimization logic
├── mission_navigation.*        # Waypoint and route handling
├── safety_monitor.*            # Safety limits, sensor checks, emergency stop
├── status_codes.hpp            # Binary status and error bitfields
└── io_interfaces/              # Hardware abstraction for sensors/actuators
```

# ⚙️ Core Components

## **1. Control Manager**
Central orchestrator for the autopilot:

- Manages autopilot states (Idle, Manual, Autonomous, Error)
- Initializes and coordinates all sub-modules
- Ensures correct call order inside the main control loop
- Handles transitions to/from error states

---

## **2. Heading Controller**
Responsible for keeping the vehicle on the correct course.

Features include:

- Heading error calculation  
- Proportional / PID-like steering logic  
- Anti-windup and deadband zones  
- Max steering limit protection  
- Smoothing to avoid oscillations

The output is a target steering angle sent to the flap controller.

---

## **3. Flap Controller**
Controls the physical **steering flap** actuator.

Responsibilities:

- Converts steering command to PWM  
- Enforces actuator limits  
- Implements rate limiting for smooth steering  
- Prevents collisions at mechanical end stops  

---

## **4. Sail Optimization Logic**
Computes optimal sail position based on:

- apparent wind  
- current heading  
- route demands  
- stability thresholds  

Includes:

- no‑go‑zone handling  
- stall prevention  
- thrust maximization heuristic  

---

## **5. Mission Navigation**
Handles the mission/waypoint logic:

- Selects next waypoint  
- Computes target course  
- Determines "arrival radius"  
- Handles mission completion  
- Interfaces with laptop/frontend mission updates  

---

## **6. Safety Monitor**
Runs continuously and checks:

- sensor validity  
- flap servo response  
- maximum allowed sail angle  
- runaway conditions  
- timeout and communication failures  
- system health flags  

Triggers:

- controlled shutdown  
- mode switch to recovery  
- binary error code updates  

---

## **7. Status & Error Encoding**
Compact bitfield‑based structure for telemetry:

- System State (Idle, Active, Error, Emergency)
- Subsystem Status (Sail, Flap, Sensors, Navigation)
- Error Flags (limit exceeded, invalid sensor data, actuator fault, etc.)

Used by:

- Laptop telemetry tool  
- Frontend visualizer  
- Log analysis  

---

# 🖧 Interfaces

## **Sensor Inputs**
- IMU / heading  
- Wind sensor  
- Odometry / wheel speed  
- GPS (via laptop middleware)

## **Actuator Outputs**
- Steering flap servo (PWM)  
- Sail actuator  

## **Communication**
- UART telemetry  
- Parameter updates  
- Mission uploads  

---

# 👩‍🔧 My Contributions (Autopilot Subteam – 3 members)

I contributed significantly to the development of this module:

### ✔ Real‑Time Control Loop
- Implemented the main cyclic `Run()` execution  
- Integrated all control components in deterministic order  

### ✔ Heading + Flap Controller
- Designed large parts of the steering logic  
- Tuning the heading regulation behavior  
- Implemented safety and smoothing constraints  

### ✔ Safety + Error System
- Wrote the binary error encoding  
- Added sensor and actuator validation steps  
- Implemented limit protection and emergency fallback  

### ✔ Sail Optimization
- Contributed to the thrust optimization logic  
- Added no‑go‑zone and stall-detection mechanisms  

### ✔ Integration Work
- STM32 HAL integration  
- PWM control setup  
- Sensor input preprocessing  

This module represents the bulk of the *core autonomous functionality* of the project.

---

# 🔗 Repository Links

### 📂 Autopilot Module Source Code  
**Sand Yacht Clifton Control**  
https://github.com/tamaraboerne/autonomesSegeln/tree/main/autopilot/src/modules/sand_yacht_clifton_control

### 📘 Root Project README  
https://github.com/tamaraboerne/autonomesSegeln



