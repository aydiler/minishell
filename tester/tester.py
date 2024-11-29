#!/usr/bin/env python3

import os
import sys
import subprocess
import tempfile
import shutil
import signal
import threading
import time
import re
import pty
import select

from test_history import test_history
from test_signals import test_signals

# Color codes for output
GREEN = '\033[0;32m'
RED = '\033[0;31m'
NC = '\033[0m'

PASS = 0
FAIL = 0

MINISHELL_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
TEST_SCRIPT = os.path.join(MINISHELL_DIR, "test_script.sh")

def cleanup():
	print("\n\nInterrupted. Cleaning up...")
	# Remove temporary files if any
	if os.path.exists(TEST_SCRIPT):
		os.remove(TEST_SCRIPT)
	print("\nPartial Results:")
	print(f"{GREEN}Passed: {PASS}{NC}")
	print(f"{RED}Failed: {FAIL}{NC}")

def signal_handler(sig, frame):
	sys.exit(1)

signal.signal(signal.SIGINT, signal_handler)
signal.signal(signal.SIGTERM, signal_handler)


import pexpect

def clean_minishell_output(command, output):
    # Remove color codes
    output = re.sub(r'\x1B\[[0-?]*[ -/]*[@-~]', '', output)
    lines = output.splitlines()
    
    # Get both original command and processed version (tabs replaced with spaces)
    processed_command = command.replace('\t', ' ')
    
    # Remove minishell prompt, exit command, and the input command (both original and processed)
    prompt_pattern = r'^minishell\$\s*'  # Made \s* to catch any trailing space
    
    lines = [line for line in lines if not (
        re.match(prompt_pattern, line) or
        line == 'exit' or
        line == command or
        line == processed_command or  # Also filter processed command
        re.match(f'^{re.escape(command)}$', line) or
        re.match(f'^{re.escape(processed_command)}$', line) or  # Match processed command
        re.match(f'^{prompt_pattern}{re.escape(command)}$', line) or
        re.match(f'^{prompt_pattern}{re.escape(processed_command)}$', line)  # Match prompt+processed command
    )]
    
    # Remove empty lines from start and end while preserving middle ones
    while lines and not lines[0]:
        lines.pop(0)
    while lines and not lines[-1]:
        lines.pop()
    
    return '\n'.join(lines)

def clean_bash_output(command, output):
    # Remove color codes
    output = re.sub(r'\x1B\[[0-?]*[ -/]*[@-~]', '', output)
    lines = output.splitlines()
    
    # Get both original command and processed version (tabs replaced with spaces)
    processed_command = command.replace('\t', ' ')
    
    # Remove bash prompt, version, and the input command (both original and processed)
    prompt_pattern = r'^.*@.*:\s*.*\$\s*'  # Made \s* to catch any trailing space
    bash_version_pattern = r'^bash-\d+\.\d+'
    
    lines = [line for line in lines if not (
        re.match(prompt_pattern, line) or
        re.match(bash_version_pattern, line) or
        line == command or
        line == processed_command or  # Also filter processed command
        re.match(f'^{re.escape(command)}$', line) or
        re.match(f'^{re.escape(processed_command)}$', line) or  # Match processed command
        any(re.match(f'^{p}{re.escape(command)}$', line) for p in [prompt_pattern]) or
        any(re.match(f'^{p}{re.escape(processed_command)}$', line) for p in [prompt_pattern])  # Match prompt+processed command
    )]
    
    # Remove empty lines from start and end while preserving middle ones
    while lines and not lines[0]:
        lines.pop(0)
    while lines and not lines[-1]:
        lines.pop()
    
    return '\n'.join(lines)


def run_minishell_command(command):
	minishell_path = os.path.join(MINISHELL_DIR, 'minishell')
	master, slave = pty.openpty()
	try:
		proc = subprocess.Popen(
			minishell_path,
			stdin=slave,
			stdout=slave,
			stderr=slave,
			preexec_fn=os.setsid,
			cwd=MINISHELL_DIR
		)
		
		# Read initial prompt
		os.read(master, 1024)
		
		# Convert tabs to spaces before sending
		processed_command = command.replace('\t', ' ')
		os.write(master, f"{processed_command}\n".encode())
		
		time.sleep(0.1)
		output = os.read(master, 1024).decode('utf-8')
		
		os.write(master, b"exit\n")
		proc.wait()
		
		return output.strip(), proc.returncode
	finally:
		os.close(master)
		os.close(slave)

def run_bash_command(command):
	"""Runs a single command in bash and returns the output and exit code."""
	master, slave = pty.openpty()
	try:
		proc = subprocess.Popen(
			['bash'],
			stdin=slave,
			stdout=slave,
			stderr=slave,
			preexec_fn=os.setsid,
			cwd=MINISHELL_DIR
		)
		
		# Read initial prompt
		os.read(master, 1024)
		
		# Send command
		processed_command = command.replace('\t', ' ')
		os.write(master, f"{processed_command}\n".encode())
		
		time.sleep(0.1)
		output = os.read(master, 1024).decode('utf-8')
		
		os.write(master, b"exit\n")
		proc.wait()
		
		return output.strip(), proc.returncode
	finally:
		os.close(master)
		os.close(slave)


def run_test(test_name, command):
	"""Runs a test by executing the command in both minishell and bash, comparing the results."""
	global PASS, FAIL
	print(f"Testing {test_name}...")

	# Run minishell
	minishell_output, minishell_exit = run_minishell_command(command)

	# Run bash
	bash_output, bash_exit = run_bash_command(command)

	# Clean both outputs
	minishell_output_clean = clean_minishell_output(command, minishell_output)
	bash_output_clean = clean_bash_output(command, bash_output)

	print(f"Command: {command}")
	print(f"Expected (bash):\n{bash_output_clean}")
	print(f"Got (minishell):\n{minishell_output_clean}")
	print(f"Expected exit code: {bash_exit}")
	print(f"Got exit code: {minishell_exit}")

	if minishell_output_clean == bash_output_clean and minishell_exit == bash_exit:
		print(f"{GREEN}PASS{NC}")
		PASS += 1
	else:
		print(f"{RED}FAIL{NC}")
		FAIL += 1

def run_minishell_commands(commands):
	"""Runs multiple commands in minishell and returns the output and exit code."""
	minishell_path = os.path.join(MINISHELL_DIR, 'minishell')
	master, slave = pty.openpty()
	try:
		proc = subprocess.Popen(
			minishell_path,
			stdin=slave,
			stdout=slave,
			stderr=slave,
			preexec_fn=os.setsid,
			cwd=MINISHELL_DIR
		)
		
		# Read initial prompt
		os.read(master, 1024)
		
		combined_output = ""
		for cmd in commands:
			# Send each command
			processed_command = cmd.replace('\t', ' ')
			os.write(master, f"{processed_command}\n".encode())
			time.sleep(0.1)
			output = os.read(master, 1024).decode('utf-8')
			combined_output += output
			
		# Send exit command
		os.write(master, b"exit\n")
		proc.wait()
		
		return combined_output.strip(), proc.returncode
	finally:
		os.close(master)
		os.close(slave)

def main():
    global PASS, FAIL

    # Basic command tests
    run_test("Echo", "echo hello world")
    run_test("PWD", "pwd")
    run_test("Exit status", "ls nonexistentfile; echo $?")

    # Path execution tests
    run_test("Absolute path", "/bin/ls /")
    run_test("PATH execution", "ls")

    # Create and test relative path
    with open(TEST_SCRIPT, 'w') as f:
        f.write("#!/bin/bash\n")
        f.write('echo "Test script executed"\n')
    os.chmod(TEST_SCRIPT, 0o755)
    run_test("Relative path", "./test_script.sh")

    # Quote tests
    run_test("Single quotes", "echo 'Hello    World'")
    run_test("Single quotes with special chars", "echo '$USER $HOME'")
    run_test("Single quotes with path", "echo '/bin/ls'")
    run_test("Nested single quotes in double quotes", 'echo "text \'nested\' text"')

    run_test("Double quotes", 'echo "Hello    World"')
    run_test("Double quotes with expansion", 'echo "Current user: $USER"')
    run_test("Double quotes with path", 'echo "/bin/ls"')

    # Empty and special cases
    run_test("Empty command", "")
    run_test("Multiple spaces", "echo    hello     world")
    command = 'echo\thello\tworld'
    run_test("Tab characters", command)

    # History and signal tests - now using imported functions
    PASS, FAIL = test_history(MINISHELL_DIR, PASS, FAIL)
    PASS, FAIL = test_signals(MINISHELL_DIR, PASS, FAIL)

    # Cleanup
    if os.path.exists(TEST_SCRIPT):
        os.remove(TEST_SCRIPT)

    # Results
    print("\nResults:")
    print(f"{GREEN}Passed: {PASS}{NC}")
    print(f"{RED}Failed: {FAIL}{NC}")
    print(f"Total: {PASS + FAIL}")

    sys.exit(1 if FAIL > 0 else 0)

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        cleanup()
        sys.exit(1)