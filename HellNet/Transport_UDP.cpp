#include "Transport_UDP.h"
#ifdef _WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#  pragma comment(lib, "ws2_32.lib")
#endif
#include <cstring>

namespace hellnet {
static bool init_winsock() {
#ifdef _WIN32
    static bool inited=false;
    if (!inited) {
        WSADATA w; if (WSAStartup(MAKEWORD(2,2), &w)!=0) return false;
        inited=true;
    }
#endif
    return true;
}

bool TransportUDP::start_server(uint16_t port) {
    if (!init_winsock()) return false;
    m_isServer = true;
    m_sock = (int)socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_sock < 0) return false;
    u_long nb = 1;
#ifdef _WIN32
    ioctlsocket(m_sock, FIONBIO, &nb);
#endif
    sockaddr_in a{}; a.sin_family = AF_INET; a.sin_port = htons(port); a.sin_addr.s_addr = INADDR_ANY;
    if (bind(m_sock, (sockaddr*)&a, sizeof(a)) < 0) return false;
    return true;
}

bool TransportUDP::start_client(const char* host, uint16_t port) {
    if (!init_winsock()) return false;
    m_isServer = false;
    m_sock = (int)socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_sock < 0) return false;
    u_long nb = 1;
#ifdef _WIN32
    ioctlsocket(m_sock, FIONBIO, &nb);
#endif
    sockaddr_in a{}; a.sin_family = AF_INET; a.sin_port = htons(port);
#ifdef _WIN32
    InetPtonA(AF_INET, host, &a.sin_addr);
#else
    inet_pton(AF_INET, host, &a.sin_addr);
#endif
    m_peer.ipBE = a.sin_addr.s_addr; m_peer.port = port;
    return true;
}

void TransportUDP::stop() {
#ifdef _WIN32
    if (m_sock >= 0) closesocket(m_sock);
#else
    if (m_sock >= 0) close(m_sock);
#endif
    m_sock = -1;
}

bool TransportUDP::send(const void* data, int len) {
    if (m_sock < 0) return false;
    sockaddr_in to{}; to.sin_family = AF_INET; to.sin_port = htons(m_peer.port); to.sin_addr.s_addr = m_peer.ipBE;
    int r = sendto(m_sock, (const char*)data, len, 0, (sockaddr*)&to, sizeof(to));
    return r == len;
}

void TransportUDP::pump(OnPacket onPacket) {
    if (m_sock < 0) return;
    char buf[1500];
    for (;;) {
        sockaddr_in from{}; int fromlen = sizeof(from);
        int r = recvfrom(m_sock, buf, sizeof(buf), 0, (sockaddr*)&from, &fromlen);
        if (r <= 0) break;
        if (m_isServer) { // latch first peer
            m_peer.ipBE = from.sin_addr.s_addr;
            m_peer.port = ntohs(from.sin_port);
        }
        onPacket((const uint8_t*)buf, r);
    }
}
} // namespace hellnet
