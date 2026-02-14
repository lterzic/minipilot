#include "host.hpp"
#include <emblib/rtos/scheduler.hpp>
#include <mp/common/pb.hpp>
#include <mp/telemetry/producer.hpp>
#include <mp/telemetry/sink.hpp>
#include <pblink/telemetry/telemetry.nanopb.h>
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

class producer1 : public mp::telemetry_producer<mp::telemetry_channel_e::ACCELEROMETER> {
public:
    bool produce(payload_u& payload) noexcept override
    {
        payload.accelerometer.acceleration = mp::pb_from({1, 2, 3});
        payload.accelerometer.has_acceleration = true;
        return true;
    }
};

class producer2 : public mp::telemetry_producer<mp::telemetry_channel_e::BAROMETER> {
public:
    bool produce(payload_u& payload) noexcept override
    {
        payload.barometer.air_pressure = 123.456f;
        payload.barometer.air_temperature = 34.567f;
        return true;
    }
};

int main()
{
    producer1 p1;
    producer2 p2;

    test_telemetry_sink sink;
    sink.subscribe({mp::telemetry_channel_e::ACCELEROMETER, 0});
    sink.subscribe({mp::telemetry_channel_e::BAROMETER, 0});

    emblib::rtos::start_scheduler();
    return 0;
}