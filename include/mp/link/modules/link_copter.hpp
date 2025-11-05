#pragma once

#include "link/link.hpp"
#include "copter/copter_controller_task.hpp"

namespace mp {

class link_copter : private rx_handler {
public:
    explicit link_copter(link& link, copter_controller_task& controller);

private:
    void handle(payload_u& payload) noexcept override;
    
private:
    copter_controller_task& m_controller;
};

}