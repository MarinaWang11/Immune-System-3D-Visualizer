# Immune System 3D Visualizer

A hybrid framework designed to visualize complex immune system interactions. This project implements a 3D visualizer developed in **C++17** and **OpenGL**, designed to process and animate spatiotemporal data exported from the [Immune System Simulator](https://github.com/alexbprr/immune_system_simulator.git).

## 🧬 Project Overview

The system provides an interactive environment to visualize biological entities (Agents) within various physiological compartments (Fields). It is built to support the **Behavior-Condition-Action (BCA)** paradigm, allowing researchers to visually validate emergent biological phenomena such as chemotaxis, viral infection, and immune cell recruitment.

### Key Features
* **Hybrid Architecture:** Optimized for high-speed data ingestion (CSV snapshots) and real-time rendering.
* **Dynamic Snapshot Loading:** A robust file-parsing system that handles non-integer simulation steps (e.g., `0.5`, `1.25`) by extracting timestamps directly from filenames and sorting them mathematically.
* **Real-time Animation:** Playback controls with adjustable speed, allowing frame-by-frame analysis of cell movement and state transitions.
* **Interactive 3D UI:** Powered by **Dear ImGui**, providing live controls for simulation playback, camera modes, and a detailed data table for agent inspection.
* **Volumetric Visualization:** Advanced rendering of semi-transparent cylinders and cubes to represent blood vessels, lymphatic ducts, and infection zones using **Alpha Blending**.

---

## 🛠 Tech Stack

| Component | Technology |
| :--- | :--- |
| **Language** | C++17 |
| **Graphics API** | OpenGL 3.3 (Core Profile) |
| **Window & Context** | GLFW 3.3, GLAD |
| **Math Library** | GLM (OpenGL Mathematics) |
| **User Interface** | Dear ImGui, ImGuiFileDialog |
| **Data Handling** | Standard C++ Filesystem & Streams |

---

## 🚀 Getting Started

### Prerequisites
* **Linux/WSL** (Recommended)
* `g++` (supporting C++17)
* `make`
* `pkg-config`
* OpenGL development headers (`libglfw3-dev`, `libassimp-dev`)

### Compilation & Execution
1. **Compile the Visualizer:**
    ```bash
    make clean
    make
    ```

2. **Run the application:**
    ```bash
    ./3dvis
    ```

---

## 🖥 Visualizer Controls

* **[W][A][S][D]:** Move camera (Free Cam mode).
* **[Arrow Keys]:** Rotate view.
* **[Mouse Scroll]:** Zoom in/out.
* **[C]:** Enable Free Camera mode.
* **[O]:** Enable Orbit mode.
* **[R]:** Reset camera to initial position.
* **[+][-]:** Advance or retreat simulation frames (supports decimal steps).

---

## 📂 Data Structure

The visualizer dynamically parses CSV files from the `data/` directory. Files should follow the naming convention:  
`field_[timestamp].csv` (e.g., `field_0.0.csv`, `field_0.5.csv`, `field_1.0.csv`).

**CSV Header Format:**
```csv
pos_x, pos_y, pos_z, agent_type, agent_state
10.5, 5.2, 12.0, Lymphocyte, Active
```

---

**Authors:** Marina Rihs Matos Wang and Alexandre Bittencourt Pigozzo   
