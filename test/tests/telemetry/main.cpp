#include "telemetry.hpp"
#include <emblib/rtos/scheduler.hpp>
#include <mp/common/pb.hpp>
#include <mp/telemetry/producer.hpp>

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