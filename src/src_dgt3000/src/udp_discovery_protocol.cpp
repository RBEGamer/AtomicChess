#include "udp_discovery_protocol.hpp"

#include <algorithm>

namespace udpdiscovery {
PacketHeader::PacketHeader() : packet_type(kPacketIAmHere), packet_index(0) {
  MakeMagic();

  reserved[0] = 0;
  reserved[1] = 0;
  reserved[2] = 0;
  reserved[3] = 0;
}

void PacketHeader::MakeMagic() {
  magic[0] = 'R';
  magic[1] = 'N';
  magic[2] = '6';
  magic[3] = 'U';
}

bool PacketHeader::TestMagic() const {
  if (magic[0] != 'R') {
    return false;
  }
  if (magic[1] != 'N') {
    return false;
  }
  if (magic[2] != '6') {
    return false;
  }
  if (magic[3] != 'U') {
    return false;
  }
  return true;
}

bool MakePacket(const PacketHeader& header, const std::string& user_data,
                size_t padding_size, std::string& packet_data_out) {
  packet_data_out.clear();

  if (user_data.size() > kMaxUserDataSize) {
    return false;
  }

  if (padding_size > kMaxPaddingSize) {
    return false;
  }

  size_t packet_size = sizeof(PacketHeader) + user_data.size() + padding_size;
  if (packet_size > kMaxPacketSize) {
    return false;
  }

  auto user_data_size = (uint16_t)user_data.size();
  auto padding_size_16 = (uint16_t)padding_size;

  packet_data_out.resize(packet_size);
  char* ptr = (char*)packet_data_out.data();

  auto* packet_header = (PacketHeader*)ptr;
  ptr += sizeof(PacketHeader);

  (*packet_header) = header;
  packet_header->MakeMagic();
  detail::StoreBigEndian(header.application_id, &packet_header->application_id);
  detail::StoreBigEndian(header.peer_id, &packet_header->peer_id);
  detail::StoreBigEndian(header.packet_index, &packet_header->packet_index);
  packet_header->reserved[0] = 0;
  packet_header->reserved[1] = 0;
  packet_header->reserved[2] = 0;
  packet_header->reserved[3] = 0;

  detail::StoreBigEndian(user_data_size, &packet_header->user_data_size);
  detail::StoreBigEndian(padding_size_16, &packet_header->padding_size);

  if (!user_data.empty()) {
    std::copy(user_data.begin(), user_data.begin() + user_data_size, ptr);
    ptr += user_data.size();
  }

  for (size_t i = 0; i < padding_size_16; ++i) {
    ptr[i] = 0;
    ++ptr;
  }

  return true;
}

bool ParsePacketHeader(const char* buffer, size_t buffer_size,
                       PacketHeader& header_out, const char*& buffer_left_out,
                       size_t& buffer_left_size_out) {
  buffer_left_out = nullptr;
  buffer_left_size_out = 0;

  if (buffer_size < sizeof(PacketHeader)) {
    return false;
  }

  const auto* header = (const PacketHeader*)buffer;
  if (!header->TestMagic()) {
    return false;
  }

  if (!IsKnownPacketType((PacketType)header->packet_type)) {
    return false;
  }

  PacketHeader parsed_packet_header = (*header);
  parsed_packet_header.application_id =
      detail::ReadBigEndian<uint32_t>(&parsed_packet_header.application_id);
  parsed_packet_header.peer_id =
      detail::ReadBigEndian<uint32_t>(&parsed_packet_header.peer_id);
  parsed_packet_header.packet_index =
      detail::ReadBigEndian<PacketIndex>(&parsed_packet_header.packet_index);
  parsed_packet_header.user_data_size =
      detail::ReadBigEndian<uint16_t>(&parsed_packet_header.user_data_size);
  parsed_packet_header.padding_size =
      detail::ReadBigEndian<uint16_t>(&parsed_packet_header.padding_size);

  header_out = parsed_packet_header;

  buffer_left_out = buffer + sizeof(PacketHeader);
  buffer_left_size_out = buffer_size - sizeof(PacketHeader);

  return true;
}

bool ReadUserData(const char* buffer, size_t buffer_size,
                  const PacketHeader& header, std::string& user_data_out,
                  const char*& buffer_left_out, size_t& buffer_left_size_out) {
  if (header.user_data_size > buffer_size) {
    return false;
  }

  user_data_out.clear();
  if (header.user_data_size)
    user_data_out.insert(user_data_out.end(), buffer,
                         buffer + header.user_data_size);

  buffer_left_out = buffer + header.user_data_size;
  buffer_left_size_out = buffer_size - header.user_data_size;

  return true;
}

bool ReadPadding(const char* buffer, size_t buffer_size,
                 const PacketHeader& header, const char*& buffer_left_out,
                 size_t& buffer_left_size_out) {
  if (header.padding_size > buffer_size) {
    return false;
  }

  buffer_left_out = buffer + header.padding_size;
  buffer_left_size_out = buffer_size - header.padding_size;

  return true;
}
}  // namespace udpdiscovery
