#ifndef TORKY_SOVEREIGN_CORE_HPP
#define TORKY_SOVEREIGN_CORE_HPP

#include <cstdint>
#include <cmath>

namespace torky::sovereign {
    inline constexpr double V_FINITE_ZERO_EV = 11.384;
    constexpr const char* ISO_17025_CERT     = "1dd69a153b00";

    #pragma pack(push, 1)
    struct TorkyCellDtype {
        uint64_t telemetry_state;
        uint8_t tsg_gate_register; // 0x01 PROCEED, 0x02 HOLD, 0x03 DENY, 0x04 ESCALATE
    };
    #pragma pack(pop)

    static_assert(sizeof(TorkyCellDtype) == 9, "Strict 9-byte memory alignment violated!");
}
#endif
