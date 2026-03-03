# minzaniX - host runner for flight controller

This repository contains a small flight controller core (`core/Inc`, `core/Source`) and a `main.cpp` host that integrates the controller with AirSim.

Features
- Wraps existing C flight-control modules (`estimator`, `controller`, `altitude_*`, `mixer`, `safety`, `params`).
- `main.cpp` connects to AirSim if compiled with AirSim enabled, otherwise runs a stub simulation.

Build & Run

1) Stub mode (no AirSim required)

On Windows with MinGW-w64 g++ (PowerShell):

```powershell
g++ -std=c++17 -O2 -I. -Icore/Inc main.cpp -o fc_host.exe
.
\# Run
./fc_host.exe
```

This runs the controller using simulated sensor data (printed motor outputs at 100 Hz).

2) With AirSim (recommended for realistic sensors/actuators)

Prerequisites:
- AirSim (compiled library and headers) and dependencies installed.
- Add AirSim include and lib paths when compiling and linking.

Example compile (g++), adjust `-I` and `-L` to your AirSim build paths and link libraries accordingly:

```powershell
g++ -std=c++17 -O2 -DAIRSIM_AVAILABLE -I. -Icore/Inc -I"<AirSimIncludePath>" main.cpp -L"<AirSimLibPath>" -lairsim -l:rpc.lib -l:zmq.lib -o fc_host_airsim.exe
```

Notes:
- The `AIRSIM_AVAILABLE` preprocessor symbol enables the AirSim client code path.
- The example `main.cpp` calls `moveByMotorPWMs()` if available; depending on the AirSim build the exact API may differ (e.g. `simSetMotorPWMs`). Adjust the call accordingly.
- On Windows you will likely compile with MSVC (Visual Studio). Create a Visual Studio project, add `main.cpp`, set additional include directories to AirSim's `include` and link against AirLib/AirSim libraries.

What I added
- `main.cpp`: Host program that initializes the flight controller, polls sensors, calls `fc_step()`, and sends motor commands to AirSim (or prints them in stub mode).

Next steps I can help with
- Add a CMakeLists.txt for cross-platform builds.
- Implement a proper AirSim motor-output mapping if you tell me which AirSim method you prefer (`simSetMotorPWMs`, `moveByMotorPWMs`, etc.).
- Add a small test harness to run unit tests or a recorded flight profile.
