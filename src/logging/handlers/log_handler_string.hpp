#pragma once

#include "logging/log_handler.hpp"

namespace mp {

class log_handler_string : public log_handler {

public:
    explicit log_handler_string(etl::ilist<log_device*>& devices) :
        log_handler(devices)
    {}
    
    void handle_log(const log_s& log) noexcept override;
};

}