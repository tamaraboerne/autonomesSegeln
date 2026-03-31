# Project: Autonomous Sailing – Clifton Autopilot

This repository contains the codebase for **Clifton**, an autonomous land‑sailing vehicle developed as a final‑semester university group project.  
Our goal was to enable the vehicle to **navigate fully autonomously** using an aerodynamic sail, a steering flap, and a multilayer software system integrating embedded control, telemetry, and a real‑time frontend.

The project was developed by a team of **13 students**, divided into three sub‑teams: Autopilot, Laptop Software, and Frontend.

---

# 🔧 1. Autopilot on the Clifton (My Contribution)

I was part of the **Autopilot Team** (3 members), responsible for the embedded control system running directly on the vehicle’s STM32 microcontroller.  
This component is the “brain” of the land sailor — it performs all real‑time control tasks necessary for autonomous operation.

### 🔗 My Code (Autopilot Module)
You can find the part of the project I worked on here:

👉 **Autopilot Control Module (`sand_yacht_clifton_control/`)**  
https://github.com/tamaraboerne/autonomesSegeln/tree/main/autopilot/src/modules/sand_yacht_clifton_control

### 🔗 Autopilot Folder (contains all embedded code)
👉 https://github.com/tamaraboerne/autonomesSegeln/tree/main/autopilot

### 🔗 Autopilot README (if you add one)
👉 https://github.com/tamaraboerne/autonomesSegeln/blob/main/autopilot/README.md


---

### ✔ My Key Responsibilities
- **Development on STM32 (bare‑metal / embedded C++)**
- **Implementation of the main real‑time control loop**  
  - Designed and implemented the core `Run()` method executed cyclically for stable control  
- **Flap Controller**  
  - Developed the control logic regulating the steering flap for route following  
  - Contributed to tuning PID‑style and logic‑based controllers  
- **Course Regulation (Heading Control)**  
  - Implemented algorithms to keep the vehicle aligned with mission waypoints  
- **Propulsion / Sail Optimization**  
  - Contributed to logic determining the optimal sail angle relative to wind and course  
- **State & Error Encoding System**  
  - Designed binary‑coded status and error flags for debugging, telemetry, and safety  
- **Safety & Limits Management**  
  - Implemented emergency shutdown conditions (limit angles, sensor faults, stall conditions)

### ✔ Focus Areas I Worked On the Most
- Real‑time control architecture  
- Route following and steering flap logic  
- Safety‑critical state handling  
- Embedded communication interfaces shared with the laptop module  

The autopilot runs independently on the vehicle and is responsible for ensuring safe, stable, and accurate autonomous movement across all mission phases.

# 💻 2. Laptop Software
- Provides communication with the autopilot via **UART / USB / Radio**  
- Handles telemetry, mission configuration, and parameter tuning  
- Serves as a middleware bridge between the embedded system and frontend UI  

*(Developed by another sub‑team — my collaboration was limited to defining message structures and validation rules.)*

---

# 🌐 3. Frontend
- Real‑time visualization of vehicle state  
- Map display, waypoint editing, mission planning  
- Playback and analysis of recorded telemetry  

*(Developed by a separate UI/UX-focused team.)*

---

# 🎯 Project Objectives
- Build a stable, robust autonomous control system for a land‑sailing vehicle  
- Optimize speed and heading based on environmental conditions  
- Enable real‑time monitoring during field tests  
- Collect and evaluate telemetry for future control improvements  

---

# 📁 Project Structure
```text
├─ autopilot/   # Embedded control system for the land sailor (my main area)
├─ laptop/      # Telemetry + configuration middleware
├─ frontend/    # Visual UI for monitoring and mission planning
└─ README.md
