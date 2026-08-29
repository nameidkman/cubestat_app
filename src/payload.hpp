#ifndef PAYLOAD_HPP
#define PAYLOAD_HPP

#include <cstdint>
#include <optional>
#include <variant>
#include <vector>

namespace FlightSoftware {

enum class Message_Type : uint8_t {
    command_mechanism = 0x01,
    command_telemetry = 0x02,
    out_telemetry = 0x03
};

enum class Mechanism_id : uint8_t {
    nose_cone = 0x00,
    parachute_release = 0x01,
    camera = 0x02,
    payload_door = 0x03
};


enum class Mechanism_command : uint8_t{
    close = 0x00,
    open = 0x01,
    set_position = 0x02
};

struct Command_mechanism {
    Mechanism_id mechanism_id = Mechanism_id::nose_cone;
    Mechanism_command command = Mechanism_command::close;
    float value = 0.0f;
};


struct Command_telemetry {
    bool enable = false;
};
struct OutTelemetry{
    uint32_t timestamps_ms = 0;
    float altitude_m = 0.0f;
    float temperature_c = 0.0f;
    float pressure_kpa = 0.0f;
    float battery_voltage_v = 0.0f;
    float gps_lat = 0.0f;
    float gps_lon = 0.0f;
    uint8_t state = 0;
};

std::vector<uint8_t> Serialize_out_telemetry(const OutTelemetry& telemetry);

enum class Desrialize_error{
    none = 0,
    payload_empty,
    unknow_message_type,
    wrong_message_type,
    invalid_length
};

const char* To_string(Desrialize_error error);
template <typename T>
struct Deserialize_result{
    Desrialize_error error = Desrialize_error::none;
    T value;
    bool ok() const {return error == Desrialize_error::none;}
    explicit operator bool() const { return ok(); }
};
std::optional<Message_Type> Peek_message_type(const std::vector<uint8_t>& payload);
Deserialize_result<Command_mechanism> Deserialize_command_mechanism(const std::vector<uint8_t>& payload);

Deserialize_result<Command_telemetry> Deserialize_command_telemetry(const std::vector<uint8_t>& payload);

using In_bound_command = std::variant<Command_mechanism, Command_telemetry>;
struct Decoded_in_bound_result{
    Desrialize_error error = Desrialize_error::none;
    In_bound_command value{};
    bool ok() const {return error == Desrialize_error::none;}
    explicit operator bool() const { return ok(); }
};

Decoded_in_bound_result  Decoded_in_bound_command(const std::vector<uint8_t>& paylaod);
}

#endif // PAYLOAD_HPP
