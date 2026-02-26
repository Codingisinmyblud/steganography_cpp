#include "steg/container.hpp"
#include "steg/checksum.hpp"
#include "steg/error_correction.hpp"
#include "steg/bitstream.hpp"

namespace steg {

std::vector<uint8_t> Container::pack(const std::vector<uint8_t>& payload) {
    std::vector<uint8_t> ecc_payload = ErrorCorrection::encode(payload);
    uint32_t csum = Checksum::compute(ecc_payload);
    
    BitstreamWriter writer;
    writer.write_byte('S');
    writer.write_byte('T');
    writer.write_byte('E');
    writer.write_byte('G');
    
    writer.write_byte((ecc_payload.size() >> 24) & 0xFF);
    writer.write_byte((ecc_payload.size() >> 16) & 0xFF);
    writer.write_byte((ecc_payload.size() >> 8) & 0xFF);
    writer.write_byte(ecc_payload.size() & 0xFF);
    
    writer.write_byte((csum >> 24) & 0xFF);
    writer.write_byte((csum >> 16) & 0xFF);
    writer.write_byte((csum >> 8) & 0xFF);
    writer.write_byte(csum & 0xFF);
    
    writer.write_bytes(ecc_payload);
    return writer.get_data();
}

bool Container::unpack(const std::vector<uint8_t>& packed_data, std::vector<uint8_t>& out_payload) {
    BitstreamReader reader(packed_data);
    std::vector<uint8_t> magic;
    if (!reader.read_bytes(magic, 4) || magic[0] != 'S' || magic[1] != 'T' || magic[2] != 'E' || magic[3] != 'G') {
        return false;
    }
    
    std::vector<uint8_t> sz_bytes;
    if (!reader.read_bytes(sz_bytes, 4)) return false;
    uint32_t payload_size = (sz_bytes[0] << 24) | (sz_bytes[1] << 16) | (sz_bytes[2] << 8) | sz_bytes[3];
    
    std::vector<uint8_t> csum_bytes;
    if (!reader.read_bytes(csum_bytes, 4)) return false;
    uint32_t expected_csum = (csum_bytes[0] << 24) | (csum_bytes[1] << 16) | (csum_bytes[2] << 8) | csum_bytes[3];
    
    std::vector<uint8_t> ecc_payload;
    if (!reader.read_bytes(ecc_payload, payload_size)) return false;
    
    uint32_t actual_csum = Checksum::compute(ecc_payload);
    if (actual_csum != expected_csum) return false;
    
    out_payload = ErrorCorrection::decode(ecc_payload);
    return true;
}

} // namespace steg
