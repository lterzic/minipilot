#pragma once

#include "task_config.hpp"
#include "util/logger.hpp"
#include <emblib/driver/sensor/sensor.hpp>
#include <emblib/rtos/task.hpp>
#include <emblib/rtos/mutex.hpp>

namespace mp {

/**
 * Template task for reading sensors
 * Allows for raw data processing through the `process` method
 */
template <typename raw_data_type, typename out_data_type = raw_data_type>
class task_sensor : public emblib::task {

public:
    using sensor_t = emblib::sensor<raw_data_type>;

public:
    explicit task_sensor(
        sensor_t& sensor,
        const char* task_name,
        task_priority_e task_priority,
        milliseconds_t task_period
    ) :
        task(task_name, task_priority, m_task_stack),
        m_sensor(sensor),
        m_task_period(task_period)
    {}
    virtual ~task_sensor() = default;

    /**
     * Get last read raw value
     */
    raw_data_type get_raw() const noexcept
    {
        emblib::scoped_lock lock{m_read_mutex};
        return m_last_raw;
    }

    /**
     * Get last corrected value
     */
    out_data_type get_corrected() const noexcept
    {
        emblib::scoped_lock lock{m_read_mutex};
        return m_last_corrected;
    }

    /**
     * Get the noise variance based on the sensor noise
     * density and the sampling frequency
     * TODO: This can be moved to the sensor itself, based on
     * the output data rate which it can track
     * TODO: Change the return type to [data_type]^2
     */
    auto get_noise_variance() const noexcept
    {
        // TODO: Change this to sensor output data rate
        // TODO: Remove .value()
        auto fs = 1.f / m_task_period.value();

        auto noise_density = m_sensor.get_noise_density();
        auto noise_variance = fs * noise_density * noise_density;
        return noise_variance;
    }

    /// @todo Add calculate_bias method

private:
    /**
     * Initialize the sensor like setting the data rate,
     * calculating the bias (offset), etc. and return
     * true if the sensor is set up correctly. If false is
     * returned when the sensor is not working properly,
     * minipilot might exit if the sensor is not optional,
     * else it will try to run without this task.
     */
    virtual bool init(sensor_t& sensor) noexcept = 0;

    /**
     * Apply processing to the raw input value
     * This can be a filter, bias subtraction, ...
     */
    virtual out_data_type process(const raw_data_type& raw_data) const noexcept = 0;

    /**
     * Task thread
     */
    void run() noexcept override;

private:
    emblib::task_stack_t<512> m_task_stack;
    milliseconds_t m_task_period;
    sensor_t& m_sensor;
    
    mutable emblib::mutex m_read_mutex;
    raw_data_type m_last_raw;
    out_data_type m_last_corrected;
};

/**
 * Task implementation
 * @todo Add sensor information to the logs in this task
 */
template <typename raw_data_type, typename out_data_type>
inline void task_sensor<raw_data_type, out_data_type>::run() noexcept
{
    // This task should only be created for valid sensors
    // so assert that the sensor is actually available
    assert(m_sensor.probe());

    if (!init(m_sensor)) {
        log_error("Sensor init failed!");
        // TODO: If the sensor is not optional
        // exit minipilot completely
        return;
    }

    raw_data_type read_data;
    while (true) {
        if (!m_sensor.is_ready()) {
            // If this happens often consider increasing the
            // sensor output rate or the task sleep period
            log_warning("Sensor data not ready!");
        }

        if (m_sensor.read(read_data)) {
            // Data is processed pre mutex lock to avoid unnecessary blocking
            // on this mutex while the shared data is not being updated
            auto processed_data = process(read_data);

            emblib::scoped_lock lock(m_read_mutex);
            m_last_raw = read_data;
            m_last_corrected = processed_data;
        } else {
            // TODO: Add information about sensor type to the log
            log_warning("Sensor reading failed");
        }

        sleep_periodic(m_task_period);
    }
}

}