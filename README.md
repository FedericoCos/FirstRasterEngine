# ⚙️ FirstRasterEngine

## 📖 Overview

FirstRasterEngine is a C++ project designed to implement a basic 3D graphics rasterization pipeline from scratch. This engine aims to provide a hands-on understanding of how 3D scenes are rendered, from camera setup and object transformations to pixel-level drawing.

The engine manages a scene graph, handles various game objects, and processes geometric data to produce a 2D image. Scene configurations, including object placement and properties, are loaded from a JSON file, offering flexibility and ease of modification.

## Features

-   **Custom 3D Rasterization Pipeline:** Core implementation of a rasterizer.
-   **Scene Graph Management:** Organize and manage 3D objects (GameObjects) within a hierarchical structure.
-   **Perspective Camera System:** Configurable camera for navigating the 3D scene.
-   **Basic 3D Model Loading:** Support for loading and rendering simple 3D models from the `models/` directory.
-   **Texture Mapping:** Apply textures to 3D objects, with assets loaded from the `textures/` directory.
-   **Dynamic Scene Loading:** Load and configure entire scenes, including objects, lights, and camera settings, from a `scene.json` file.
-   **Object-Oriented Scene Elements:** Pre-defined classes for various interactive/static scene elements such as `Flag`, `Car`, `Lamp`, `Lava`, and `Forest`.
-   **Player Interaction:** Includes a `Player` component for basic user interaction and movement within the scene.
-   **Basic Lighting Model:** Implementation of a simple lighting model, suggested by the `Lamp` object.

## 🛠️ Tech Stack

**Core Language:**

![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)

**Build System:**

![GNU Make](https://img.shields.io/badge/GNU%20Make-8E5C02?style=for-the-badge&logo=gnu-make&logoColor=white)

## 🚀 Quick Start

Follow these steps to get FirstRasterEngine up and running on your local machine.

### Prerequisites

Before you begin, ensure you have the following installed:

-   **C++ Compiler:** A C++11 compatible compiler (e.g., GCC, Clang).
-   **GNU Make:** For building the project.

### Installation

1.  **Clone the repository**
    ```bash
    git clone https://github.com/FedericoCos/FirstRasterEngine.git
    cd FirstRasterEngine
    ```

2.  **Build the engine**
    The project uses a Makefile for compilation.:
    ```bash
    make test
    ```
    This command will compile all source files and link them into an executable named `VulkanEngine`. It will then immediately execute it.
    The application will render the scene defined in `scene.json`.

## 📁 Project Structure

```
FirstRasterEngine/
├── .gitignore          # Standard Git ignore file
├── Camera.cpp          # Camera class implementation
├── Camera.hpp          # Camera class definition
├── Car.hpp             # Car object definition
├── Flag.cpp            # Flag object implementation
├── Flag.hpp            # Flag object definition
├── Forest.hpp          # Forest object definition
├── GameObject.cpp      # Base class for all scene objects implementation
├── GameObject.hpp      # Base class for all scene objects definition
├── Lamp.hpp            # Lamp (light source) object definition
├── Lava.hpp            # Lava object definition
├── Makefile            # Build configuration using GNU Make
├── Player.cpp          # Player control implementation
├── Player.hpp          # Player control definition
├── Scene.cpp           # Scene management and rendering implementation
├── Scene.hpp           # Scene management and rendering definition
├── headers/            # Directory for additional header files (if any)
├── models/             # Directory for 3D model assets
├── scene.json          # Main scene configuration file (JSON format)
├── shaders/            # Directory for shader logic or configurations
├── start.hpp           # The core structure of the engine
└── textures/           # Directory for image texture assets
```

## ⚙️ Configuration

The primary configuration for the 3D scene is handled via `scene.json`.

### `scene.json`

This file defines the layout, objects, and properties of your 3D world. It includes:

-   **Camera settings:** Position, orientation, field of view.
-   **Object definitions:** Type, position, rotation, scale, associated models and textures.
-   **Light sources:** Position, color, intensity.
-   **Other scene-specific parameters.**

Modify `scene.json` to customize the rendered scene.

## 🔧 Development

### Building the Project

The `Makefile` orchestrates the build process. Key commands include:

| Command     | Description                           |

| :---------- | :------------------------------------ |

| `make test`      | Compiles all source files, links the executable, and runs it. |

| `make clean`| Removes all compiled object files and the executable. |

