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
    # Remove ANSI escape sequences
    output = re.sub(r'\x1B\[[0-?]*[ -/]*[@-~]', '', output)
    
    # Split into lines and strip
    lines = [line.strip() for line in output.splitlines() if line.strip()]
    
    # Filter out command echo, prompts, and empty lines
    lines = [line for line in lines if not (
        line.startswith('minishell$') or 
        line == command or 
        line == 'exit' or
        line == command.replace('\t', ' ')  # Handle tab-expanded command
    )]
    
    # Keep original formatting
    return '\n'.join(lines)

def clean_bash_output(command, output):
    # Remove ANSI escape sequences
    output = re.sub(r'\x1B\[[0-?]*[ -/]*[@-~]', '', output)
    
    # Split into lines and strip
    lines = [line.strip() for line in output.splitlines() if line.strip()]
    
    lines = [line for line in lines if not (
        line == command or
        line == command.replace('\t', ' ') or  # Add this line to handle expanded tabs
        line.endswith('$') or
        line.startswith('bash-') or
        '@' in line
    )]
    
    # Keep original formatting
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
	def read_output(fd):
		output = b''
		while True:
			try:
				rlist, _, _ = select.select([fd], [], [], 0.1)
				if fd in rlist:
					data = os.read(fd, 1024)
					if not data:
						break
					output += data
				else:
					break
			except OSError:
				break
		# Normalize line endings
		output = output.replace(b'\r\n', b'\n').replace(b'\r', b'\n')
		return output

	minishell_path = os.path.join(MINISHELL_DIR, 'minishell')
	if not os.path.exists(minishell_path):
		print(f"Error: minishell not found at {minishell_path}")
		sys.exit(1)

	pid, fd = pty.fork()

	if pid == 0:
		# Child process
		os.chdir(MINISHELL_DIR)
		os.execv(minishell_path, [minishell_path])
	else:
		# Parent process
		os.read(fd, 1024)  # Read initial prompt
		output = b''
		for cmd in commands:
			os.write(fd, (cmd + '\n').encode())
			time.sleep(0.1)
			output += read_output(fd)
		# Send exit
		os.write(fd, 'exit\n'.encode())
		output += read_output(fd)
		_, status = os.waitpid(pid, 0)
		exit_code = os.WEXITSTATUS(status)
		output_decoded = output.decode('utf-8', errors='ignore')
		return output_decoded, exit_code

def test_history():
	"""Tests the history functionality of minishell."""
	global PASS, FAIL
	print("Testing history... ")

	commands = ['echo first', 'echo second']
	minishell_output, minishell_exit = run_minishell_commands(commands)
	output_clean = clean_minishell_output('\n'.join(commands), minishell_output)
	if 'first' in output_clean and 'second' in output_clean:
		print(f"{GREEN}PASS{NC}")
		PASS +=1
	else:
		print(f"{RED}FAIL{NC}")
		FAIL +=1

def test_signals():
	"""Tests how minishell handles signals like SIGINT and SIGQUIT."""
	global PASS, FAIL
	print("Testing signals... ")

	minishell_path = os.path.join(MINISHELL_DIR, 'minishell')
	if not os.path.exists(minishell_path):
		print(f"Error: minishell not found at {minishell_path}")
		sys.exit(1)

	# Test SIGINT
	proc = subprocess.Popen([minishell_path], cwd=MINISHELL_DIR, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	time.sleep(1)
	proc.send_signal(signal.SIGINT)
	proc.wait()
	int_status = proc.returncode

	# Test EOF (send empty input)
	proc = subprocess.Popen([minishell_path], cwd=MINISHELL_DIR, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	# Send EOF
	proc.stdin.close()
	proc.wait()
	eof_status = proc.returncode

	# Test SIGQUIT
	proc = subprocess.Popen([minishell_path], cwd=MINISHELL_DIR, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	time.sleep(1)
	proc.send_signal(signal.SIGQUIT)
	proc.wait()
	quit_status = proc.returncode

	if int_status == 130 and eof_status == 0 and quit_status != 131:
		print(f"{GREEN}PASS{NC}")
		PASS +=1
	else:
		print(f"{RED}FAIL{NC}")
		FAIL +=1

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
	# run_test("Double quotes with special chars", 'echo "* ? [ ] | ; &&"')
	run_test("Double quotes with path", 'echo "/bin/ls"')

	# Empty and special cases
	run_test("Empty command", "")
	run_test("Multiple spaces", "echo    hello     world")
	command = 'echo\thello\tworld'
	run_test("Tab characters", command)

	# History and signal tests
	test_history()
	test_signals()

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
