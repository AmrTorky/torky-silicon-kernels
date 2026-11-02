#ifndef TORKY_SILICON_CUDA_KERNELS_HPP
#define TORKY_SILICON_CUDA_KERNELS_HPP

#include <cmath>
#include <cstdint>
#include <algorithm>
#include <vector>

namespace torky::silicon::kernels {

    inline constexpr double EPSILON_NM2       = 0.016;
    inline constexpr double V_FINITE_ZERO_EV  = 11.384;

    #pragma pack(push, 1)
    struct TorkyCellDtype {
        uint8_t  gov_gate;     // TSG Gate Register (0x01 - 0x04)
        int16_t  phase_q16;    // Phase Q16 Fixed-Point
        int16_t  field_q16;    // Field Q16 Fixed-Point
        uint32_t syndrome_u32; // SHA-256 Merkle Syndrome
    };
    #pragma pack(pop)

    static_assert(sizeof(TorkyCellDtype) == 9, "CRITICAL: TorkyCellDtype must be precisely 9 bytes!");

    class SiliconAcceleratorEngine {
    public:
        // FP4/FP8 Low-Precision Quantization Simulation Routine
        static inline uint8_t quantize_fp8(double value) {
            double clamped = std::max(-448.0, std::min(448.0, value));
            return static_cast<uint8_t>((clamped / 448.0) * 127.0 + 128.0);
        }

        static inline double dequantize_fp8(uint8_t code) {
            return ((static_cast<double>(code) - 128.0) / 127.0) * 448.0;
        }

        // Sub-1nm Micro-Synchrotron ISA State Transformer
        static inline double emulate_synchrotron_step(double field_intensity, double cutoff_energy) {
            double safe_intensity = std::max(0.0001, field_intensity);
            double response = std::log(safe_intensity) * (cutoff_energy / 124.8);
            return std::isnan(response) ? 0.0 : response;
        }

        static inline double evaluate_v_finite(double r_nm) {
            return 1.0 / std::sqrt((r_nm * r_nm) + EPSILON_NM2);
        }
    };

    class TsgGateController {
    public:
        static inline bool validate_authorization(uint8_t gate_request, bool bit_tampered) {
            if (bit_tampered) return false;
            return (gate_request >= 0x01 && gate_request <= 0x04);
        }
    };

} // namespace torky::silicon::kernels

#endif // TORKY_SILICON_CUDA_KERNELS_HPP
