#include "main.hpp"
#include "logging/log.hpp"
#include "tasks/task_telemetry.hpp"
#include "tasks/task_accelerometer.hpp"
#include "tasks/task_gyroscope.hpp"
#include "tasks/task_state_estimator.hpp"
#include "tasks/task_receiver.hpp"
#include "tasks/task_vehicle.hpp"

namespace mp {

int main(const devices_s& devices, state_estimator& state_estimator, vehicle& vehicle)
{
    // Initialize the logging system if there is an available logging device
    if (!devices.log_handlers.empty()) {
        logger::get_instance().set_handlers(devices.log_handlers);
        log_info("Logging available!");
    }

    // Accelerometer is required
    if (!devices.accelerometer.sensor.probe()) {
        log_error("Accelerometer not available!");
        return 1;
    }
    // Create the accelerometer task
    static task_accelerometer task_accelerometer(
        devices.accelerometer.sensor,
        devices.accelerometer.transform
    );

    // Gyroscope is required
    if (!devices.gyroscope.sensor.probe()) {
        log_error("Gyroscope not available!");
        return 1;
    }
    // Create the gyroscope task
    static task_gyroscope task_gyroscope(
        devices.gyroscope.sensor,
        devices.gyroscope.transform
    );

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
    if (devices.telemetry_device) {
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

    // Start the scheduler
    emblib::task::start_scheduler();
    
    // Should never reach this
    return 1;
}

}