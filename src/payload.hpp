#ifndef PAYLOAD_HPP
#define PAYLOAD_HPP

#include <vector>
#include <cstdint>
#include <stdexcept>
#include <variant>

namespace FlightSoftware {

// Application Structures as defined in the assignment
struct CommandMechanism {
    uint8_t mechanism_id;
    uint8_t value;
};

struct CommandTelemetry {
    bool is_on;
};

struct OutTelemetry {
    uint32_t seconds_since_epoch;
    uint16_t mechanisms_deployed_flags;
};

// Variant representing any of the received application messages
using ReceivedMessage = std::variant<CommandMechanism, CommandTelemetry, OutTelemetry>;

class Payload {
public:
    /**
     * @brief Serializes an OutTelemetry structure into a byte payload vector.
     */
    static std::vector<uint8_t> serialize(const OutTelemetry& telemetry);

    /**
     * @brief Deserializes a raw payload vector into the appropriate command or telemetry structure.
     */
    static ReceivedMessage deserialize(const std::vector<uint8_t>& payload);
};

} // namespace FlightSoftware

#endif // PAYLOAD_CODEC_HPP
