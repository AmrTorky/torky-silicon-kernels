#ifndef TORKY_CONTINUUM_CORE_HPP
#define TORKY_CONTINUUM_CORE_HPP
#include <cmath>
#include <cstdint>
#include <array>
namespace torky {
inline constexpr double L_A_NM            = 0.36787944117144233;
inline constexpr double EPSILON_NM2       = 0.016;
inline constexpr double XI_T_NORM         = 1.02e60;
inline constexpr double V_FINITE_ZERO_EV  = 11.384;
inline constexpr double FIM_LAMBDA_MIN    = 7.38905609893065;
inline constexpr double HESSIAN_KAPPA_MAX = 18.50;
inline constexpr uint64_t BIGINT_PTR_JUMP = 0x14f46b04000ULL;

#pragma pack(push, 1)
struct TORKY_CELL_DTYPE {
    uint32_t quantized_coord;
    uint16_t fim_mantissa;
    uint16_t phase_rad;
    uint8_t flags;
};
#pragma pack(pop)
static_assert(sizeof(TORKY_CELL_DTYPE) == 9, "9-byte invariant violated");

inline double v_finite_ev(double r_nm) noexcept {
    const double k = V_FINITE_ZERO_EV * std::sqrt(EPSILON_NM2);
    return k / std::sqrt((r_nm * r_nm) + EPSILON_NM2);
}
}
#endif
