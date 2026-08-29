#include "frame.hpp"
#include <cstdint>

namespace FlightSoftware{

const char* to_string(PareError error){
    switch (error) {
        case PareError::none: return "None";
        case PareError::frame_too_short: return "Frame too short";
        case PareError::missing_starting_delimiter: return "Missing starting delimiter";
        case PareError::incomplete_frame: return "Incomplete frame";
        case PareError::invalid_check_sum: return "Invalid check sum";
        case PareError::unexpected_frame_type: return "Unexpected frame type";
        case PareError::payload_length_mismactch: return "Payload length mismatch";
    }
    return "Unknown error";
}

namespace {
uint8_t sum_bytes(const std::vector<uint8_t>&bytes, size_t begin, size_t end){
        uint8_t sum = 0;
        for(size_t i = begin; i < end; i++){
            sum = static_cast<uint8_t>(sum + bytes[i]);
        }
        return sum;
}

void apped_u16(std::vector<uint8_t>& out, uint16_t value){
    out.push_back(static_cast<uint8_t>(value >> 8) & 0xFF);
    out.push_back(static_cast<uint8_t>(value & 0xFF));
}

void apped_u64(std::vector<uint8_t>& out, uint64_t value){
    for(int shift = 56; shift >= 0; shift -=8){
        out.push_back(static_cast<uint8_t>((value >> shift) & 0xFF));
    }
}

uint16_t read_u16(const std::vector<uint8_t>& bytes, size_t offset){
    return static_cast<uint16_t>((bytes[offset] << 8 )| bytes[offset + 1]);
}

uint64_t read_u64(const std::vector<uint8_t>& bytes, size_t offset){
    uint64_t value = 0;
    for(size_t i = 0; i < 8; ++i){
        value = (value << 8) | (bytes[offset + i]);
    }
    return value;
}
}

std::vector<uint8_t> build_transmit_request_frame(
    const std::vector<uint8_t> payload,
    const transmit_request_options& options){
        std::vector<uint8_t> frame_data;
        frame_data.reserve(14 + payload.size());
        frame_data.push_back(frame_type_transmit_request);
        apped_u64(frame_data, options.dest_address_64);
        apped_u16(frame_data, options.dest_address_16);
        frame_data.push_back(options.brodcasting_radius);
        frame_data.push_back(options.options);
        frame_data.insert(frame_data.end(), payload.begin(), payload.end());
        const uint8_t check_sum = static_cast<uint8_t>(
            0xFF - sum_bytes(frame_data, 0, frame_data.size())
        );
        std::vector<uint8_t> frame;
        frame.reserve(4 + frame_data.size());
        frame.push_back(frame_delimiter);
        apped_u16(frame, static_cast<uint16_t>(frame_data.size()));
        frame.insert(frame.end(), frame_data.begin(), frame_data.end());
        return frame;

}

}
