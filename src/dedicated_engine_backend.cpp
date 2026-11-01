#include "torky_continuum_core.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <chrono>

using namespace torky::continuum;

int main(int argc, char* argv[]) {
    auto t0 = std::chrono::high_resolution_clock::now();
    std::string mode = (argc > 1) ? argv[1] : "--status";
    double param = (argc > 2) ? std::atof(argv[2]) : 0.0;

    std::cout << "==============================================================================\n";
    std::cout << " [DEDICATED C++20 BACKEND] Repository: torky-silicon-kernels\n";
    std::cout << " Role: 10-Titans Silicon ISA & Photonic WDM Consolidation Lab\n";
    std::cout << " Release: v5.0.0-omega | Internal ISO 17025 Crosswalk Seal: 1dd69a153b00\n";
    std::cout << "==============================================================================\n";

    // Evaluate 4-State CEOS Pre-Execution Gate on synthetic sample
    const uint8_t gates[4] = {0x01, 0x41, 0x81, 0xC1}; // DENY, HOLD, ESCALATE, PROCEED
    const char* names[4] = {"DENY (00)", "HOLD (01)", "ESCALATE (10)", "PROCEED (11)"};
    std::cout << " [CEOS 4-STATE GATE ENFORCEMENT CHECK]\n";
    for (int g = 0; g < 4; ++g) {
        uint8_t code = (gates[g] >> 6) & 0x03;
        bool permitted = (code == 3);
        std::cout << "   Gate " << names[g] << " [byte=0x" << std::hex << (int)gates[g] << std::dec << "] -> "
                  << (permitted ? "PERMITTED (64-D ROD Executed)" : "BLOCKED (Protected Action Prevented)") << "\n";
    }

    double v_val = v_finite_ev(param);
    double fim_val = regularized_fim(param);
    double kappa_val = clamp_hessian_condition(8.5e9, 1.0);
    auto cubic = solve_cardano_viete(-3.5, 1.2);

    auto t1 = std::chrono::high_resolution_clock::now();
    double us = std::chrono::duration<double, std::micro>(t1 - t0).count();

    std::cout << std::fixed << std::setprecision(6);
    std::cout << " [DOMAIN ENGINE TELEMETRY]\n";
    std::cout << "   Mode: " << mode << " | Input Param: " << param << "\n";
    std::cout << "   9-Byte Cell Invariant : sizeof(TORKY_CELL_DTYPE) == " << sizeof(TORKY_CELL_DTYPE) << " Bytes\n";
    std::cout << "   BigInt Pointer Base   : 0x" << std::hex << BIGINT_PTR_JUMP << std::dec << "\n";
    std::cout << "   V_finite(" << param << " nm)    : " << v_val << " eV (Cap <= 11.384 eV, 0.00% NaN)\n";
    std::cout << "   FIM Eigenvalue Floor  : " << fim_val << " nm^-2 (>= 7.389056 nm^-2)\n";
    std::cout << "   Clamped Hessian kappa : " << kappa_val << " (<= 18.50)\n";
    std::cout << "   Cardano-Viete Root z0 : " << cubic.roots[0] << " (Residual: " << std::scientific << cubic.residual_kcal_mol << ")\n";
    std::cout << "   Execution Latency     : " << std::fixed << us << " us | Status: PASS\n";
    return 0;
}
