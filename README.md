# Pendulum Wave Simulator (SFML 3.0 & C++20)

A real-time visual simulation of the classic **Pendulum Wave** physics phenomenon, written in modern **C++20** and powered by **SFML 3.0**.

## Overview

The pendulum wave effect occurs when a series of uncoupled simple pendulums with monotonically increasing lengths are released simultaneously. Because each successive pendulum completes one additional oscillation within a shared global cycle time, the pendulums drift in and out of phase—creating mesmerizing traveling waves, standing waves, alternating sub-patterns, and apparent chaos before returning to full synchronization at the end of the cycle.

## Physics & Mathematical Model

> [!NOTE]
> This simulation models **ideal mathematical (simple) pendulums** under the small-angle approximation—point masses suspended from massless, inextensible strings oscillating without air resistance, pivot friction, or physical collisions.

Given a global cycle period $T_{\text{total}} = 60\text{ s}$ and a base oscillation count $N_0 = 50$, the $i$-th pendulum ($i \in \{0, 1, \dots, 24\}$) completes $N_0 + i$ oscillations per cycle:

$$T_i = \frac{T_{\text{total}}}{N_0 + i}, \qquad \omega_i = \frac{2\pi}{T_i}$$

Using the small-angle approximation for a simple gravity pendulum ($g = 9.81\text{ m/s}^2$), the physical length $L_i$ required to achieve period $T_i$ is:

$$L_i = g \left(\frac{T_i}{2\pi}\right)^2$$

At any simulation time $t$, the angular displacement $\theta_i(t)$ is computed analytically as:

$$\theta_i(t) = \theta_{\max} \cos(\omega_i t)$$

Physical lengths are automatically scaled to fit 80% of the window height while preserving exact relative proportions.

## Features

- **Modern C++20 & SFML 3.0 API**: Demonstrates SFML 3's `std::optional`-based event loop (`event->is<T>()`, `event->getIf<T>()`), scoped enums, vector brace initialization, and C++20 `<numbers>` and `<ranges>`.
- **Analytical Precision**: Uses closed-form harmonic motion equations (`std::cos`) rather than numerical integration, ensuring zero phase drift over long runs.
- **Smooth Rendering**: Configured with 8x MSAA anti-aliasing, a 120 FPS framerate cap, dynamic window resizing support, and a continuous RGB spectrum gradient across the 25 pendulums.
- **Interactive Time Controls**: Pause, reset, or dynamically speed up and slow down the simulation time scale.
- **macOS App Bundle Support**: Configured via CMake to build a native `.app` bundle (`Pendulum Wave 3.app`) on Apple Silicon (`arm64`).

## Controls

| Key | Action |
| :--- | :--- |
| `Space` | Pause / Resume simulation |
| `R` | Reset simulation time to $t = 0$ |
| `Up` / `Right` | Speed up simulation (`timeScale *= 1.2`) |
| `Down` / `Left` | Slow down simulation (`timeScale /= 1.2`) |
| `Escape` | Quit application |

## Simulation Parameters

Key constants can be customized directly at the top of `main.cpp`:

| Constant | Default | Description |
| :--- | :--- | :--- |
| `SCREEN_WIDTH` | `1800` | Initial window width in pixels |
| `SCREEN_HEIGHT` | `1000` | Initial window height in pixels |
| `NUM_PENDULUMS` | `25` | Total number of pendulums |
| `TOTAL_PERIOD_S` | `60.0f` | Full synchronization cycle duration (seconds) |
| `BASE_OSCILLATIONS` | `50` | Number of oscillations of the longest pendulum per cycle |
| `MAX_AMPLITUDE_DEG` | `22.0f` | Initial release angle (degrees) |
| `BOB_RADIUS` | `12.0f` | Visual radius of each pendulum bob (pixels) |

## Prerequisites

- **C++20** compatible compiler (Clang 13+, GCC 11+, or MSVC 2022+)
- **CMake** 3.17 or newer
- **SFML 3.0+** (`Graphics`, `Window`, and `System` components)

### Installing SFML 3.0

> [!IMPORTANT]
> Make sure your package manager or installation provides **SFML 3.x** (not SFML 2.5/2.6), as this project uses the SFML 3.0 API (`std::optional` event polling, scoped enums, etc.).

- **macOS (Homebrew):**
  Ensure Homebrew is up to date so the `sfml` formula resolves to `3.x` (as opposed to `sfml@2`):
  ```bash
  brew update
  brew install cmake sfml
  brew list --versions sfml  # Verify version is 3.0.0 or newer
  ```
- **Official Binaries / Building from Source (All Platforms):**
  If your system package manager still defaults to SFML 2.x (common on older Linux distributions or outdated package trees), download the official **SFML 3.0+** SDK from [sfml-dev.org](https://www.sfml-dev.org/download/) or build it from source:
  ```bash
  git clone --branch 3.0.x https://github.com/SFML/SFML.git
  cmake -S SFML -B SFML/build -DCMAKE_BUILD_TYPE=Release
  cmake --build SFML/build
  sudo cmake --install SFML/build
  ```
  *(If installed to a custom directory, pass `-DCMAKE_PREFIX_PATH=/path/to/sfml3` when configuring this project).*

## Building and Running

1. **Configure and build the project:**

   ```bash
   cmake -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build
   ```

2. **Run the executable:**

   - **macOS (App Bundle):**
     ```bash
     open "build/Pendulum Wave 3.app"
     # Or run the binary directly:
     ./build/Pendulum\ Wave\ 3.app/Contents/MacOS/Pendulum\ Wave\ 3
     ```
   - **Linux / Windows:**
     ```bash
     ./build/PendulumSim3
     ```

## License

This project is licensed under the **MIT License** with an additional **Beer-ware** clause — see the [LICENCE](LICENCE) file for details:

> *"THE BEER-WARE LICENSE" (Revision 42):*
> *Imre Városi wrote this software. As long as you retain this notice you can do whatever you want with this stuff. If we meet some day, and you think this stuff is worth it, you can buy me a beer in return.*


