#include "main.hpp"
#include "tasks/task_logger.hpp"
#include "tasks/task_telemetry.hpp"
#include "tasks/task_accelerometer.hpp"
#include "tasks/task_gyroscope.hpp"
#include "tasks/task_state_estimator.hpp"
#include "tasks/task_receiver.hpp"
#include "tasks/task_vehicle.hpp"
#include "util/logger.hpp"

namespace mp {

// Timeout for checking if the device is properly working
inline constexpr auto DEVICE_PROBE_TIMEOUT = std::chrono::milliseconds(10);

int main(const devices_s& devices, state_estimator& state_estimator, vehicle& vehicle)
{
    // If the logging task is not created, this stays uninitialized
    task_logger* task_logger_ptr = nullptr;

    // Initialize the logging system (task) if there is an available logging device
    if (devices.log_device && devices.log_device->probe(DEVICE_PROBE_TIMEOUT)) {
        // Create the logging task
        static task_logger task_logger(*devices.log_device);
        task_logger_ptr = &task_logger;

        // Logging device is used directly until the scheduler starts
        logger::get_instance().set_output_device(*devices.log_device);
        log_info("Logging available!");
    }

    // Accelerometer is required
    if (!devices.accelerometer.sensor.probe()) {
        log_error("Accelerometer not available!");
        return 1;
    }
    // TODO: Read bias from the configuration if exists, else warn the user
    vector3f accelerometer_bias = 0;
    // Create the accelerometer task
    static task_accelerometer task_accelerometer(
        devices.accelerometer.sensor,
        devices.accelerometer.transform,
        accelerometer_bias
    );

    // Gyroscope is required
    if (!devices.gyroscope.sensor.probe()) {
        log_error("Gyroscope not available!");
        return 1;
    }
    // Create the gyroscope task
    static task_gyroscope task_gyroscope(devices.gyroscope.sensor, devices.gyroscope.transform);

    // Receiver is required
    if (!devices.receiver_device.probe(DEVICE_PROBE_TIMEOUT)) {
        log_error("Receiver not available!");
        return 1;
    }
    // Create the receiver task
    static task_receiver task_receiver(devices.receiver_device);

    // Create the state estimator task
    static task_state_estimator task_state_estimator(
        state_estimator,
        task_accelerometer,
        task_gyroscope
    );

    // Create the vehicle task
    static task_vehicle task_vehicle(
        vehicle,
        task_receiver,
        task_state_estimator
    );

    // If there is a telemetry device available, create the telemetry task
    // Telemetry could also be required (not optional)
    if (devices.telemetry_device && devices.telemetry_device->probe(DEVICE_PROBE_TIMEOUT)) {
        static task_telemetry task_telemetry(
            *devices.telemetry_device,
            task_accelerometer,
            task_gyroscope,
            task_state_estimator
        );
        log_info("Telemetry available!");
    } else {
        log_warning("Telemetry not available!");
    }


    log_info("Starting the scheduler...");
    
    // If a logging device exists, it means the task was already
    // created, so now switch the logging to go through the logging task
    if (task_logger_ptr) {
        logger::get_instance().set_output_device(*task_logger_ptr);
    }

    // Start the scheduler
    emblib::task::start_tasks();
    
    // Should never reach this
    return 1;
}

}