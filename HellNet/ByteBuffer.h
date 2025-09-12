#pragma once
#include <vector>
#include <cstdint>
#include <cstring>
#include <type_traits>
namespace hellnet {
struct ByteBuffer {
    std::vector<uint8_t> data;
    void clear() { data.clear(); }
    template<class T> void write_pod(const T& v) {
        static_assert(std::is_trivially_copyable_v<T>, "POD only");
        size_t off = data.size();
        data.resize(off + sizeof(T));
        std::memcpy(data.data() + off, &v, sizeof(T));
    }
    template<class T> bool read_pod(size_t& rpos, T& out) const {
        if (rpos + sizeof(T) > data.size()) return false;
        std::memcpy(&out, data.data() + rpos, sizeof(T));
        rpos += sizeof(T);
        return true;
    }
    void write_bytes(const void* p, size_t n) {
        size_t off = data.size();
        data.resize(off + n);
        std::memcpy(data.data() + off, p, n);
    }
    bool read_bytes(size_t& rpos, void* p, size_t n) const {
        if (rpos + n > data.size()) return false;
        std::memcpy(p, data.data() + rpos, n);
        rpos += n;
        return true;
    }
};
} // namespace hellnet
