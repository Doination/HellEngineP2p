#pragma once
#include <cstdint>
#include <functional>

namespace hellnet {
struct UdpEndpoint { uint32_t ipBE = 0; uint16_t port = 0; };

class TransportUDP {
public:
    using OnPacket = std::function<void(const uint8_t* data, int len)>;
    bool start_server(uint16_t port);
    bool start_client(const char* host, uint16_t port);
    void stop();
    bool send(const void* data, int len);
    void pump(OnPacket onPacket);
private:
    int   m_sock = -1;
    bool  m_isServer = false;
    UdpEndpoint m_peer{};
};
} // namespace hellnet
