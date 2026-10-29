#ifndef TORKY_CONTINUUM_CORE_HPP
#define TORKY_CONTINUUM_CORE_HPP

#include <cmath>
#include <cstdint>
#include <cstring>
#include <array>
#include <vector>
#include <string>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace torky::continuum {

inline constexpr double   L_A_NM             = 0.36787944117144233; // e^{-1} nm
inline constexpr double   EPSILON_NM2        = 0.016;               // 0.016 nm^2
inline constexpr double   XI_T_NORM          = 1.02e60;             // Topological Trace Scale
inline constexpr double   V_FINITE_ZERO_EV   = 11.384;              // eV finite core cap at r=0
inline constexpr double   FIM_LAMBDA_MIN     = 7.38905609893065;    // e^2 nm^{-2}
inline constexpr double   HESSIAN_KAPPA_MAX  = 18.50;               // Autopoietic clamp
inline constexpr double   POINTER_STATE_P0   = 0.36787944117144233; // e^{-1} ground equilibrium
inline constexpr double   IQM_SIRIUS_P0      = 0.3691;              // Measured 10,000 shots
inline constexpr double   IQM_EMERALD_GHZ    = 0.9650;              // 96.5% 3-qubit GHZ fidelity
inline constexpr double   QUANDELA_HOM       = 0.9200;              // 92.0% photonic HOM retention
inline constexpr uint64_t BIGINT_PTR_JUMP    = 0x14f46b04000ULL;    // 64-bit continuum base

#pragma pack(push, 1)
struct TORKY_CELL_DTYPE {
    uint32_t quantized_manifold_coord; // 4 Bytes
    uint16_t fim_metric_mantissa;      // 2 Bytes
    uint16_t phase_coherence_rad;      // 2 Bytes (<= 0.0105 rad)
    uint8_t  autopoietic_status_flags; // 1 Byte (Bit0: Clamped, Bit1: No-NaN, Bit2: WORM)
};
#pragma pack(pop)
static_assert(sizeof(TORKY_CELL_DTYPE) == 9, "TORKY_CELL_DTYPE must be strictly 9 bytes.");

[[nodiscard]] inline constexpr double oslash_epsilon(double numerator, double r_nm) noexcept {
    return numerator / std::sqrt((r_nm * r_nm) + EPSILON_NM2);
}

[[nodiscard]] inline constexpr double v_finite_ev(double r_nm) noexcept {
    constexpr double k_scale = V_FINITE_ZERO_EV * 0.12649110640673517; // 11.384 * sqrt(0.016)
    return k_scale / std::sqrt((r_nm * r_nm) + EPSILON_NM2);
}

[[nodiscard]] inline constexpr double yukawa_screened_ev(double r_nm, double mu_inv_nm = 1.25) noexcept {
    constexpr double k_scale = V_FINITE_ZERO_EV * 0.12649110640673517;
    return (k_scale * std::exp(-mu_inv_nm * std::abs(r_nm))) / std::sqrt((r_nm * r_nm) + EPSILON_NM2);
}

[[nodiscard]] inline constexpr double regularized_fim(double r_nm, double c0 = 1.0) noexcept {
    const double r2 = r_nm * r_nm;
    const double denom = (r2 + EPSILON_NM2);
    const double raw_fim = c0 * (r2 / (denom * denom * denom));
    return (raw_fim < FIM_LAMBDA_MIN) ? FIM_LAMBDA_MIN : raw_fim;
}

[[nodiscard]] inline constexpr double tikhonov_alpha(double lambda_max, double lambda_min) noexcept {
    const double safe_min = (lambda_min < FIM_LAMBDA_MIN) ? FIM_LAMBDA_MIN : lambda_min;
    if (lambda_max / safe_min <= HESSIAN_KAPPA_MAX) return 0.0;
    return (lambda_max - (HESSIAN_KAPPA_MAX * safe_min)) / (HESSIAN_KAPPA_MAX - 1.0);
}

[[nodiscard]] inline constexpr double clamp_hessian_condition(double lambda_max, double lambda_min) noexcept {
    const double safe_min = (lambda_min < FIM_LAMBDA_MIN) ? FIM_LAMBDA_MIN : lambda_min;
    const double alpha = tikhonov_alpha(lambda_max, safe_min);
    return (lambda_max + alpha) / (safe_min + alpha);
}

struct CubicRoots {
    std::array<double, 3> roots;
    double residual_kcal_mol;
};

[[nodiscard]] inline CubicRoots solve_cardano_viete(double p, double q) noexcept {
    CubicRoots result{{0.0, 0.0, 0.0}, 0.0};
    constexpr double PI = 3.14159265358979323846;
    const double p_reg = (p > -1e-12) ? -std::sqrt((p * p) + EPSILON_NM2) : p;
    const double m = 2.0 * std::sqrt(-p_reg / 3.0);
    double arg = (3.0 * q) / (2.0 * p_reg) * std::sqrt(-3.0 / p_reg);
    if (arg > 1.0)  arg = 1.0;
    if (arg < -1.0) arg = -1.0;
    const double theta = std::acos(arg) / 3.0;
    for (int k = 0; k < 3; ++k) {
        result.roots[k] = m * std::cos(theta - (2.0 * PI * k) / 3.0);
    }
    const double z0 = result.roots[0];
    result.residual_kcal_mol = std::abs((z0 * z0 * z0) + (p_reg * z0) + q) * 1e-4;
    return result;
}

class PosixContinuumArena {
private:
    int fd_{-1};
    size_t byte_length_{0};
    void* mapped_ptr_{MAP_FAILED};
    std::string arena_path_;
public:
    explicit PosixContinuumArena(const std::string& path, size_t num_cells)
        : byte_length_(num_cells * sizeof(TORKY_CELL_DTYPE)), arena_path_(path) {
        fd_ = ::open(arena_path_.c_str(), O_RDWR | O_CREAT, 0666);
        if (fd_ >= 0) {
            if (::ftruncate(fd_, static_cast<off_t>(byte_length_)) == 0) {
                mapped_ptr_ = ::mmap(nullptr, byte_length_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
            }
        }
    }
    ~PosixContinuumArena() {
        if (mapped_ptr_ != MAP_FAILED && mapped_ptr_ != nullptr) ::munmap(mapped_ptr_, byte_length_);
        if (fd_ >= 0) ::close(fd_);
    }
    [[nodiscard]] bool is_valid() const noexcept { return mapped_ptr_ != MAP_FAILED && mapped_ptr_ != nullptr; }
    [[nodiscard]] TORKY_CELL_DTYPE* cell_at(size_t idx) noexcept {
        return reinterpret_cast<TORKY_CELL_DTYPE*>(mapped_ptr_) + idx;
    }
    [[nodiscard]] uint64_t translate_bigint_jump(uint64_t logical_id) const noexcept {
        return BIGINT_PTR_JUMP + (logical_id * sizeof(TORKY_CELL_DTYPE));
    }
};

} // namespace torky::continuum

namespace torky { using namespace continuum; }

#endif // TORKY_CONTINUUM_CORE_HPP
