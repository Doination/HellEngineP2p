#include "NetSystem.h"
#include <chrono>
#include <cstring>

namespace hellnet {

static double now_ms() {
    using namespace std::chrono;
    return duration<double, std::milli>(high_resolution_clock::now().time_since_epoch()).count();
}

bool NetSystem::host(uint16_t port) {
    if (!m_udp.start_server(port)) return false;
    m_role = Role::Host;
    m_seq = 1;
    m_connected = false;
    m_lastPingSentMs = -1.0;
    m_lastSnapshotSentMs = 0.0;
    return true;
}

bool NetSystem::join(const char* host, uint16_t port) {
    if (!m_udp.start_client(host, port)) return false;
    m_role = Role::Client;
    m_seq = 1;
    m_connected = false;
    // Send JoinRequest
    ByteBuffer b;
    NetHeader h;
    h.msg = Msg::JoinRequest;
    h.seq = m_seq++;
    b.write_pod(h);
    m_udp.send(b.data.data(), (int)b.data.size());
    return true;
}

void NetSystem::shutdown() {
    m_udp.stop();
    m_role = Role::Offline;
    m_connected = false;
}

void NetSystem::sendPacket(Msg m, const ByteBuffer& payload) {
    ByteBuffer out;
    NetHeader h;
    h.msg = m;
    h.seq = m_seq++;
    out.write_pod(h);
    if (!payload.data.empty()) out.write_bytes(payload.data.data(), payload.data.size());
    m_udp.send(out.data.data(), (int)out.data.size());
}

void NetSystem::pump(double nowMs) {
    // poll socket
    m_udp.pump([&](const uint8_t* data, int len){ handlePacket(data, len, nowMs); });
    // keepalive/ping
    if (m_lastPingSentMs < 0.0 || nowMs - m_lastPingSentMs > kPingIntervalMs) {
        sendPing(nowMs);
        m_lastPingSentMs = nowMs;
    }
    // host: send snapshots at cadence
    if (m_role == Role::Host && m_connected) {
        maybeSendSnapshot(nowMs);
    }
}

void NetSystem::sendPing(double nowMs) {
    ByteBuffer bb;
    double t = nowMs;
    bb.write_pod(t);
    sendPacket(Msg::Ping, bb);
}

void NetSystem::handlePacket(const uint8_t* data, int len, double nowMs) {
    if (len < (int)sizeof(NetHeader)) return;
    size_t rp = 0;
    NetHeader h{};
    std::memcpy(&h, data, sizeof(h));
    rp += sizeof(h);
    switch (h.msg) {
    case Msg::JoinRequest:
        if (m_role == Role::Host) {
            m_connected = true;
            ByteBuffer bb;
            sendPacket(Msg::JoinAccept, bb);
        }
        break;
    case Msg::JoinAccept:
        if (m_role == Role::Client) {
            m_connected = true;
        }
        break;
    case Msg::Ping: {
        // echo back sender's local send time and our recv time
        double tSend{};
        ByteBuffer tail;
        tail.data.insert(tail.data.end(), data + rp, data + len);
        size_t rpp = 0;
        tail.read_pod(rpp, tSend);
        ByteBuffer reply;
        reply.write_pod(tSend);
        reply.write_pod(nowMs);
        sendPacket(Msg::Pong, reply);
        break;
    }
    case Msg::Pong: {
        double tSend{};
        double tRecv{};
        ByteBuffer tail;
        tail.data.insert(tail.data.end(), data + rp, data + len);
        size_t rpp = 0;
        tail.read_pod(rpp, tSend);
        tail.read_pod(rpp, tRecv);
        double rtt = nowMs - tSend;
        m_clock.on_ping_result(rtt, tSend, tRecv);
        break;
    }
    case Msg::InputFrame: {
        if (m_role == Role::Host && m_cb.onHostReceiveRemoteInput) {
            InputFrame in{};
            ByteBuffer tail;
            tail.data.insert(tail.data.end(), data + rp, data + len);
            size_t rpp = 0;
            tail.read_pod(rpp, in);
            m_cb.onHostReceiveRemoteInput(in);
        }
        break;
    }
    case Msg::FireEvent: {
        if (m_role == Role::Host && m_cb.onHostFireEvent) {
            FireEvent fe{};
            ByteBuffer tail;
            tail.data.insert(tail.data.end(), data + rp, data + len);
            size_t rpp = 0;
            tail.read_pod(rpp, fe);
            m_cb.onHostFireEvent(fe);
        }
        break;
    }
    case Msg::Snapshot: {
        if (m_role == Role::Client && m_cb.onClientApplySnapshot) {
            Snapshot snap{};
            ByteBuffer tail;
            tail.data.insert(tail.data.end(), data + rp, data + len);
            size_t rpp = 0;
            tail.read_pod(rpp, snap);
            m_cb.onClientApplySnapshot(snap);
        }
        break;
    }
    default:
        break;
    }
}

void NetSystem::captureLocalInput(const InputFrame& in) {
    if (m_role == Role::Offline) return;
    ByteBuffer bb;
    bb.write_pod(in);
    sendPacket(Msg::InputFrame, bb);
}

void NetSystem::sendFireEvent(const FireEvent& fe) {
    if (m_role == Role::Offline) return;
    ByteBuffer bb;
    bb.write_pod(fe);
    sendPacket(Msg::FireEvent, bb);
}

void NetSystem::hostSubmitAuthoritativeStates(uint32_t tick,
                                              const PlayerState& hostState,
                                              const PlayerState& clientState) {
    if (m_role != Role::Host) return;
    m_lastAuthoritative.tick   = tick;
    m_lastAuthoritative.host   = hostState;
    m_lastAuthoritative.client = clientState;
    // store in history for lag-comp
    m_hist[tick % kMaxHistoryTicks] = m_lastAuthoritative;
    if (!m_histInit) {
        m_histInit = true;
        m_histBaseTick = tick;
    }
}

bool NetSystem::getHistoricalSnapshot(uint32_t tick, Snapshot& out) const {
    if (!m_histInit) return false;
    out = m_hist[tick % kMaxHistoryTicks];
    return out.tick == tick;
}

void NetSystem::maybeSendSnapshot(double nowMs) {
    const double period = 1000.0 / double(kSnapshotHz);
    if (nowMs - m_lastSnapshotSentMs < period) return;
    if (m_lastAuthoritative.tick == 0) return;
    ByteBuffer bb;
    bb.write_pod(m_lastAuthoritative);
    sendPacket(Msg::Snapshot, bb);
    m_lastSnapshotSentMs = nowMs;
}

} // namespace hellnet
