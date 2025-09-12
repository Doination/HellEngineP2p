#pragma once
#include <cstdint>
namespace hellnet {
// --- Tuning (PoC defaults; safe for LAN/port-forward) ---
constexpr int   kTickRate      = 60;
constexpr float kTickDt        = 1.0f / kTickRate;
constexpr int   kInputHz       = 60;   // send inputs every tick
constexpr int   kSnapshotHz    = 30;   // host sends snapshots ~30 Hz
// --- History (lag-comp buffer on host) ---
constexpr int   kHistorySeconds    = 1;
constexpr int   kMaxHistoryTicks   = kTickRate * kHistorySeconds;
// --- Client prediction buffer ---
constexpr int   kClientInputBuffer = kTickRate * 2; // 2s of inputs
// --- UDP safety ---
constexpr int   kMaxPacketBytes    = 1200; // MTU safe
// --- Port ---
constexpr uint16_t kDefaultPort    = 29050;
// --- Ping cadence ---
constexpr double kPingIntervalMs   = 1000.0;
// --- Wire packing (keep OFF for PoC; flip later without touching gameplay) ---
constexpr int   kEnableWirePacking = 0; // 0 = plain structs on wire, 1 = bit-packed (future)
} // namespace hellnet
