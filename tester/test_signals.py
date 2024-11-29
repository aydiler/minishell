#!/usr/bin/env python3

import os
import sys
import subprocess
import time
import signal

# Color codes for output
GREEN = '\033[0;32m'
RED = '\033[0;31m'
NC = '\033[0m'

def test_signals(MINISHELL_DIR, PASS, FAIL):
    """Tests how minishell handles signals:
    - SIGINT (Ctrl+C): Should print a new prompt on a new line
    - EOF (Ctrl+D): Should exit the shell
    - SIGQUIT (Ctrl+\): Should have no action
    """
    print("Testing signals... ", end='', flush=True)

    minishell_path = os.path.join(MINISHELL_DIR, 'minishell')
    if not os.path.exists(minishell_path):
        print(f"\nError: minishell not found at {minishell_path}")
        sys.exit(1)

    try:
        all_tests_passed = True
        errors = []

        # Test SIGINT (Ctrl+C) - Should show new prompt on new line
        proc = subprocess.Popen(
            [minishell_path],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            cwd=MINISHELL_DIR
        )
        time.sleep(0.5)  # Wait for initial prompt
        initial_output = proc.stdout.read1().decode('utf-8')  # Get initial prompt
        
        proc.send_signal(signal.SIGINT)
        time.sleep(0.5)  # Wait for SIGINT handling
        
        output = proc.stdout.read1().decode('utf-8')  # Get output after SIGINT
        if not (output.startswith('\n') or initial_output.endswith('\n')):
            all_tests_passed = False
            errors.append("SIGINT: No new line before prompt")
            
        proc.terminate()
        proc.wait(timeout=2)

        # Test EOF (Ctrl+D) - Should exit shell
        proc = subprocess.Popen(
            [minishell_path],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            cwd=MINISHELL_DIR
        )
        proc.communicate(input=b'')  # Send EOF
        eof_status = proc.returncode

        if eof_status != 0:  # Should exit cleanly
            all_tests_passed = False
            errors.append(f"EOF: Incorrect exit status {eof_status}, expected 0")

        # Test SIGQUIT (Ctrl+\) - Should have no action
        proc = subprocess.Popen(
            [minishell_path],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            cwd=MINISHELL_DIR
        )
        time.sleep(0.5)
        proc.send_signal(signal.SIGQUIT)
        time.sleep(0.5)
        
        # Check if process is still running (it should be)
        if proc.poll() is not None:
            all_tests_passed = False
            errors.append("SIGQUIT: Shell terminated when it shouldn't")
        
        # Cleanup
        proc.terminate()
        proc.wait(timeout=2)

        if all_tests_passed:
            print(f"{GREEN}PASS{NC}")
            PASS += 1
        else:
            print(f"{RED}FAIL{NC}")
            print("\nSignal test details:")
            for error in errors:
                print(f"- {error}")
            FAIL += 1

    except subprocess.TimeoutExpired:
        print(f"{RED}FAIL - Process timeout{NC}")
        FAIL += 1
        proc.kill()
        proc.wait()
    except Exception as e:
        print(f"{RED}FAIL - Unexpected error: {e}{NC}")
        FAIL += 1

    return PASS, FAIL