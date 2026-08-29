#ifndef FRAME_HPP
#define FRAME_HPP

#include <cstdint>
#include <vector>
#include <sys/types.h>

namespace FlightSoftware {

constexpr uint8_t frame_delimiter = 0x7e;

constexpr uint8_t frame_type_transmit_request = 0x10;
constexpr uint8_t frame_type_recive = 0x90;


constexpr uint16_t unknow_networking_address = 0xFFFFE;
constexpr uint64_t broadcast_address = 0x000000000000ffffull;

enum class PareError{
    none = 0,
    frame_too_short,
    missing_starting_delimiter,
    incomplete_frame,
    invalid_check_sum,
    unexpected_frame_type,
    payload_length_mismactch
};

const char* to_string(PareError error);

struct transmit_request_options{
    // non zero requests a transmit status response from the radio
    // default to 1 so that response are enables unless the otherwise
    uint8_t frame_id = 0x01;

    // default address matching the digi example
    uint64_t dest_address_64 = 0x0000000000000000ULL;

    uint16_t dest_address_16 = unknow_networking_address;

    uint8_t brodcasting_radius = 0x00;
    uint8_t options = 0x00;
};

std::vector<uint8_t> build_transmit_request_frame(
    const std::vector<uint8_t> payload,
    const transmit_request_options& option= transmit_request_options{}
);

struct recive_packet{
    uint64_t source_address_64 = 0;
    uint16_t source_address_16 = 0;
    uint8_t recive_option = 0;
    std::vector<uint8_t> payload;
};

struct parese_result{
    PareError error = PareError::none;
    recive_packet packet;
    bool ok() const { return error == PareError::none; }
    // make it explicitly convertible to book so that we always get a
    // boolean value when checking the result
    explicit operator bool() const {return ok(); }
};


parese_result parese_receive_packet_frame(const std::vector<uint8_t>& frame_bytes);
}

#endif // FRAME_HPP
