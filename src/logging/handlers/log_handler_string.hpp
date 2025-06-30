#pragma once

#include "logging/log_handler.hpp"

namespace mp {

class log_handler_string : public log_handler {

public:
    void handle_log(const log_s& log) noexcept override;
};

}