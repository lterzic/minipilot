#pragma once

#include "copter/copter_controller_task.hpp"
#include "link/rx.hpp"

namespace mp {

class handle_copter {
public:
    handle_copter(rx& rx, copter_controller_task& controller);

private:
    void handle(const mp_pb_link_Uplink& msg);

private:
    copter_controller_task& m_controller;
};

}