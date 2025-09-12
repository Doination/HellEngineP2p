#pragma once
namespace hellnet {
// PoC: simple EMA-based RTT/offset smoother
struct ClockSync {
    double rttMs = 0;
    double oneWayMs = 0;
    double offsetMs = 0; // remote_time - (local_time + oneWay)
    void on_ping_result(double rtt, double localSentMs, double remoteRecvMs) {
        rttMs   = 0.9 * rttMs + 0.1 * rtt;
        oneWayMs = rttMs * 0.5;
        double est = remoteRecvMs - (localSentMs + oneWayMs);
        offsetMs = 0.9 * offsetMs + 0.1 * est;
    }
    double local_to_remote(double localMs) const { return localMs + offsetMs; }
    double remote_to_local(double remoteMs) const { return remoteMs - offsetMs; }
};
} // namespace hellnet
