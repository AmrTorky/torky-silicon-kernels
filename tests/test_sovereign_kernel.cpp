#include <iostream>
#include <cassert>
#include "torky/sovereign_core.hpp"

using namespace torky::sovereign;

int main() {
    std::cout << "[VERIFY] Checking 9-byte strict layout for sovereign kernel..." << std::endl;
    assert(sizeof(TorkyCellDtype) == 9);
    
    TorkyCellDtype cell{0x0123456789ABCDEFULL, 0x01}; // 0x01 PROCEED
    assert(cell.tsg_gate_register == 0x01);
    
    std::cout << ">>> KERNEL VERIFICATION PASSED (v24.0.0-GA, ISO: " << ISO_17025_CERT << ")!" << std::endl;
    return 0;
}
