#include "main.hpp"
#include "logging/log.hpp"
#include "tasks/task_telemetry.hpp"
#include "tasks/task_state_estimator.hpp"
#include "tasks/task_receiver.hpp"
#include "tasks/task_vehicle.hpp"

namespace mp {

int main(
    const sensor_readers_s& sensors,
    const devices_s& devices,
    state_estimator& state_estimator,
    vehicle& vehicle
)
{
    // Initialize the logging system if there is an available logging device
    if (!devices.log_handlers.empty()) {
        logger::get_instance().set_handlers(devices.log_handlers);
        log_info("Logging available!");
    }

    // Create the sensor manager based on the available sensors
    static const sensor_manager sensor_manager(sensors);

    // Create the receiver task
    static task_receiver task_receiver(devices.receiver_device);

    // Create the state estimator task
    static task_state_estimator task_state_estimator(
        sensor_manager,
        state_estimator
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
            sensor_manager,
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