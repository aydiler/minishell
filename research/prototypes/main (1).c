#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

struct command {
    char *argv[100];
    int argc;
};

// Parse input into command structure
void parse_command(char *line, struct command *cmd) {
    char *token;
    cmd->argc = 0;
    
    token = strtok(line, " \n");
    while(token && cmd->argc < 99) {
        cmd->argv[cmd->argc++] = token;
        token = strtok(NULL, " \n");
    }
    cmd->argv[cmd->argc] = NULL;
}

// Handle built-in commands
int builtin_command(struct command *cmd) {
    if(cmd->argc == 0) return 1;
    
    if(strcmp(cmd->argv[0], "cd") == 0) {
        if(cmd->argc > 1) chdir(cmd->argv[1]);
        return 1;
    }
    if(strcmp(cmd->argv[0], "exit") == 0) {
        exit(0);
    }
    return 0;
}

// Execute external command
void execute_command(struct command *cmd) {
    pid_t pid = fork();
    
    if(pid == 0) {
        execvp(cmd->argv[0], cmd->argv);
        printf("Command not found: %s\n", cmd->argv[0]);
        exit(1);
    } else if(pid > 0) {
        wait(NULL);
    }
}

int main() {
    char line[512];
    struct command cmd;
    
    while(1) {
        printf("$ ");
        fflush(stdout);
        
        if(fgets(line, sizeof(line), stdin) == NULL) break;
        
        parse_command(line, &cmd);
        
        if(!builtin_command(&cmd)) {
            execute_command(&cmd);
        }
    }
    return 0;
}
