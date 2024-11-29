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

def clean_shell_output(command, output, shell_type='bash'):
    """
    Clean shell output by removing prompts, color codes, and command echoes.
    
    Args:
        command (str): The original command that was executed
        output (str): The raw output from the shell
        shell_type (str): Either 'bash' or 'minishell' to determine prompt pattern
    
    Returns:
        str: Cleaned output with unnecessary lines removed
    """
    # Remove color codes
    output = re.sub(r'\x1B\[[0-?]*[ -/]*[@-~]', '', output)
    lines = output.splitlines()
    
    # Get both original command and processed version (tabs replaced with spaces)
    processed_command = command.replace('\t', ' ')
    
    # Define prompt patterns based on shell type
    if shell_type == 'bash':
        prompt_pattern = r'^.*@.*:\s*.*\$\s*'  # Matches bash-style prompts
        version_pattern = r'^bash-\d+\.\d+'
        patterns_to_remove = [
            prompt_pattern,
            version_pattern,
            f'^{re.escape(command)}$',
            f'^{re.escape(processed_command)}$',
            f'^{prompt_pattern}{re.escape(command)}$',
            f'^{prompt_pattern}{re.escape(processed_command)}$'
        ]
    else:  # minishell
        prompt_pattern = r'^minishell\$\s*'
        patterns_to_remove = [
            prompt_pattern,
            f'^{re.escape(command)}$',
            f'^{re.escape(processed_command)}$',
            f'^{prompt_pattern}{re.escape(command)}$',
            f'^{prompt_pattern}{re.escape(processed_command)}$'
        ]
    
    # Filter out unwanted lines
    lines = [
        line for line in lines 
        if not (
            line == command or
            line == processed_command or
            line == 'exit' or
            any(re.match(pattern, line) for pattern in patterns_to_remove)
        )
    ]
    
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

	# In run_test function:
	minishell_output_clean = clean_shell_output(command, minishell_output, shell_type='minishell')
	bash_output_clean = clean_shell_output(command, bash_output, shell_type='bash')

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

def main():
    global PASS, FAIL

    # Basic command tests
    run_test("Simple Echo", "echo hello world")
    run_test("Echo multiple words", "echo first second third fourth")
    run_test("Echo with numbers", "echo 123 456 789")
    # run_test("Echo special characters", "echo hello! @#$%^&*()")
    run_test("Echo with unicode", "echo ñ å ç ë î ø")
    run_test("PWD", "pwd")
    run_test("PWD with arguments", "pwd -L")
    run_test("PWD with invalid flag", "pwd --invalid")
    run_test("CD basic", "cd .. && pwd")
    run_test("CD to home", "cd ~ && pwd")
    run_test("CD with no args", "cd && pwd")
    run_test("Exit basic", "exit")
    run_test("Exit with status", "exit 42")
    run_test("Exit with invalid status", "exit abc")
    run_test("Exit status checking", "ls nonexistentfile; echo $?")
    run_test("Exit status success", "ls /; echo $?")
    run_test("Multiple commands status", "true; echo $? && false; echo $?")

    # Path execution tests
    run_test("Absolute path basic", "/bin/ls /")
    run_test("Absolute path home", "/bin/ls $HOME")
    run_test("Absolute path with args", "/usr/bin/wc -l /etc/passwd")
    run_test("Absolute path nonexistent", "/nonexistent/program")
    run_test("PATH basic execution", "ls")
    run_test("PATH with arguments", "ls -la")
    run_test("PATH multiple commands", "ls && pwd && whoami")
    run_test("PATH command not found", "nonexistentcommand")
    run_test("Relative path basic", "./test_script.sh")
    run_test("Relative path with dots", "../test_dir/../test_script.sh")
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
    run_test("Single quotes special chars", "echo '$USER $HOME'")
    run_test("Single quotes with path", "echo '/bin/ls'")
    run_test("Single quotes preserving spaces", "echo '    lots   of   spaces    '")
    run_test("Single quotes with numbers", "echo '12 34  56'")
    run_test("Single quotes with symbols", "echo '!@#$%^&*()'")
    run_test("Double quotes basic", 'echo "Hello    World"')
    run_test("Double quotes empty", 'echo ""')
    run_test("Double quotes with expansion", 'echo "Current user: $USER"')
    run_test("Double quotes multiple vars", 'echo "User: $USER Home: $HOME Shell: $SHELL"')
    run_test("Double quotes with path", 'echo "/bin/ls"')
    run_test("Double quotes preserving spaces", 'echo "    lots   of   spaces    "')
    run_test("Mixed quotes", "echo '\"nested quotes\"'")
    run_test("Mixed quotes complex", "echo \"'single'\" '\"double\"'")
    run_test("Alternate quotes", "echo 'this' \"that\" 'other' \"another\"")
    run_test("Empty quotes", "echo '' \"\"")
    run_test("Quotes with spaces", "echo '   '  \"   \"")
    run_test("Quotes with tabs", "echo '\t\t'  \"\t\t\"")
    run_test("Quotes with newlines", "echo '\n\n'  \"\n\n\"")

    # Empty and special cases
    run_test("Empty command", "")
    run_test("Single space", " ")
    run_test("Multiple spaces", "     ")
    run_test("Multiple tabs", "\t\t\t")
    run_test("Mixed whitespace", " \t \t \t ")
    run_test("Semicolon only", ";")
    run_test("Multiple semicolons", ";;;")
    run_test("Spaces between semicolons", "; ; ;")
    run_test("Command with multiple spaces", "echo    hello     world")
    run_test("Command with multiple tabs", "echo\thello\tworld")
    run_test("Command with mixed whitespace", "echo \t hello \t world")
    run_test("Only redirections", "> out")
    run_test("Only pipe", "|")
    run_test("Multiple pipes", "|||")
    run_test("Backslash at end", "echo hello\\")
    run_test("Backslash with space", "echo hello\\ world")
    run_test("Multiple backslashes", "echo hello\\\\world")
    run_test("Special chars sequence", "echo !@#$%^&*()_+-=[]{}\\|;:'\",.<>/?`~")

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