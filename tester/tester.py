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
	print("\nPartial Results:")
	print(f"{GREEN}Passed: {PASS}{NC}")
	print(f"{RED}Failed: {FAIL}{NC}")

def signal_handler(sig, frame):
	cleanup()
	sys.exit(1)

signal.signal(signal.SIGINT, signal_handler)
signal.signal(signal.SIGTERM, signal_handler)

def normalize_error_message(line):
	"""Normalizes error messages by removing shell-specific prefixes"""
	# Remove 'shell:', 'bash:', or 'minishell:' prefixes
	line = re.sub(r'^(shell|bash|minishell):\s*', '', line)
	return line.strip()

def clean_shell_output(command, output, shell_type='bash'):
	"""Cleans and normalizes shell output for comparison"""
	# Remove color codes
	output = re.sub(r'\x1B\[[0-?]*[ -/]*[@-~]', '', output)
	
	# Split into lines and process each line
	lines = []
	for line in output.splitlines():
		# Skip the command itself and exit
		if line == command or line == 'exit':
			continue
			
		# Skip shell prompts
		if shell_type == 'bash' and re.match(r'^.*@.*:\s*.*\$\s*', line):
			continue
		if shell_type == 'minishell' and line.startswith('minishell$'):
			continue
		
		# Normalize error messages
		line = normalize_error_message(line)
		
		# Only append non-empty lines
		if line:
			lines.append(line)
	
	return lines

def run_minishell_command(command):
	"""Runs a command in minishell and returns output and exit code"""
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
		
		# Convert tabs to spaces and send command
		processed_command = command.replace('\t', ' ')
		os.write(master, f"{processed_command}\n".encode())
		
		# Allow time for command execution
		time.sleep(0.2)
		output = os.read(master, 1024).decode('utf-8')
		
		# Exit cleanly
		os.write(master, b"exit\n")
		proc.wait()
		
		return output.strip(), proc.returncode
	finally:
		os.close(master)
		os.close(slave)

def run_bash_command(command):
	"""Runs a command in bash and returns output and exit code"""
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
		
		# Convert tabs to spaces and send command
		processed_command = command.replace('\t', ' ')
		os.write(master, f"{processed_command}\n".encode())
		
		# Allow time for command execution
		time.sleep(0.2)
		output = os.read(master, 1024).decode('utf-8')
		
		# Exit cleanly
		os.write(master, b"exit\n")
		proc.wait()
		
		return output.strip(), proc.returncode
	finally:
		os.close(master)
		os.close(slave)

def run_test(test_name, command):
	"""Runs a test comparing minishell and bash behavior"""
	global PASS, FAIL
	print(f"Testing {test_name}...")

	# Run both shells
	minishell_output, minishell_exit = run_minishell_command(command)
	bash_output, bash_exit = run_bash_command(command)

	# print raw output
	# print(f"Minishell output:\n{minishell_output}")
	# print(f"Bash output:\n{bash_output}")

	# Clean and normalize outputs
	minishell_output_clean = clean_shell_output(command, minishell_output, shell_type='minishell')
	bash_output_clean = clean_shell_output(command, bash_output, shell_type='bash')

	# Print test information
	print(f"Command: {command}")
	print(f"Expected (bash):\n{bash_output_clean}")
	print(f"Got (minishell):\n{minishell_output_clean}")
	print(f"Expected exit code: {bash_exit}")
	print(f"Got exit code: {minishell_exit}")

	# Compare results
	if minishell_output_clean == bash_output_clean and minishell_exit == bash_exit:
		print(f"{GREEN}PASS{NC}")
		PASS += 1
	else:
		print(f"{RED}FAIL{NC}")
		FAIL += 1

def main():
	global PASS, FAIL

	# Empty and special cases
	run_test("Empty command", "")
	run_test("Single space", " ")
	run_test("Multiple spaces", "     ")
	run_test("Multiple tabs", "\t\t\t")
	run_test("Mixed whitespace", " \t \t \t ")
	run_test("Command with multiple spaces", "echo    hello     world")
	run_test("Command with multiple tabs", "echo\thello\tworld")
	run_test("Command with mixed whitespace", "echo \t hello \t world")

	# Basic command tests
	run_test("Simple Echo", "echo hello world")
	run_test("Echo multiple words", "echo first second third fourth")
	run_test("Echo with numbers", "echo 123 456 789")
	run_test("Echo special characters", "echo hello! @#$%^&*()")
	run_test("Echo with unicode", "echo ñ å ç ë î ø")
	run_test("PWD", "/bin/pwd")
	run_test("PWD with arguments", "/bin/pwd -L")
	run_test("PWD with invalid flag", "/bin/pwd --invalid")

	# Path execution tests
	run_test("Absolute path basic", "/bin/ls /")
	run_test("Absolute path with args", "/usr/bin/wc -l /etc/passwd")
	run_test("Absolute path nonexistent", "/nonexistent/program")
	run_test("PATH basic execution", "ls")
	run_test("PATH with arguments", "ls -la")
	run_test("PATH command not found", "nonexistentcommand")
	run_test("Relative path basic", "./test_script.sh")
	run_test("Relative path with dots", "/tester/../test_script.sh")
	run_test("Relative path nonexistent", "./nonexistent.sh")
	run_test("Current directory command", "test_script.sh")

	# Create and test relative path
	with open(TEST_SCRIPT, 'w') as f:
		f.write("#!/bin/bash\n")
		f.write('echo "Test script executed"\n')
	os.chmod(TEST_SCRIPT, 0o755)
	run_test("Relative path", "./test_script.sh")

	# Quote handling tests
	run_test("Single quotes basic", "echo 'Hello    World'")
	run_test("Single quotes empty", "echo ''")
	run_test("Single quotes with path", "echo '/bin/ls'")
	run_test("Single quotes preserving spaces", "echo '    lots   of   spaces    '")
	run_test("Single quotes with numbers", "echo '12 34  56'")
	run_test("Single quotes with symbols", "echo '!@#$%^&*()'")
	run_test("Double quotes basic", 'echo "Hello    World"')
	run_test("Double quotes empty", 'echo ""')
	run_test("Double quotes with path", 'echo "/bin/ls"')
	run_test("Double quotes preserving spaces", 'echo "    lots   of   spaces    "')

	# # Handle exit status ($?)
	# run_test("Exit basic", "exit")
	# run_test("Exit with status", "exit 42")
	# run_test("Exit with invalid status", "exit abc")
	# run_test("Exit status checking", "ls nonexistentfile; echo $?")
	# run_test("Exit status success", "ls /; echo $?")

	# # \ and ;
	# run_test("Mixed quotes", "echo '\"nested quotes\"'")
	# run_test("Mixed quotes complex", "echo \"'single'\" '\"double\"'")
	# run_test("Alternate quotes", "echo 'this' \"that\" 'other' \"another\"")
	# run_test("Empty quotes", "echo '' \"\"")
	# run_test("Quotes with spaces", "echo '   '  \"   \"")
	# run_test("Quotes with tabs", "echo '\t\t'  \"\t\t\"")
	# run_test("Quotes with newlines", "echo '\n\n'  \"\n\n\"")
	# run_test("Semicolon only", ";")
	# run_test("Multiple semicolons", ";;;")
	# run_test("Spaces between semicolons", "; ; ;")
	# run_test("Backslash at end", "echo hello\\")
	# run_test("Backslash with space", "echo hello\\ world")
	# run_test("Multiple backslashes", "echo hello\\\\world")
	# run_test("Special chars sequence", "echo !@#$%^&*()_+-=[]{}\\|;:'\",.<>/?`~")

	# # Handle environment variables
	# run_test("Absolute path home", "/bin/ls $HOME")
	# run_test("Single quotes special chars", "echo '$USER $HOME'")
	# run_test("Double quotes with expansion", 'echo "Current user: $USER"')
	# run_test("Double quotes multiple vars", 'echo "User: $USER Home: $HOME Shell: $SHELL"')

	# Redirection
	# run_test("Only redirections", "> out")

	# Pipes
	# run_test("Only pipe", "|")
	# run_test("Multiple pipes", "|||")

	# History and signal tests - now using imported functions
	PASS, FAIL = test_history(MINISHELL_DIR, PASS, FAIL)
	PASS, FAIL = test_signals(MINISHELL_DIR, PASS, FAIL)

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