#!/usr/bin/env python3

import os
import sys
import subprocess
import signal
import time
import re
import pty
import select
import subprocess

# Color codes for output
GREEN = '\033[0;32m'
RED = '\033[0;31m'
YELLOW = '\033[0;33m'
NC = '\033[0m'
TEST_SCRIPT = 'test_script.sh'
MINISHELL_DIR = os.path.join(os.path.dirname(os.getcwd()), './')

PASS = 0
FAIL = 0
DEBUG = False

from test_history import test_history
from test_signals import test_signals

def debug_print(*args, **kwargs):
	if DEBUG:
		print(f"{YELLOW}[DEBUG]{NC}", *args, **kwargs)
		sys.stdout.flush()

def cleanup():
	# Remove any test files that might have been created
	test_files = ['testfile.txt', 'existing.txt', 'permissions.txt']
	for file in test_files:
		try:
			if os.path.exists(file):
				os.remove(file)
		except:
			pass
			
	print("\n\nInterrupted. Cleaning up...")
	print("\nPartial Results:")
	print(f"{GREEN}Passed: {PASS}{NC}")
	print(f"{RED}Failed: {FAIL}{NC}")
	sys.stdout.flush()

def signal_handler(sig, frame):
	cleanup()
	sys.exit(1)

signal.signal(signal.SIGINT, signal_handler)
signal.signal(signal.SIGTERM, signal_handler)

def normalize_error_message(line):
	# Remove shell-specific prefixes with more comprehensive patterns
	line = re.sub(r'^(shell|bash|bash-[0-9.]+|minishell|\$|>|\+):\s*', '', line)
	
	# Remove trailing shell prompts
	line = re.sub(r'(bash-[0-9.]+\$|minishell\$)\s*$', '', line)
	
	return line.strip()

def clean_shell_output(command, output, shell_type='bash'):
	# Remove ANSI escape sequences
	output = re.sub(r'\x1B\[[0-?]*[ -/]*[@-~]', '', output)
	
	# Standardize line endings
	output = re.sub(r'\r\n', '\n', output)
	
	# Remove control characters while preserving intentional whitespace
	output = re.sub(r'[\x00-\x08\x0b-\x1f\x7f]', '', output)
	
	lines = []
	for line in output.splitlines():
		# Skip empty lines after stripping
		if not line.strip():
			continue
			
		# First remove prompts
		for prompt in ['minishell$ ', 'minishell$', 'bash-5.1$ ', 'bash-5.1$', '$ ', '$']:
			line = line.replace(prompt, '')
		line = line.strip()
		
		# Then normalize error messages
		line = normalize_error_message(line)
		
		# Skip exit commands
		if line == 'exit':
			continue
			
		# Skip lines that start with the command (ignoring whitespace differences)
		cmd_parts = command.strip().split()
		line_parts = line.strip().split()
		if cmd_parts and line_parts and all(c in line_parts for c in cmd_parts):
			continue
		
		# Only append non-empty lines after all processing
		if line.strip():
			lines.append(line)
	
	return lines

def run_shell_command(command, shell_type='bash'):
	"""Runs a command in specified shell and returns output and exit code"""
	debug_print(f"Starting {shell_type} command execution")

	env = os.environ.copy()
	env['TERM'] = 'dumb'

	# Set inputrc for minishell to disable tab completion
	if shell_type == 'minishell':
		inputrc_content = "set disable-completion on\n"
		with open('minishell_inputrc', 'w') as f:
			f.write(inputrc_content)
		env['INPUTRC'] = os.path.abspath('minishell_inputrc')

	shell_cmd = None
	if shell_type == 'bash':
		shell_cmd = ['bash', '--norc', '--noediting', '--posix']
	else:
		shell_cmd = [os.path.join(os.path.dirname(os.getcwd()), 'minishell')]

	master, slave = pty.openpty()
	try:
		debug_print(f"Launching {shell_type} process")
		subprocess.run(['stty', '-echo'], stdin=master)
		proc = subprocess.Popen(
			shell_cmd,
			stdin=slave,
			stdout=slave,
			stderr=slave,
			env=env,
			preexec_fn=os.setsid
		)

		# Wait for initial prompt
		time.sleep(0.01)

		# Send command
		debug_print(f"Sending command: {repr(command)}")
		os.write(master, command.encode() + b'\n')

		# Allow time for command execution and collect output
		output = b""
		while True:
			try:
				ready, _, _ = select.select([master], [], [], 0.03)
				if not ready:
					break

				chunk = os.read(master, 4096)
				if not chunk:
					break
				output += chunk
				debug_print(f"Received chunk: {repr(chunk)}")
			except (OSError, IOError) as e:
				debug_print(f"Error reading output: {e}")
				break

		# Send exit command
		os.write(master, b"exit\n")
		proc.wait()

		# Normalize and decode final output
		final_output = output.replace(b'\r\n', b'\n').decode('utf-8', errors='replace')
		debug_print(f"Final output: {repr(final_output)}")

		return final_output, proc.returncode

	finally:
		os.close(master)
		os.close(slave)


def run_test(test_name, command):
	"""Runs a test comparing minishell and bash behavior"""
	global PASS, FAIL
	print(f"\nTesting {test_name}...")
	sys.stdout.flush()

	debug_print(f"Starting test: {test_name}")
	debug_print(f"Command: {repr(command)}")

	# Run both shells
	minishell_output, minishell_exit = run_shell_command(command, 'minishell')
	bash_output, bash_exit = run_shell_command(command, 'bash')

	# Clean and normalize outputs
	minishell_output_clean = clean_shell_output(command, minishell_output, 'minishell')
	bash_output_clean = clean_shell_output(command, bash_output, 'bash')

	# Print test information
	print(f"Command: {repr(command)}")
	print(f"Expected (bash):", bash_output_clean)
	print(f"Got (minishell):", minishell_output_clean)
	print(f"Expected exit code: {bash_exit}")
	print(f"Got exit code: {minishell_exit}")
	sys.stdout.flush()

	# Compare results - only comparing actual output, not command lines
	if minishell_output_clean == bash_output_clean and minishell_exit == bash_exit:
		print(f"{GREEN}PASS{NC}")
		PASS += 1
	else:
		print(f"{RED}FAIL{NC}")
		FAIL += 1
	sys.stdout.flush()
 
def test_file_content(filename, expected_content):
	"""Helper function to verify file contents after redirection"""
	try:
		with open(filename, 'r') as f:
			content = f.read().strip()
			return content == expected_content.strip()
	except:
		return False
 
def run_redirection_test(test_name, command, expected_file):
	"""
	Modified version that gets expected content from bash execution
	"""
	global PASS, FAIL
	
	print(f"\nTesting {test_name}...")
	sys.stdout.flush()

	# First run bash to get expected behavior and content
	bash_output, bash_exit = run_shell_command(command, 'bash')
	
	# Get the content created by bash for comparison
	try:
		with open(expected_file, 'r') as f:
			expected_content = f.read()
	except:
		expected_content = ""
		
	# Remove the file before testing minishell
	try:
		os.remove(expected_file)
	except:
		pass
		
	# Now run minishell
	minishell_output, minishell_exit = run_shell_command(command, 'minishell')

	# Clean and normalize outputs
	minishell_output_clean = clean_shell_output(command, minishell_output, 'minishell')
	bash_output_clean = clean_shell_output(command, bash_output, 'bash')

	# Verify command execution
	command_success = (minishell_output_clean == bash_output_clean and 
					  minishell_exit == bash_exit)

	# Verify file contents
	file_exists = os.path.exists(expected_file)
	if file_exists:
		with open(expected_file, 'r') as f:
			actual_content = f.read()
			content_success = actual_content == expected_content
	else:
		content_success = False
		actual_content = ""

	# Print test information
	print(f"Command: {repr(command)}")
	print(f"Expected output:", bash_output_clean)
	print(f"Got output:", minishell_output_clean)
	print(f"Expected exit code: {bash_exit}")
	print(f"Got exit code: {minishell_exit}")
	print(f"File created/modified: {expected_file}")
	print(f"File exists: {file_exists}")
	if file_exists:
		print(f"Expected content: {repr(expected_content)}")
		print(f"Actual content: {repr(actual_content)}")

	# Overall test result
	if command_success and content_success:
		print(f"{GREEN}PASS{NC}")
		PASS += 1
	else:
		print(f"{RED}FAIL{NC}")
		FAIL += 1
	sys.stdout.flush()

def run_input_redirection_test(test_name, input_content, command, expected_output=None):
	global PASS, FAIL
	
	print(f"\nTesting {test_name}...")
	sys.stdout.flush()

	# Create input file
	input_file = "test_input.txt"
	with open(input_file, 'w') as f:
		f.write(input_content)

	# Run command in both shells
	minishell_output, minishell_exit = run_shell_command(command, 'minishell')
	bash_output, bash_exit = run_shell_command(command, 'bash')

	# Clean and normalize outputs
	minishell_output_clean = clean_shell_output(command, minishell_output, 'minishell')
	bash_output_clean = clean_shell_output(command, bash_output, 'bash')

	# If no expected output provided, use bash output as reference
	if expected_output is None:
		expected_output = bash_output_clean
	else:
		expected_output = [expected_output] if isinstance(expected_output, str) else expected_output

	# Print test information
	print(f"Command: {repr(command)}")
	print(f"Input content: {repr(input_content)}")
	print(f"Expected output: {expected_output}")
	print(f"Got output: {minishell_output_clean}")
	print(f"Expected exit code: {bash_exit}")
	print(f"Got exit code: {minishell_exit}")

	# Compare results
	if minishell_output_clean == expected_output and minishell_exit == bash_exit:
		print(f"{GREEN}PASS{NC}")
		PASS += 1
	else:
		print(f"{RED}FAIL{NC}")
		FAIL += 1
	sys.stdout.flush()

	# Cleanup
	try:
		os.remove(input_file)
	except:
		pass

def test_append_redirection(test_name, commands, expected_file, expected_content):
	"""Helper function to test append redirection"""
	global PASS, FAIL
	
	print(f"\nTesting {test_name}...")
	sys.stdout.flush()

	# Clean up test file before starting
	try:
		os.remove(expected_file)
	except:
		pass

	# Run all commands in minishell first
	for command in commands:
		minishell_output, minishell_exit = run_shell_command(command, 'minishell')

	# Check minishell results
	file_exists = os.path.exists(expected_file)
	if file_exists:
		with open(expected_file, 'r') as f:
			actual_content = f.read()
	else:
		actual_content = ""
		
	# Clean up for bash test
	try:
		os.remove(expected_file)
	except:
		pass
		
	# Run all commands in bash to verify expected behavior
	for command in commands:
		bash_output, bash_exit = run_shell_command(command, 'bash')

	# Verify bash created the expected content
	with open(expected_file, 'r') as f:
		bash_content = f.read()
	
	# Cleanup bash file
	try:
		os.remove(expected_file)
	except:
		pass

	# Print test information
	print(f"Commands executed:")
	for cmd in commands:
		print(f"  {repr(cmd)}")
	print(f"File created/modified: {expected_file}")
	print(f"File exists: {file_exists}")
	print(f"Expected content: {repr(expected_content)}")
	print(f"Bash content: {repr(bash_content)}")
	print(f"Actual content: {repr(actual_content)}")

	# Compare results
	content_matches_expected = actual_content.strip() == expected_content.strip()
	content_matches_bash = actual_content.strip() == bash_content.strip()
	
	if content_matches_expected and content_matches_bash:
		print(f"{GREEN}PASS{NC}")
		PASS += 1
	else:
		print(f"{RED}FAIL{NC}")
		FAIL += 1
	sys.stdout.flush()


def test_mixed_redirection(test_name, input_content, commands, expected_file):
	"""Helper function to test combinations of input, output, and append redirection"""
	global PASS, FAIL
	
	print(f"\nTesting {test_name}...")
	sys.stdout.flush()

	# Create input file
	input_file = "test_input.txt"
	with open(input_file, 'w') as f:
		f.write(input_content)

	# Clean up test file before starting
	try:
		os.remove(expected_file)
	except:
		pass

	# Run all commands in minishell
	for command in commands:
		minishell_output, minishell_exit = run_shell_command(command, 'minishell')

	# Check minishell results
	file_exists = os.path.exists(expected_file)
	if file_exists:
		with open(expected_file, 'r') as f:
			actual_content = f.read()
	else:
		actual_content = ""
		
	# Clean up files for bash test
	try:
		os.remove(expected_file)
	except:
		pass
		
	# Run all commands in bash to verify expected behavior
	for command in commands:
		bash_output, bash_exit = run_shell_command(command, 'bash')

	# Verify bash created the expected content
	with open(expected_file, 'r') as f:
		bash_content = f.read()
	
	# Print test information
	print(f"Commands executed:")
	for cmd in commands:
		print(f"  {repr(cmd)}")
	print(f"Input content: {repr(input_content)}")
	print(f"File created/modified: {expected_file}")
	print(f"File exists: {file_exists}")
	print(f"Expected (bash output): {repr(bash_content)}")
	print(f"Got (minishell output): {repr(actual_content)}")

	# Compare results - only compare against bash output
	content_matches_bash = actual_content.strip() == bash_content.strip()
	
	if content_matches_bash:
		print(f"{GREEN}PASS{NC}")
		PASS += 1
	else:
		print(f"{RED}FAIL{NC}")
		FAIL += 1
	sys.stdout.flush()

	# Cleanup
	try:
		os.remove(input_file)
	except:
		pass

def run_shell_command_with_state(commands, shell_type='bash'):
    """Runs commands in shell while maintaining state"""
    env = os.environ.copy()
    env['TERM'] = 'dumb'
    
    shell_cmd = ['bash', '--norc', '--noediting', '--posix'] if shell_type == 'bash' else [os.path.join(os.path.dirname(os.getcwd()), 'minishell')]
    
    master, slave = pty.openpty()
    try:
        subprocess.run(['stty', '-echo'], stdin=master)
        proc = subprocess.Popen(
            shell_cmd,
            stdin=slave,
            stdout=slave,
            stderr=slave,
            env=env,
            preexec_fn=os.setsid
        )
        
        output = b""
        exit_code = 0
        
        # Wait for initial prompt
        time.sleep(0.01)
        # Execute each command in sequence
        for cmd in commands.split(';'):
            os.write(master, f"{cmd.strip()}\n".encode())
            
            while True:
                try:
                    ready, _, _ = select.select([master], [], [], 0.03)
                    if not ready:
                        break
                    chunk = os.read(master, 4096)
                    if not chunk:
                        break
                    output += chunk
                except (OSError, IOError):
                    break
        
        # Get exit status
        os.write(master, b"echo $?\n")
        status_output = os.read(master, 4096)
        try:
            exit_code = int(status_output.decode().split('\n')[1].strip())
        except:
            exit_code = 0
            
        os.write(master, b"exit\n")
        proc.wait()
        
        return output.decode('utf-8', errors='replace'), exit_code
        
    finally:
        os.close(master)
        os.close(slave)

def run_cd_test(test_name, commands):
    """Runs CD command test by checking pwd after execution"""
    global PASS, FAIL
    print(f"\nTesting {test_name}...")
    sys.stdout.flush()

    # Run in minishell
    minishell_output, minishell_exit = run_shell_command_with_state(f"{commands}; pwd", 'minishell')
    minishell_pwd = re.sub(r'(minishell\$|bash-[0-9.]+\$)', '', minishell_output.splitlines()[-2]).strip()
    
    # Run in bash
    bash_output, bash_exit = run_shell_command_with_state(f"{commands}; pwd", 'bash')
    bash_pwd = re.sub(r'(minishell\$|bash-[0-9.]+\$)', '', bash_output.splitlines()[-2]).strip()
    
    print(f"Command: {repr(commands)}")
    print(f"Expected pwd (bash): {bash_pwd}")
    print(f"Got pwd (minishell): {minishell_pwd}")
    print(f"Expected exit code: {bash_exit}")
    print(f"Got exit code: {minishell_exit}")
    
    if minishell_pwd == bash_pwd and minishell_exit == bash_exit:
        print(f"{GREEN}PASS{NC}")
        PASS += 1
    else:
        print(f"{RED}FAIL{NC}")
        FAIL += 1
    sys.stdout.flush()


def main():
	global PASS, FAIL

	subprocess.run(["make", "."], check=True)
	# Empty and special cases
	run_test("Empty command", "")
	run_test("Single space", " ")
	run_test("Multiple spaces", "     ")
	run_test("Multiple tabs", "			")
	run_test("Mixed whitespace", " 	 	 	 ")
	run_test("Command with multiple spaces", "echo    hello     world")
	run_test("Command with multiple tabs", "echo	hello	world")
	run_test("Command with mixed whitespace", "echo 	 hello 	 world")

	# Basic command tests
	run_test("Simple Echo", "echo hello world")
	run_test("Echo multiple words", "echo first second third fourth")
	run_test("Echo with numbers", "echo 123 456 789")
	# run_test("Echo special characters", "echo hello! @#$%^&*()")
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
	run_test("Relative path with dots", "../tester/test_script.sh")
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
 
	# # Test special characters in content
	# run_redirection_test(
	# 	"Special characters redirection",
	# 	"echo '!@#$%^&*()' > testfile.txt",
	# 	"testfile.txt",
	# )

	# Test handling of quotes within content
	run_redirection_test(
		"Quotes in redirected content",
		"echo \"Single quotes 'test' inside\" > testfile.txt",
		"testfile.txt"	)

	# Test handling of multiple spaces
	run_redirection_test(
		"Multiple spaces in content",
		"echo '   multiple    spaces    test   ' > testfile.txt",
		"testfile.txt",
	)

	run_redirection_test(
		"Tab characters in content",
		"echo 'tab	here	there' > testfile.txt",  # Original command
		"testfile.txt",
	)

	# Test numbers and mixed content
	run_redirection_test(
		"Numbers and mixed content",
		"echo '123 abc !@# 456' > testfile.txt",
		"testfile.txt",
	)

	# Test unicode characters
	run_redirection_test(
		"Unicode characters",
		"echo 'Hello 世界 π θ ∞' > testfile.txt",
		"testfile.txt",
	)

	# # Test multiple redirections in sequence
	# run_redirection_test(
	# 	"Sequential redirections",
	# 	"echo 'first' > test1.txt; echo 'second' > test2.txt",
	# 	"test2.txt",
	# )

	# Test redirection with pwd command
	run_redirection_test(
		"PWD output redirection",
		"pwd > testfile.txt",
		"testfile.txt",
	)

	# # Test redirection with command substitution
	# run_redirection_test(
	# 	"Command output with spaces",
	# 	"ls -l / | head -n 1 > testfile.txt",
	# 	"testfile.txt",
	# )

	# # Test redirection with environment variables
	# run_redirection_test(
	# 	"Environment variable content",
	# 	"echo $USER > testfile.txt",
	# 	"testfile.txt",
	# )

	# # Test redirection with path expansion
	# run_redirection_test(
	# 	"Path expansion in content",
	# 	"echo ~/testfile > testfile.txt",
	# 	"testfile.txt",
	# )

	# Test redirection with large content
	run_redirection_test(
		"Large content redirection",
		"python3 -c 'print(\"x\" * 1000)' > testfile.txt",
		"testfile.txt",
	)

	# run_redirection_test(
	# 	"Content with newlines",
	# 	"echo $'line1 line2 line3' > testfile.txt",  # Using $' notation
	# 	"testfile.txt",
	# )

	# Test redirection to file with spaces in name
	run_redirection_test(
		"Filename with spaces",
		"echo 'content' > 'test file.txt'",
		"test file.txt",
	)

	# Test redirection with empty echo
	run_redirection_test(
		"Empty echo redirection",
		"echo -n '' > testfile.txt",
		"testfile.txt",
	)

	# Test redirection with varying line endings
	run_redirection_test(
		"Mixed line endings",
		"printf 'line1\\r line2 line3\\rline4' > mixed_endings.txt",
		"mixed_endings.txt"
	)

	# Test redirection with null characters
	run_redirection_test(
		"Null character handling",
		"printf 'text1\\0text2\\0text3' > nullchar.txt",
		"nullchar.txt"
	)

	print("\nRunning input redirection tests...")
	
	# Basic input redirection
	run_input_redirection_test(
		"Basic input redirection",
		"hello world",
		"cat < test_input.txt"
	)

	# Empty file input
	run_input_redirection_test(
		"Empty file input",
		"",
		"cat < test_input.txt"
	)

	# Multiple lines input
	run_input_redirection_test(
		"Multiple lines input",
		"line1\nline2\nline3",
		"cat < test_input.txt"
	)

	# Large content input
	run_input_redirection_test(
		"Large content input",
		"x" * 1000,
		"cat < test_input.txt"
	)

	# Input with special characters
	run_input_redirection_test(
		"Special characters input",
		"!@#$%^&*()\n<>?\"'[];,./",
		"cat < test_input.txt"
	)

	# Input with spaces and tabs
	run_input_redirection_test(
		"Spaces and tabs input",
		"    spaced    content	tabbed	content    ",
		"cat < test_input.txt"
	)

	# Unicode content
	run_input_redirection_test(
		"Unicode content input",
		"Hello 世界 π θ ∞\nñ å ç ë î ø",
		"cat < test_input.txt"
	)

	# Word count command with input redirection
	run_input_redirection_test(
		"Word count with input redirection",
		"word1 word2\nword3 word4\nword5",
		"wc < test_input.txt"
	)

	# Sort command with input redirection
	run_input_redirection_test(
		"Sort with input redirection",
		"banana\napple\ncherry\ndate",
		"sort < test_input.txt"
	)

	# Head command with input redirection
	run_input_redirection_test(
		"Head with input redirection",
		"line1\nline2\nline3\nline4\nline5",
		"head -n 3 < test_input.txt"
	)

	# Input file with spaces in name
	run_input_redirection_test(
		"Filename with spaces",
		"test content",
		"cat < 'test input.txt'"
	)
 
	# Test with fold command
	run_input_redirection_test(
		"Fold command width limiting",
		"this is a very long line that should be wrapped at specific width",
		"fold -w 20 < test_input.txt"
	)

	# Test with cut command
	run_input_redirection_test(
		"Cut command field extraction",
		"field1,field2,field3\nvalue1,value2,value3",
		"cut -d',' -f2 < test_input.txt"
	)

	# Test with character counting
	run_input_redirection_test(
		"Character count processing",
		"áéíóú\n12345\nabcde",
		"wc -m < test_input.txt"
	)

	print("\nRunning append redirection tests...")

	# Basic append test
	test_append_redirection(
		"Basic append",
		[
			"echo first > testfile.txt",
			"echo second >> testfile.txt"
		],
		"testfile.txt",
		"first\nsecond\n"
	)

	# Multiple appends test
	test_append_redirection(
		"Multiple appends",
		[
			"echo line1 > testfile.txt",
			"echo line2 >> testfile.txt",
			"echo line3 >> testfile.txt"
		],
		"testfile.txt",
		"line1\nline2\nline3\n"
	)

	# Append to empty file
	test_append_redirection(
		"Append to empty file",
		[
			"echo -n '' > testfile.txt",
			"echo appended >> testfile.txt"
		],
		"testfile.txt",
		"appended\n"
	)

	# Append with spaces
	test_append_redirection(
		"Append with spaces",
		[
			"echo first > testfile.txt",
			"echo '   spaced    content   ' >> testfile.txt"
		],
		"testfile.txt",
		"first\n   spaced    content   \n"
	)

	# Append to file with spaces in name
	test_append_redirection(
		"Append to file with spaces in name",
		[
			"echo first > 'test file.txt'",
			"echo second >> 'test file.txt'"
		],
		"test file.txt",
		"first\nsecond\n"
	)
 
	# Test append with interleaved content
	test_append_redirection(
		"Interleaved append",
		[
			"echo 'odd1' > interleaved.txt",
			"echo 'even1' >> interleaved.txt",
			"echo 'odd2' >> interleaved.txt",
			"echo 'even2' >> interleaved.txt"
		],
		"interleaved.txt",
		"odd1\neven1\nodd2\neven2\n"
	)

	# Test append with number sequences
	test_append_redirection(
		"Number sequence append",
		[
			"seq 1 5 > numbers.txt",
			"seq 6 10 >> numbers.txt"
		],
		"numbers.txt",
		"1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n"
	)

	print("\nRunning mixed redirection tests...")

	# Test input redirection with output redirection
	test_mixed_redirection(
		"Input and output redirection",
		"hello world\ntest content",
		["cat < test_input.txt > output.txt"],
		"output.txt"
	)

	# Test input redirection with append
	test_mixed_redirection(
		"Input with append redirection",
		"appended content",
		[
			"echo 'initial' > output.txt",
			"cat < test_input.txt >> output.txt"
		],
		"output.txt"
	)

	# Test sorting with input and output redirection
	test_mixed_redirection(
		"Sort with input and output",
		"zebra\napple\nbanana",
		["sort < test_input.txt > sorted.txt"],
		"sorted.txt"
	)

	# Test word count with mixed redirections
	test_mixed_redirection(
		"Word count with mixed redirections",
		"one two three\nfour five six",
		[
			"wc < test_input.txt > count.txt",
			"echo '=== Word Count ===' >> count.txt"
		],
		"count.txt"
	)

	# Test multiple input files and append
	test_mixed_redirection(
		"Multiple input files and append",
		"file1 content",
		[
			"echo 'file2 content' > input2.txt",
			"cat < test_input.txt > combined.txt",
			"cat < input2.txt >> combined.txt"
		],
		"combined.txt"
	)

	# Test complex mixed redirections
	test_mixed_redirection(
		"Complex mixed redirections",
		"original\ncontent\nto sort",
		[
			"sort < test_input.txt > sorted.txt",
			"echo '---' >> sorted.txt",
			"cat < test_input.txt >> sorted.txt",
			"echo '---' >> sorted.txt",
			"sort -r < test_input.txt >> sorted.txt"
		],
		"sorted.txt"
	)

	# Test mixed redirections with spaces in filenames
	test_mixed_redirection(
		"Mixed redirections with spaced filenames",
		"input data",
		[
			"cat < test_input.txt > 'output file.txt'",
			"echo 'additional data' >> 'output file.txt'"
		],
		"output file.txt"
	)

	# Test redirection with empty input file
	test_mixed_redirection(
		"Mixed redirections with empty input",
		"",
		[
			"echo 'start' > result.txt",
			"cat < test_input.txt >> result.txt",
			"echo 'end' >> result.txt"
		],
		"result.txt"
	)

	# Test redirection with special characters
	test_mixed_redirection(
		"Mixed redirections with special chars",
		"!@#$%^&*()\n<>?\"'[];,./",
		[
			"cat < test_input.txt > special.txt",
			"echo '===' >> special.txt",
			"cat < test_input.txt >> special.txt"
		],
		"special.txt"
	)


	# Test with multiple transforms
	test_mixed_redirection(
		"Multiple transforms",
		"UPPER,lower,MiXeD",
		[
			"tr ',' ' ' < test_input.txt > lines.txt",
			"tr '[:upper:]' '[:lower:]' < lines.txt > lowercase.txt",
			"sort < lowercase.txt > final.txt"
		],
		"final.txt"
	)

	# Test with file content comparison
	test_mixed_redirection(
		"File content comparison",
		"original content",
		[
			"cat < test_input.txt > file1.txt",
			"cat < test_input.txt > file2.txt",
			"cmp file1.txt file2.txt > cmp_result.txt"
		],
		"cmp_result.txt"
	)

	# Test handling of whitespace variations
	test_mixed_redirection(
		"Whitespace handling",
		"  leading spaces\ntrailing spaces  \n\tmixed   spaces   ",
		[
			"sed 's/^[[:space:]]*//;s/[[:space:]]*$//' < test_input.txt > trimmed.txt"
		],
		"trimmed.txt"
	)

	# Test case conversion pipeline
	test_mixed_redirection(
		"Case conversion pipeline",
		"Mixed Case Text\nANOTHER LINE\nlower line",
		[
			"tr '[:upper:]' '[:lower:]' < test_input.txt > lowercase.txt"
		],
		"lowercase.txt"
	)
 
	print("\nRunning pipe tests...")
	
	# Basic pipe test
	run_test(
		"Basic pipe",
		"echo hello world | wc -w"
	)
	
	# Multiple pipes test
	run_test(
		"Multiple pipes",
		"echo hello world | tr 'a-z' 'A-Z' | tr ' ' '_'"
	)
	
	# Pipe with spaces
	run_test(
		"Pipe with spaces",
		"echo    hello    world    |    wc    -w"
	)
	
	# Empty pipe
	run_test(
		"Empty pipe input",
		"echo -n '' | wc -c"
	)
	
	# Large content through pipe
	run_test(
		"Large content pipe",
		"python3 -c 'print(\"x\" * 1000)' | wc -c"
	)
	
	# # Multiple lines through pipe
	# run_test(
	# 	"Multiple lines pipe",
	# 	"printf 'line1 line2 line3' | wc -l"
	# )
	
	# Character transformation
	run_test(
		"Character transformation",
		"echo 'hello world' | tr 'a-z' 'A-Z'"
	)
	
	# Word count pipeline
	run_test(
		"Word count pipeline",
		"echo 'hello world hello' | tr ' ' ' ' | sort | uniq -c"
	)

	
	# Multiple commands with pipes
	run_test(
		"Multiple commands pipe",
		"echo 'hello world' | tr 'a-z' 'A-Z' | tr ' ' '_' | rev"
	)
	
	# Pipeline with special characters
	run_test(
		"Pipeline with special chars",
		"echo '!@#$%^&*()' | tr '!@#' 'ABC'"
	)
	
	# Pipeline with sed
	run_test(
		"Pipeline with sed",
		"echo 'hello world' | sed 's/hello/hi/'"
	)
	
	# Long pipeline
	run_test(
		"Long pipeline",
		"echo 'hello world' | tr 'a-z' 'A-Z' | tr ' ' '_' | rev | tr '_' ' ' | tr 'A-Z' 'a-z'"
	)
	
	# Pipeline with quotes
	run_test(
		"Pipeline with quotes",
		"echo 'hello\"world' | tr '\"' '_'"
	)
	
	# Pipeline with head/tail
	run_test(
		"Pipeline with head/tail",
		"seq 1 10 | head -n 5 | tail -n 2"
	)

	# Test error propagation
	run_test(
		"Error propagation in pipe",
		"ls /nonexistent | wc -l"
	)

	# Test with non-existent commands
	run_test(
		"Non-existent command in pipe",
		"echo hello | nonexistentcmd"
	)

	# Test with failed middle command
	run_test(
		"Failed middle command",
		"echo hello | ls /nonexistent | wc -l"
	)

	print("\nRunning pipe with redirection tests...")
	
	# Test pipe with input redirection
	test_mixed_redirection(
		"Pipe with input redirection",
		"hello world\ntest data",
		["cat < test_input.txt | tr 'a-z' 'A-Z' > output.txt"],
		"output.txt"
	)
	
	# Test pipe with multiple redirections
	test_mixed_redirection(
		"Multiple redirections with pipe",
		"first line\nsecond line",
		[
			"cat < test_input.txt | tr 'a-z' 'A-Z' > output.txt",
			"echo '---' >> output.txt",
			"cat < test_input.txt | tr 'A-Z' 'a-z' >> output.txt"
		],
		"output.txt"
	)
	
	# Test complex pipeline with redirections
	test_mixed_redirection(
		"Complex pipeline with redirections",
		"apple\nbanana\ncherry\napple",
		[
			"cat < test_input.txt | sort | uniq | tr 'a-z' 'A-Z' > sorted.txt",
			"cat sorted.txt | grep 'A' >> final.txt"
		],
		"final.txt"
	)
	
	# Test pipeline with multiple input files
	test_mixed_redirection(
		"Pipeline with multiple inputs",
		"content1\ncontent2",
		[
			"echo 'content3\ncontent4' > input2.txt",
			"cat < test_input.txt input2.txt | sort | uniq > combined.txt"
		],
		"combined.txt"
	)
	
	# Test long pipeline with redirections
	test_mixed_redirection(
		"Long pipeline with redirections",
		"hello\nworld\nhello\ntest",
		[
			"cat < test_input.txt | sort | uniq | tr 'a-z' 'A-Z' | grep 'H' > step1.txt",
			"cat step1.txt | tr ' ' '_' | sort -r >> final.txt"
		],
		"final.txt"
	)
	
	# Test pipeline with word counting and redirections
	test_mixed_redirection(
		"Word count pipeline with redirections",
		"apple banana apple\ncherry banana",
		[
			"cat < test_input.txt | tr ' ' ' ' | sort | uniq -c | sort -nr > counts.txt"
		],
		"counts.txt"
	)
	
	# Test pipeline with sed and redirections
	test_mixed_redirection(
		"Sed pipeline with redirections",
		"hello world\ngoodbye world",
		[
			"cat < test_input.txt | sed 's/world/earth/' | tr 'a-z' 'A-Z' > output.txt"
		],
		"output.txt"
	)
	
	# Test pipeline with head/tail and redirections
	test_mixed_redirection(
		"Head/tail pipeline with redirections",
		"line1\nline2\nline3\nline4\nline5",
		[
			"cat < test_input.txt | head -n 4 | tail -n 2 > middle.txt"
		],
		"middle.txt"
	)
	
	# Test pipeline with file transformations
	test_mixed_redirection(
		"File transformation pipeline",
		"Field1,Value1\nField2,Value2\nField3,Value3",
		[
			"cat < test_input.txt | cut -d',' -f2 | sort > values.txt",
			"cat values.txt | tr 'a-z' 'A-Z' >> final.txt"
		],
		"final.txt"
	)
	
	# Test pipeline with filtering and redirections
	test_mixed_redirection(
		"Filter pipeline with redirections",
		"apple 123\nbanana 456\ncherry 789",
		[
			"cat < test_input.txt | grep '[0-9]' | cut -d' ' -f2 > numbers.txt"
		],
		"numbers.txt"
	)
 
	print("\nRunning CD command tests...")
	
	# Create test directories
	os.makedirs("/tmp/test dir", exist_ok=True)
	os.makedirs("/tmp/test.dir", exist_ok=True)
	
	# Basic CD tests
	run_cd_test("CD to home directory", "cd ~; pwd")
	run_cd_test("CD with no arguments", "cd; pwd")
	run_cd_test("CD to absolute path", "cd /tmp; pwd")
	run_cd_test("CD to relative path", "cd ..; pwd")
	run_cd_test("CD with spaces", "cd '/tmp/test dir'; pwd")
	run_cd_test("CD to nonexistent directory", "cd /nonexistent; pwd")
	run_cd_test("CD to parent directory", "cd ..; pwd")
	run_cd_test("CD with dot directory", "cd .; pwd")
	run_cd_test("CD with special characters", "cd /tmp/test.dir; pwd")
	
	# CD with environment variables
	run_cd_test("CD with HOME variable", "cd $HOME; pwd")
	run_cd_test("CD with PWD variable", "cd $PWD; pwd")
	run_cd_test("CD with undefined variable", "cd $NONEXISTENT; pwd")
	
	# Complex CD scenarios
	run_cd_test("CD chain", "cd /tmp; cd ..; cd /var; pwd")
	run_cd_test("CD with directory creation", "mkdir -p /tmp/newdir; cd /tmp/newdir; pwd")
	run_cd_test("CD to previous dir", "cd /tmp; cd /var; cd -; pwd")

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
 
	# Cleanup test files
	cleanup()

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