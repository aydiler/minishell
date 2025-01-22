#!/usr/bin/env python3

import os
import sys
import subprocess
import re
import glob

# Color codes for output
GREEN = '\033[0;32m'
RED = '\033[0;31m'
YELLOW = '\033[0;33m'
NC = '\033[0m'

# Global counters for test results
PASS = 0
FAIL = 0

def normalize_error_message(line):
    """Normalize error messages between bash and minishell"""
    # Remove shell name prefix if present
    line = re.sub(r'^(bash|minishell):\s*', '', line)
    
    # Remove the "line X:" prefix that bash adds
    line = re.sub(r'^line \d+:\s*', '', line)
    
    # Remove leading colon and space if present
    line = re.sub(r'^:\s*', '', line)
    
    # Remove trailing shell prompts
    line = re.sub(r'(bash-[0-9.]+\$|minishell\$)\s*$', '', line)
    
    # Remove bash -c prefix if present
    line = re.sub(r'^bash -c\s+', '', line)
    
    return line.strip()

def run_shell_command(command, shell_type='bash'):
    """
    Run a command in specified shell and return output and exit code
    
    Args:
        command (str): Command to execute
        shell_type (str): Either 'bash' or 'minishell'
        
    Returns:
        tuple: (output string, exit code)
    """
    env = os.environ.copy()
    env['TERM'] = 'dumb'

    script = f'''#!/bin/bash
cd {os.getcwd()}

# Execute command and capture all output including prompts
(
    {shell_type if shell_type == 'bash' else os.path.abspath(os.path.join(os.path.dirname(os.getcwd()), './minishell'))} << 'EOF'
{command}
EOF
) > /tmp/cmd_output.$$ 2>&1

echo $? > /tmp/cmd_status.$$

# Filter out shell artifacts but preserve actual output
cat /tmp/cmd_output.$$ | grep -v '^{shell_type}\$' | grep -v '^exit$' | sed 's/{shell_type}\$ exit$//g'

rm -f /tmp/cmd_output.$$
STATUS=$(cat /tmp/cmd_status.$$)
rm -f /tmp/cmd_status.$$
exit $STATUS
'''
    script_path = '/tmp/cmd_script.sh'
    
    try:
        with open(script_path, 'w') as f:
            f.write(script)
        os.chmod(script_path, 0o755)

        result = subprocess.run([script_path], 
                             capture_output=True, 
                             text=True,
                             env=env)
        
        output = result.stdout + result.stderr
        output = output.rstrip()
        exit_code = result.returncode
        
        return output, exit_code
        
    finally:
        # Clean up temporary files
        for pattern in ['/tmp/cmd_script.sh', '/tmp/cmd_output.*', '/tmp/cmd_status.*']:
            try:
                for f in glob.glob(pattern):
                    if os.path.exists(f):
                        os.remove(f)
            except:
                pass

def clean_shell_output(command, output, shell_type='bash'):
    """
    Process shell output to remove prompts and standardize format
    
    Args:
        command (str): Original command that was executed
        output (str): Raw output from shell
        shell_type (str): Either 'bash' or 'minishell'
        
    Returns:
        list: Cleaned and normalized output lines
    """
    # Remove ANSI escape sequences
    output = re.sub(r'\x1B\[[0-?]*[ -/]*[@-~]', '', output)

    # Standardize line endings
    output = re.sub(r'\r\n', '\n', output)

    # Remove control characters while preserving normal whitespace
    output = re.sub(r'[\x00-\x08\x0b-\x1f\x7f]', '', output)

    lines = []
    for line in output.splitlines():
        if not line.strip():
            continue
        
        if re.match(r'^(minishell|bash-[0-9.]+)\$\s*$', line.strip()):
            continue

        line = re.sub(r'^(minishell|bash-[0-9.]+)\$\s*', '', line)
        line = normalize_error_message(line)

        if any(line.strip() == x for x in ['exit', 'echo $?', command.strip()]):
            continue

        if line.strip():
            lines.append(line.strip())

    return lines