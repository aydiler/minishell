#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

PASS=0
FAIL=0

MINISHELL_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TEST_SCRIPT="$MINISHELL_DIR/test_script.sh"

cleanup() {
    echo -e "\n\nInterrupted. Cleaning up..."
    rm -f /tmp/minishell_* "$TEST_SCRIPT" commands.txt
    echo -e "\nPartial Results:"
    echo -e "${GREEN}Passed: $PASS${NC}"
    echo -e "${RED}Failed: $FAIL${NC}"
    exit 1
}

trap cleanup SIGINT SIGTERM

run_test() {
    local test_name="$1"
    local command="$2"

    echo -n "Testing $test_name... "

    local minishell_output=$(mktemp)
    local bash_output=$(mktemp)

    (cd "$MINISHELL_DIR" && printf '%s\n' "$command" | ./minishell 2>&1 | clean_minishell_output "$command" > "$minishell_output")
    minishell_exit=$?

    (cd "$MINISHELL_DIR" && printf '%s\n' "$command" | bash 2>&1 > "$bash_output")
    bash_exit=$?

    echo
    echo "Command: $command"
    echo -e "Expected (bash):\n$(cat $bash_output)"
    echo -e "Got (minishell):\n$(cat $minishell_output)"
    echo "Expected exit code: $bash_exit"
    echo "Got exit code: $minishell_exit"

    if diff -q "$minishell_output" "$bash_output" >/dev/null && [ $minishell_exit -eq $bash_exit ]; then
        echo -e "${GREEN}PASS${NC}"
        ((PASS++))
    else
        echo -e "${RED}FAIL${NC}"
        ((FAIL++))
    fi

    rm -f "$minishell_output" "$bash_output"
}

clean_minishell_output() {
    local command_escaped=$(printf '%s\n' "$1" | sed 's/[][\.^$(){}?+|/]/\\&/g')
    sed -e "/^minishell\\\$ /d" -e "/^$command_escaped\$/d" -e "/^exit$/d" -e '/^$/d' -e 's/[[:space:]]$//'
}

test_history() {
    echo -n "Testing history... "
    
    echo "echo first" > commands.txt
    echo "echo second" >> commands.txt
    
    result=$(cd "$MINISHELL_DIR" && cat commands.txt | ./minishell)
    
    if echo "$result" | grep -q "first" && echo "$result" | grep -q "second"; then
        echo -e "${GREEN}PASS${NC}"
        ((PASS++))
    else
        echo -e "${RED}FAIL${NC}"
        ((FAIL++))
    fi
    rm -f commands.txt
}

test_signals() {
    echo -n "Testing signals... "
    
    (cd "$MINISHELL_DIR" && ./minishell) &
    pid=$!
    sleep 1
    kill -SIGINT $pid
    wait $pid 2>/dev/null
    int_status=$?
    
    (cd "$MINISHELL_DIR" && echo "" | ./minishell) &
    pid=$!
    sleep 1
    wait $pid 2>/dev/null
    eof_status=$?
    
    (cd "$MINISHELL_DIR" && ./minishell) &
    pid=$!
    sleep 1
    kill -SIGQUIT $pid
    wait $pid 2>/dev/null
    quit_status=$?
    
    if [ $int_status -eq 130 ] && [ $eof_status -eq 0 ] && [ $quit_status -ne 131 ]; then
        echo -e "${GREEN}PASS${NC}"
        ((PASS++))
    else
        echo -e "${RED}FAIL${NC}"
        ((FAIL++))
    fi
}

# Basic command tests
run_test "Echo" "echo hello world"
run_test "PWD" "pwd"
run_test "Exit status" "ls nonexistentfile; echo \$?"

# Path execution tests
run_test "Absolute path" "/bin/ls /"
run_test "PATH execution" "ls"

# Create and test relative path
cat > "$TEST_SCRIPT" << 'EOF'
#!/bin/bash
echo "Test script executed"
EOF
chmod +x "$TEST_SCRIPT"
run_test "Relative path" "./test_script.sh"

# Quote tests
run_test "Single quotes" "echo 'Hello    World'"
run_test "Single quotes with special chars" "echo '$USER $HOME'"
run_test "Single quotes with path" "echo '/bin/ls'"
run_test "Nested single quotes in double quotes" "echo \"text 'nested' text\""

run_test "Double quotes" "echo \"Hello    World\""
run_test "Double quotes with expansion" "echo \"Current user: $USER\""
# run_test "Double quotes with special chars" "echo \"* ? [ ] | ; &&\""
run_test "Double quotes with path" "echo \"/bin/ls\""

# Empty and special cases
run_test "Empty command" ""
run_test "Multiple spaces" "echo    hello     world"
command=$'echo\thello\tworld'
run_test "Tab characters" "$command"

# History and signal tests
test_history
test_signals

# Cleanup
rm -f "$TEST_SCRIPT"

# Results
echo -e "\nResults:"
echo -e "${GREEN}Passed: $PASS${NC}"
echo -e "${RED}Failed: $FAIL${NC}"
echo "Total: $((PASS + FAIL))"

exit $((FAIL > 0))