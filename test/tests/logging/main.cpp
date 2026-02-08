#include "host.hpp"
#include <emblib/rtos/scheduler.hpp>
#include <mp/logging/logging.hpp>
#include <pb_encode.h>

class test_log_sink : public mp::log_sink {
public:
    test_log_sink() :
        m_host(mp::test::get_host_dev())
    {}

    void write(const mp::log_entry_s& log) noexcept override
    {
        pb_byte_t send_buf[256];
        pb_ostream_t pb_ostream = pb_ostream_from_buffer(send_buf, sizeof(send_buf));

        assert(pb_encode(&pb_ostream, PBLINK_LOGGING_LOG_FIELDS, &log));
        ssize_t sent = m_host.write(reinterpret_cast<const char*>(send_buf), pb_ostream.bytes_written, emblib::io::timeout_t(0));
        assert(sent == pb_ostream.bytes_written);
    }

private:
    emblib::io::net::udp_stream m_host;
};

class test_task : public emblib::rtos::static_task<256> {
public:
    test_task() :
        static_task("test task", 1)
    {}
private:
    void run() noexcept override
    {
        log_warning("Logging from a task with 1 argument: {}", "My string argument");
        suspend();
    }
};

int main()
{
    test_log_sink sink;
    mp::logger::get_instance().add_sink(sink);

    log_info("Pre scheduler start log with two args: {}, {}", -12345, 123.456f);
    
    test_task t;
    emblib::rtos::start_scheduler();
    return 0;
}