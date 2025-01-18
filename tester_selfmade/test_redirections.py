#!/usr/bin/env python3

import os
import sys
from test_utils import run_shell_command, clean_shell_output, GREEN, RED, NC

def run_redirection_test(test_name, command, expected_file):
    """
    Run redirection test comparing minishell and bash behavior.
    
    Args:
        test_name (str): Name of the test to be displayed
        command (str): Command to execute in both shells
        expected_file (str): File to check for redirected output
    """
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
        return True
    else:
        print(f"{RED}FAIL{NC}")
        return False
    sys.stdout.flush()