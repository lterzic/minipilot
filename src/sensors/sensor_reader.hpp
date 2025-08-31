#pragma once

#include "logging/logging.hpp"
#include <emblib/devices/sensors/sensor.hpp>
#include <emblib/rtos/task.hpp>
#include <emblib/rtos/mutex.hpp>

namespace mp {

/**
 * Template task for reading sensors
 * Allows for raw data processing through the `process` method
 */
template <typename raw_data_type, typename out_data_type = raw_data_type>
class sensor_reader : private emblib::rtos::static_task<512> {

public:
    using sensor_t = emblib::devices::sensor<raw_data_type>;

public:
    explicit sensor_reader(
        sensor_t& sensor,
        emblib::rtos::mutex* sensor_mutex,
        task_priority_e task_priority,
        milliseconds_t task_period
    ) :
        static_task(create_task_name(sensor).c_str(), task_priority),
        m_sensor(sensor),
        m_sensor_mutex(sensor_mutex),
        m_task_period(task_period),
        m_sample_count(0)
    {}
    virtual ~sensor_reader() = default;

    /**
     * Get last read raw value
     */
    raw_data_type get_raw() const noexcept
    {
        emblib::rtos::scoped_lock lock(m_read_mutex);
        return m_last_raw;
    }

    /**
     * Get last processed value
     */
    out_data_type get_processed() const noexcept
    {
        emblib::rtos::scoped_lock lock(m_read_mutex);
        return m_last_processed;
    }

    /**
     * Get the count of the current available sample
     */
    size_t get_sample_count() const noexcept
    {
        emblib::rtos::scoped_lock lock(m_read_mutex);
        return m_sample_count;
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
    milliseconds_t m_task_period;
    
    // The sensor might be shared with other tasks
    // for example if one IMU provides both accelerometer
    // and gyroscope data. This is needed to make sure
    // both tasks don't read at the same time
    emblib::rtos::mutex* m_sensor_mutex;
    sensor_t& m_sensor;
    
    mutable emblib::rtos::mutex m_read_mutex;
    raw_data_type m_last_raw;
    out_data_type m_last_processed;
    size_t m_sample_count;
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

        // If a mutex exists, acquire it before trying to read
        if (m_sensor_mutex) {
            m_sensor_mutex->lock();
        }

        if (m_sensor.read(read_data)) {
            // Reading done, so allow other tasks to use the sensor
            if (m_sensor_mutex) {
                m_sensor_mutex->unlock();
            }

            // Data is processed pre mutex lock to avoid unnecessary blocking
            // on this mutex while the shared data is not being updated
            auto processed_data = process(read_data);

            emblib::rtos::scoped_lock lock(m_read_mutex);
            m_last_raw = read_data;
            m_last_processed = processed_data;
            m_sample_count++;

            // TODO: Once pub/sub system implemented, publish values from here
        } else {
            if (m_sensor_mutex) {
                m_sensor_mutex->unlock();
            }
            
            // TODO: Add information about sensor type to the log
            log_warning("Sensor reading failed");
        }

        sleep_periodic(m_task_period);
    }
}

}