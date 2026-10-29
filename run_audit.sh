#!/usr/bin/env bash
set -euo pipefail
echo "=================================================="
echo " [AT-ACE UBUNTU PIPELINE] Running 7-Pass Verification"
echo "=================================================="
python3 custom_input_runner.py
echo "[PASS] Audit completed successfully."
