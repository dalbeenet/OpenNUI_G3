#include <kernel/session.h>

namespace kernel {
namespace protocol {

::std::array<unsigned char, protocol::data_frame_header::header_size> protocol::data_frame_header::to_buffer() const
{
    ::std::array<unsigned char, protocol::data_frame_header::header_size> result;
    this->to_buffer(result.data());
    return result;
}

void protocol::data_frame_header::to_buffer(unsigned char* dest) const
{
    memset(dest, 0, header_size);
    memmove(dest, &identifier, sizeof(identifier));
    memmove(dest + sizeof(identifier), &data_size, sizeof(data_size));
    memmove(dest + sizeof(identifier) + sizeof(data_size), padding.data(), padding.size());
    memmove(dest + sizeof(identifier) + sizeof(data_size) + padding.size(), &opcode, sizeof(opcode));
}

void protocol::data_frame_header::from_raw_data(unsigned char* raw_data)
{
    uint32_t shift = 0;
    memmove(&identifier, raw_data + shift, sizeof(identifier));
    shift += sizeof(identifier);
    memmove(&data_size, raw_data + shift, sizeof(data_size));
    shift += sizeof(data_size);
    memmove(padding.data(), raw_data + shift, padding.size());
    shift += padding.size();
    memmove(&opcode, raw_data + shift, sizeof(opcode));
    shift += sizeof(opcode);
}

uint32_t packet_formatting(unsigned char* dst, data_frame_header& header, const unsigned char* data, uint32_t data_length)
{
    header.data_size = data_length + 2;
    header.to_buffer(dst);    
    memmove(dst + header.header_size, data, data_length);
    return header.header_size + data_length;
}

data_frame_header packet_parsing(unsigned char* raw_data, unsigned char* data_buffer)
{
    data_frame_header header;
    header.from_raw_data(raw_data);
    memmove(data_buffer, raw_data + header.header_size, header.data_size);
    return header;
}

} // namespace protocol
} // namespace kernel