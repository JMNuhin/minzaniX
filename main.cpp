// main.cpp
// Minimal flight controller host for AirSim (motor-PWM only)

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <csignal>

extern "C" {
#include "core/Inc/fc_interface.h"
#include "core/Inc/fc_types.h"
}

#ifdef AIRSIM_AVAILABLE
#include "vehicles/multirotor/api/MultirotorRpcLibClient.hpp"
using namespace msr::airlib;
#endif

static std::atomic<bool> g_run{true};

void signal_handler(int) {
    g_run = false;
}

int main() {
    std::signal(SIGINT, signal_handler);
    std::cout << "Starting flight controller host...\n";

    // -------------------------------
    // Initialize FC
    // -------------------------------
    fc_init();
    fc_set_armed(true);

#ifdef AIRSIM_AVAILABLE
    // -------------------------------
    // Connect to AirSim
    // -------------------------------
    MultirotorRpcLibClient client;
    client.confirmConnection();
    client.enableApiControl(true);
    client.armDisarm(true);

    std::cout << "Connected to AirSim. Running loop (100Hz, PWM-only)...\n";

    const float dt = 0.01f;              // 100 Hz
    const std::chrono::milliseconds period(10);

    int dbg = 0;

    while (g_run) {
        auto t0 = std::chrono::steady_clock::now();

        // ------------------------------------------------------
        // Read IMU & Barometer from AirSim
        // ------------------------------------------------------
        auto imu = client.getImuData();
        auto baro = client.getBarometerData();

        imu_sample_t imu_in;
        imu_in.ax = (float)imu.linear_acceleration.x();
        imu_in.ay = (float)imu.linear_acceleration.y();
        imu_in.az = (float)imu.linear_acceleration.z();
        imu_in.gx = (float)imu.angular_velocity.x();
        imu_in.gy = (float)imu.angular_velocity.y();
        imu_in.gz = (float)imu.angular_velocity.z();

        baro_sample_t baro_in;
        baro_in.pressure = (float)baro.pressure;
        baro_in.temperature = 0.0f;

        // ------------------------------------------------------
        // Run FC step
        // ------------------------------------------------------
        motor_output_t out;
        fc_step(&imu_in, &baro_in, dt, &out);

        float m0 = out.motor[0]; // front-left
        float m1 = out.motor[1]; // front-right
        float m2 = out.motor[2]; // rear-right
        float m3 = out.motor[3]; // rear-left

        // Optional debug output
        if (++dbg >= 20) { // ~5 Hz
            dbg = 0;
            std::cout << "PWM: " << m0 << ", " << m1 << ", " << m2 << ", " << m3 << "\n";
        }

        // ------------------------------------------------------
        // Send raw PWM to AirSim
        // AirSim order is: (FR, RL, FL, RR)
        // Project mixer order is: M0=FL, M1=FR, M2=RR, M3=RL
        // ------------------------------------------------------
        try {
            client
                .moveByMotorPWMsAsync(
                    m1,   // AirSim front-right  = M1
                    m3,   // AirSim rear-left    = M3
                    m0,   // AirSim front-left   = M0
                    m2,   // AirSim rear-right   = M2
                    dt
                )
                ->waitOnLastTask();
        } catch (...) {
            std::cerr << "Motor PWM RPC send error.\n";
        }

        auto t1 = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
        if (elapsed < period) std::this_thread::sleep_for(period - elapsed);
    }

    client.armDisarm(false);
    client.enableApiControl(false);
    std::cout << "Shutting down.\n";

#else
    std::cerr << "Error: AIRSIM_AVAILABLE not defined. This build requires AirSim.\n";
#endif

    return 0;
}
