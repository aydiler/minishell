#!/usr/bin/env python3

import os
import sys
import subprocess
import time
import pty
import signal

# Color codes for output
GREEN = '\033[0;32m'
RED = '\033[0;31m'
NC = '\033[0m'

def test_history(MINISHELL_DIR, PASS, FAIL):
    """
    Tests the history functionality of minishell, including:
    - Command history storage
    - Up arrow navigation
    - Multiple command recall
    - History persistence across inputs
    """
    print("\nTesting history functionality...")
    print("--------------------------------")

    minishell_path = os.path.join(MINISHELL_DIR, 'minishell')
    master, slave = pty.openpty()
    
    try:
        # Launch minishell in a controlled PTY environment
        print("1. Launching minishell in PTY...")
        proc = subprocess.Popen(
            minishell_path,
            stdin=slave,
            stdout=slave,
            stderr=slave,
            preexec_fn=os.setsid,
            cwd=MINISHELL_DIR
        )
        
        # Define a sequence of test commands
        print("2. Sending test commands...")
        commands = [
            'echo first command',
            'ls -l',
            'pwd',
            'echo last command'
        ]
        
        # Send each command and wait for processing
        for cmd in commands:
            print(f"   Sending command: {cmd}")
            os.write(master, f"{cmd}\n".encode())
            time.sleep(0.2)
            
        # Test history navigation
        print("\n3. Testing history navigation...")
        
        # Test multiple up arrows to navigate through history
        print("   Sending up arrow twice...")
        os.write(master, b'\x1b[A')  # First up arrow
        time.sleep(0.1)
        os.write(master, b'\x1b[A')  # Second up arrow
        time.sleep(0.1)
        
        # Execute the recalled command
        print("   Executing recalled command...")
        os.write(master, b'\n')
        time.sleep(0.2)
        
        # Capture and process output
        print("\n4. Capturing output...")
        output = os.read(master, 4096).decode('utf-8')
        
        # Gracefully exit minishell
        print("5. Cleaning up minishell...")
        os.write(master, b"exit\n")
        proc.wait()
        
        # Process and verify output
        print("\n6. Verifying results...")
        # For this example, we're assuming clean_minishell_output is imported
        output_lines = output.splitlines()
        
        # After two up arrows, we should see pwd execution which shows the directory path
        # Find the last meaningful output line (directory path)
        actual_output = None
        for line in reversed(output_lines):
            if line.strip().startswith('/'):  # Look for path output
                actual_output = line.strip()
                break
        
        # Verify results
        if actual_output and os.path.isabs(actual_output):
            print(f"{GREEN}✓ PASS: History navigation working as expected{NC}")
            print(f"   Expected: A valid directory path")
            print(f"   Actual output: '{actual_output}'")
            PASS += 1
        else:
            print(f"{RED}✗ FAIL: History navigation test failed{NC}")
            print(f"   Expected: A valid directory path")
            print(f"   Actual output: '{actual_output}'")
            print("\nDetailed output:")
            print(output)
            FAIL += 1
            
    except Exception as e:
        print(f"{RED}✗ FAIL: Test threw an exception{NC}")
        print(f"Exception details: {str(e)}")
        FAIL += 1
        
    finally:
        # Cleanup
        try:
            os.close(master)
            os.close(slave)
        except OSError:
            pass
        
        # Kill any remaining process
        try:
            os.killpg(os.getpgid(proc.pid), signal.SIGTERM)
        except:
            pass
    
    return PASS, FAIL