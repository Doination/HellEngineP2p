#pragma once
#include <cstdint>
#include "NetConfig.h"
#include "ByteBuffer.h"
#include "Protocol.h"
#include "Transport_UDP.h"
#include "ClockSync.h"

namespace hellnet {
enum class Role { Host, Client, Offline };

struct NetCallbacks {
    // Host-only: remote input arrived for this tick
    void (*onHostReceiveRemoteInput)(const InputFrame& in) = nullptr;
    // Client-only: authoritative snapshot to apply (reconciliation is your code)
    void (*onClientApplySnapshot)(const Snapshot& snap) = nullptr;
    // Host-only: fire event to resolve with lag-comp + PhysX
    void (*onHostFireEvent)(const FireEvent& fe) = nullptr;
};

class NetSystem {
public:
    bool host(uint16_t port = kDefaultPort);
    bool join(const char* host, uint16_t port = kDefaultPort);
    void shutdown();
    Role role() const { return m_role; }
    bool connected() const { return m_connected; }
    void setCallbacks(const NetCallbacks& cb) { m_cb = cb; }
    // Call once per frame
    void pump(double nowMs);
    // Client: submit local input each tick
    void captureLocalInput(const InputFrame& in);
    // Client/Host: report a fire event (host will resolve)
    void sendFireEvent(const FireEvent& fe);
    // Host: after PhysX step each tick
    void hostSubmitAuthoritativeStates(uint32_t tick,
                                       const PlayerState& hostState,
                                       const PlayerState& clientState);
    // Host-side history query (for lag-comp helpers in your game)
    bool getHistoricalSnapshot(uint32_t tick, Snapshot& out) const;
    // Stats
    double rttMs() const { return m_clock.rttMs; }
private:
    void sendPacket(Msg m, const ByteBuffer& payload);
    void handlePacket(const uint8_t* data, int len, double nowMs);
    void sendPing(double nowMs);
    void maybeSendSnapshot(double nowMs);
private:
    Role         m_role = Role::Offline;
    bool         m_connected = false;
    TransportUDP m_udp;
    uint32_t     m_seq = 1;
    NetCallbacks m_cb{};
    ClockSync    m_clock;
    double       m_lastPingSentMs = -1.0;
    // Host snapshot timing/state
    Snapshot     m_lastAuthoritative{};
    double       m_lastSnapshotSentMs = 0.0;
    // Host history buffer (tick -> both players)
    Snapshot     m_hist[kMaxHistoryTicks]{};
    bool         m_histInit = false;
    uint32_t     m_histBaseTick = 0;
};
} // namespace hellnet
