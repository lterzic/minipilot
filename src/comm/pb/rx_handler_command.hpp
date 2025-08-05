#include "pb_rx.hpp"
#include "vehicles/copter/copter_control.hpp"

namespace mp {

class rx_handler_command : public rx_handler {
public:
    explicit rx_handler_command(copter_control* copter);

    /**
     * Execute the command on the appropriate vehicle type
     */
    void handle(const pb_mp_UplinkMessage& msg) noexcept override;

private:
    copter_control* m_copter;
};

}