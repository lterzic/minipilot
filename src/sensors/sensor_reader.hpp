#pragma once

#include "logging/log.hpp"
#include <emblib/driver/sensor/sensor.hpp>
#include <emblib/rtos/task.hpp>
#include <emblib/rtos/mutex.hpp>

namespace mp {

/**
 * Template task for reading sensors
 * Allows for raw data processing through the `process` method
 */
template <typename raw_data_type, typename out_data_type = raw_data_type>
class sensor_reader : public emblib::task {

public:
    using sensor_t = emblib::sensor<raw_data_type>;

public:
    explicit sensor_reader(
        sensor_t& sensor,
        task_priority_e task_priority,
        milliseconds_t task_period
    ) :
        task(create_task_name(sensor).c_str(), task_priority, m_task_stack),
        m_sensor(sensor),
        m_task_period(task_period)
    {}
    virtual ~sensor_reader() = default;

    /**
     * Get last read raw value
     */
    raw_data_type get_raw() const noexcept
    {
        emblib::scoped_lock lock(m_read_mutex);
        return m_last_raw;
    }

    /**
     * Get last processed value
     */
    out_data_type get_processed() const noexcept
    {
        emblib::scoped_lock lock(m_read_mutex);
        return m_last_processed;
    }

    /**
     * Get the underlying sensor
     */
    const sensor_t& get_sensor() const noexcept
    {
        return m_sensor;
    }

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
    virtual out_data_type process(const raw_data_type& raw_data) noexcept = 0;

    /**
     * Task thread
     */
    void run() noexcept override;

    /// @todo Add calculate_bias method

    /**
     * Create the task name based on the sensor name
     * @note This returns the string object copy so that it
     * lasts as long as the task constructor
     */
    static etl::string<configMAX_TASK_NAME_LEN> create_task_name(sensor_t& sensor) noexcept
    {
        etl::string<configMAX_TASK_NAME_LEN> task_name = "Sensor reader: ";
        task_name += sensor.get_name();
        return task_name;
    }

private:
    emblib::task_stack_t<512> m_task_stack;
    milliseconds_t m_task_period;
    sensor_t& m_sensor;
    
    mutable emblib::mutex m_read_mutex;
    raw_data_type m_last_raw;
    out_data_type m_last_processed;
};

/**
 * Task implementation
 * @todo Add sensor information to the logs in this task
 */
template <typename raw_data_type, typename out_data_type>
inline void sensor_reader<raw_data_type, out_data_type>::run() noexcept
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
        // TODO: Remove this (too much overhead)
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
            m_last_processed = processed_data;

            // TODO: Once pub/sub system implemented, publish values from here
        } else {
            // TODO: Add information about sensor type to the log
            log_warning("Sensor reading failed");
        }

        sleep_periodic(m_task_period);
    }
}

}