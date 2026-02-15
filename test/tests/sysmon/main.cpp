#include "telemetry.hpp"
#include <emblib/rtos/task.hpp>
#include <emblib/rtos/scheduler.hpp>
#include <mp/common/pb.hpp>

class task1 : private emblib::rtos::static_task<128> {
public:
    task1() : static_task("task1", 1) {}

private:
    void run() noexcept override
    {
        auto task = xTaskGetCurrentTaskHandle();
        vTaskSetTaskNumber(task, 1);

        while (true) {
            for (int i = 0; i < 10000; i++) {}
            sleep_periodic(mp::milliseconds_t(60));
        }
    }
};

class task2 : private emblib::rtos::static_task<128> {
public:
    task2() : static_task("task2", 1) {}

private:
    void run() noexcept override
    {
        auto task = xTaskGetCurrentTaskHandle();
        vTaskSetTaskNumber(task, 2);

        while (true) {
            for (int i = 0; i < 10000; i++) {}
            sleep_periodic(mp::milliseconds_t(70));
        }
    }
};

int main()
{
    task1 t1;
    task2 t2;

    test_telemetry_sink sink;
    sink.subscribe({mp::telemetry_channel_e::KERNEL, 0});

    emblib::rtos::start_scheduler();
    return 0;
}