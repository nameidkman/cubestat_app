#include "payload.hpp"

#include <cstdint>
#include <cstring>
#include <optional>
#include <vector>

namespace FlightSoftware{
namespace {
// little endian
void Append_U32_LE(std::vector<uint8_t>& out, uint32_t value){
    out.push_back(static_cast<uint8_t>(value & 0xFF));
    out.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    out.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
    out.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
}

void Append_Float_LE(std::vector<uint8_t>& out, float value){
    uint32_t bits;
    std::memcpy(&bits, &value, sizeof(bits));
    Append_U32_LE(out, bits);
}

uint32_t Read_U32_LE(const std::vector<uint8_t>& bytes, size_t offset){
    return static_cast<uint32_t>(bytes[offset]) |
           static_cast<uint32_t>(bytes[offset + 1]) << 8 |
           static_cast<uint32_t>(bytes[offset + 2]) << 16 |
           static_cast<uint32_t>(bytes[offset + 3]) << 24;
}

float Read_float_LE(const std::vector<uint8_t>& bytes, size_t offset){
    uint32_t bits = Read_U32_LE(bytes, offset);
    float value;
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}

constexpr size_t command_mechanism_payload_size = 1 + 1 + 1 + 4;
constexpr size_t command_telemetry_payload_size = 1 + 1;
constexpr size_t out_telemetry_payload_size = 1 + 4 + 4 + 6 + 1;

}


const char* To_string(Desrialize_error error){
    switch(error){
        case Desrialize_error::none: return "none";
        case Desrialize_error::payload_empty: return "payload_empty";
        case Desrialize_error::unknow_message_type: return "unknow_message_type";
        case Desrialize_error::wrong_message_type: return "wrong_message_type";
        case Desrialize_error::invalid_length: return "invalid_length";
    }
    return "Unknow error";
}

std::vector<uint8_t> Serialize_out_telemetry(const OutTelemetry& telemetry){
    std::vector<uint8_t> paylaod;
    paylaod.reserve(out_telemetry_payload_size);
    paylaod.push_back(static_cast<uint8_t>(Message_Type::out_telemetry));
   Append_U32_LE(paylaod, telemetry.timestamps_ms);
   Append_Float_LE(paylaod, telemetry.altitude_m);
   Append_Float_LE(paylaod, telemetry.temperature_c);
   Append_Float_LE(paylaod, telemetry.pressure_kpa);
   Append_Float_LE(paylaod, telemetry.battery_voltage_v);
   Append_Float_LE(paylaod, telemetry.gps_lat);
   Append_Float_LE(paylaod, telemetry.gps_lon);
   paylaod.push_back(telemetry.state);
   return paylaod;
}

std::optional<MessageType> PeekMessageType(const std::vector<uint8_t>& payload) {
    if (payload.empty()) {
        return std::nullopt;
    }
    return static_cast<MessageType>(payload[0]);
}

Deserialize_result<Command_mechanism> DeserializeCommandMechanism(
    const std::vector<uint8_t>& payload) {
    Deserialize_result<Command_mechanism> result;

    if (payload.empty()) {
        result.error = Desrialize_error::payload_empty;
        return result;
    }
    if (payload[0] != static_cast<uint8_t>(Message_Type::command_mechanism)) {
        result.error = Desrialize_error::wrong_message_type;
        return result;
    }
    if (payload.size() != command_mechanism_payload_size) {
        result.error = Desrialize_error::invalid_length;
        return result;
    }

    Command_mechanism cmd;
    cmd.mechanism_id = static_cast<Mechanism_id>(payload[1]);
    cmd.command = static_cast<Mechanism_command>(payload[2]);
    cmd.value = Read_float_LE(payload, 3);

    result.value = cmd;
    return result;
}

Deserialize_result<Command_telemetry> DeserializeCommandTelemetry(
    const std::vector<uint8_t>& payload) {
    Deserialize_result<Command_telemetry> result;

    if (payload.empty()) {
        result.error = Desrialize_error::payload_empty;
        return result;
    }
    if (payload[0] != static_cast<uint8_t>(Message_Type::command_telemetry)) {
        result.error =Desrialize_error::wrong_message_type;
        return result;
    }
    if (payload.size() != command_telemetry_payload_size) {
        result.error = Desrialize_error::invalid_length;
        return result;
    }

    Command_telemetry cmd;
    cmd.enable = (payload[1] != 0);

    result.value = cmd;
    return result;
}

Decoded_in_bound_result Decoded_in_bound_command(const std::vector<uint8_t>& payload) {
    Decoded_in_bound_result result;

    const auto type = PeekMessageType(payload);
    if (!type.has_value()) {
        result.error = Desrialize_error::payload_empty;
        return result;
    }

    switch (*type) {
        case Message_Type::command_mechanism: {
            auto decoded = DeserializeCommandMechanism(payload);
            result.error = decoded.error;
            if (decoded.ok()) {
                result.value = decoded.value;
            }
            return result;
        }
        case Message_Type::command_telemetry: {
            auto decoded = DeserializeCommandTelemetry(payload);
            result.error = decoded.error;
            if (decoded.ok()) {
                result.value = decoded.value;
            }
            return result;
        }
        case Message_Type::out_telemetry:
            // OutTelemetry flows CanSat -> Ground Station; it is not a
            // valid *inbound* command from the CanSat's perspective.
            result.error = Desrialize_error::unknow_message_type;
            return result;
    }

    result.error = Desrialize_error::unknow_message_type;
    return result;
}

};
