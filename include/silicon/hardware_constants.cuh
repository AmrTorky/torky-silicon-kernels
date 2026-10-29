#ifndef TORKY_SILICON_CONSTANTS_CUH
#define TORKY_SILICON_CONSTANTS_CUH

#include <cstdint>

#define TORKY_CUDA_HOST_DEVICE __host__ __device__

namespace torky::silicon {

// ISO/IEC 17025 Hardware Sealed Invariants
constexpr double CUDA_EPSILON_NM2      = 0.016;
constexpr double CUDA_V_FINITE_ZERO_EV = 11.384;
constexpr double CUDA_FIM_FLOOR        = 7.38905609893065;
constexpr double CUDA_HESSIAN_MAX      = 18.50;
constexpr uint64_t CUDA_BIGINT_BASE    = 0x14f46b04000ULL;

#pragma pack(push, 1)
struct CudaContinuumCell {
    uint32_t quantized_coord;
    uint16_t fim_mantissa;
    uint16_t phase_coherence_rad;
    uint8_t  autopoietic_flags;
};
#pragma pack(pop)

static_assert(sizeof(CudaContinuumCell) == 9, "CUDA Cell Struct must be strictly 9 bytes.");

} // namespace torky::silicon

#endif // TORKY_SILICON_CONSTANTS_CUH
