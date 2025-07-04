#pragma once

#include "vehicles/vehicle.hpp"
#include "logging/log_handler.hpp"
#include "sensors/sensor_manager.hpp"
#include <emblib/io/istream.hpp>
#include <emblib/io/ostream.hpp>
#include <etl/list.h>

namespace mp {

/**
 * Device drivers required by minipilot
 * 
 * @note Drivers taken as a pointer are optional,
 * and `nullptr` can be passed.
 */
struct devices_s {
    emblib::io::ostream<char>* telemetry_device;
    emblib::io::istream<char>& receiver_device;
    etl::ilist<log_handler*>& log_handlers;
};

/**
 * Minipilot entry point
 */
int main(
    const sensor_readers_s& sensors,
    const devices_s& devices,
    state_estimator& state_estimator,
    vehicle& vehicle
);

}