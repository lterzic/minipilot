#include "host.hpp"
#include <mp/telemetry/sink.hpp>
#include <pb_encode.h>

class test_telemetry_sink : public mp::telemetry_sink {
public:
    test_telemetry_sink() :
        telemetry_sink(mp::milliseconds_t(500)),
        m_host(mp::test::get_host_dev())
    {}

    bool write(const pblink::telemetry::telemetry_s& msg) noexcept override
    {
        pb_byte_t send_buf[256];
        pb_ostream_t pb_ostream = pb_ostream_from_buffer(send_buf, sizeof(send_buf));

        assert(pb_encode(&pb_ostream, PBLINK_TELEMETRY_TELEMETRY_FIELDS, &msg));
        ssize_t sent = m_host.write(reinterpret_cast<const char*>(send_buf), pb_ostream.bytes_written, emblib::io::timeout_t(0));
        assert(sent == pb_ostream.bytes_written);

        return true;
    }

private:
    emblib::io::net::udp_stream m_host;
};