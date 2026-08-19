#include "payload.hpp"
#include <execution>
#include <iterator>


// identifires type for the message
const uint8_t TYPE_COMMAND_MECHANISM = 0x01;
const uint8_t TYPE_COMMAND_TELEMETRY = 0x02;
const uint8_t TYPE_OUT_TELEMETRY = 0x03;

FlightSoftware::ReceivedMessage FlightSoftware::Payload::deserialize(const std::vector<uint8_t>& payload) {
    if (payload.empty()) {
        throw std::invalid_argument("payload is empty");
    }
    uint8_t message_type = payload[0];
    switch(message_type){
        case TYPE_COMMAND_MECHANISM:{
            if(payload.size() < 3)
                throw std::invalid_argument("payload is too short");

            CommandMechanism cmd;
            cmd.mechanism_id = payload[1];
            cmd.value = payload[2];
            return cmd;
        }
            break;
        case TYPE_COMMAND_TELEMETRY:{
            if(payload.size() < 2)
                throw std::invalid_argument("payload is too short");
            CommandTelemetry cmd;
            cmd.is_on = (payload[1] != 0);
            return cmd;
        }
            break;
        case TYPE_OUT_TELEMETRY:{
             if(payload.size() < 7)
                throw std::invalid_argument("payload is too short");
             OutTelemetry out;
             out.seconds_since_epoch =
                 (static_cast<uint32_t>(payload[1]) << 24) |
                 (static_cast<uint32_t>(payload[2]) << 16) |
                 (static_cast<uint32_t>(payload[3]) << 8) |
                 (static_cast<uint32_t>(payload[4]));
             out.mechanisms_deployed_flags =
                 (static_cast<uint16_t>(payload[5]) << 8) |
                 (static_cast<uint16_t>(payload[6]));
             return out;
        }
            break;
        default:
            throw std::invalid_argument("unknown payload type");
    }
}


std::vector<uint8_t> FlightSoftware::Payload::serialize(const OutTelemetry& out) {
    std::vector<uint8_t> result;
    result.push_back(TYPE_OUT_TELEMETRY);
    result.push_back(static_cast<uint8_t>(out.seconds_since_epoch >> 24));
    result.push_back(static_cast<uint8_t>(out.seconds_since_epoch >> 16));
    result.push_back(static_cast<uint8_t>(out.seconds_since_epoch >> 8));
    result.push_back(static_cast<uint8_t>(out.seconds_since_epoch));
    result.push_back(static_cast<uint8_t>(out.mechanisms_deployed_flags >> 8));
    result.push_back(static_cast<uint8_t>(out.mechanisms_deployed_flags));
    return result;
}
