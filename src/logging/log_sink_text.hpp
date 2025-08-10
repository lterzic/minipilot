#pragma once

#include "log_sink.hpp"

namespace mp {

class log_sink_text : public log_sink {
public:
    explicit log_sink_text(emblib::io::ostream<char>& dev) noexcept;

    void write(const log_s& log) noexcept override;

private:
    emblib::io::ostream<char>& m_dev;
};

}