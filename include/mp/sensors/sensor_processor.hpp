#pragma once

#include "logging/logging.hpp"
#include <emblib/rtos/task.hpp>
#include <emblib/rtos/spinlock.hpp>
#include <etl/optional.h>

namespace mp {

/**
 * Sensor processor is a task which periodically reads a sensor
 * and does some processing to improve the quality of the data
 * such as noise filtering, bias removal, etc.
 */
template <typename sensor_type>
class sensor_processor : private emblib::rtos::static_task<256> {
public:
    using sensor_data = typename sensor_type::data_t;

    /**
     * Reader simplifies getting only new sensor data
     */
    class reader {
    public:
        reader(const sensor_processor& processor) :
            m_processor(processor),
            m_last_read_count(0)
        {}

        auto read_if_new() noexcept
        {
            etl::optional<sensor_data> reading;
            auto current_data = m_processor.read();
            if (current_data.second > m_last_read_count) {
                m_last_read_count = current_data.second;
                reading = current_data.first;
            }
            return reading;
        }

    private:
        size_t m_last_read_count;
        const sensor_processor& m_processor;
    };

public:
    /**
     * Construct a processor for a sensor with the specified
     * priority. Processor period will be set based on the
     * sensor data output rate.
     */
    explicit sensor_processor(
        sensor_type& sensor,
        task_priority_e task_priority
    ) :
        static_task("sensor processor", task_priority),
        m_sensor(sensor),
        m_task_period(1000 / sensor.get_rate()),
        m_sample_count(0)
    {}
    virtual ~sensor_processor() = default;

    /**
     * Get last processed value
     */
    etl::pair<sensor_data, size_t> read() const noexcept
    {
        emblib::rtos::scoped_lock lock(m_lock);
        return {m_data_processed, m_sample_count};
    }

    /**
     * Get last raw value
     */
    etl::pair<sensor_data, size_t> read_raw() const noexcept
    {
        emblib::rtos::scoped_lock lock(m_lock);
        return {m_data_raw, m_sample_count};
    }

    /**
     * Noise density is just the sensor noise density
     */
    float get_noise_density() const noexcept
    {
        return m_sensor.get_noise_density();
    }

    /**
     * Get a reader
     */
    auto get_reader() const noexcept
    {
        return reader(*this);
    }

private:
    /**
     * Apply processing to the raw input value
     * This can be a filter, bias subtraction, ...
     */
    virtual sensor_data process(const sensor_data& raw_data) noexcept = 0;

    /**
     * Task thread
     */
    void run() noexcept override;

    /// @todo Add calculate_bias method

private:
    mutable emblib::rtos::spinlock m_lock;
    milliseconds_t m_task_period;

    sensor_type& m_sensor;
    sensor_data m_data_raw, m_data_processed;
    size_t m_sample_count;
};

/**
 * Task implementation
 * @todo Add sensor information to the logs in this task
 */
template <typename sensor_type>
inline void sensor_processor<sensor_type>::run() noexcept
{
    // This task should only be created for valid sensors
    // so assert that the sensor is actually available
    if (!m_sensor.ping()) {
        log_error("Sensor did not respond to ping!");
        return;
        // TODO: If the sensor is not optional exit minipilot completely
    }

    sensor_data read_data;
    while (true) {
        if (m_sensor.read(read_data)) {
            // Data is processed pre mutex lock to avoid unnecessary blocking
            // on this mutex while the shared data is not being updated
            sensor_data processed_data = process(read_data);

            emblib::rtos::scoped_lock lock(m_lock);
            m_data_raw = read_data;
            m_data_processed = processed_data;
            m_sample_count++;
        } else {
            // TODO: Add information about sensor type to the log
            log_warning("Sensor reading failed");
        }

        sleep_periodic(m_task_period);
    }
}

}