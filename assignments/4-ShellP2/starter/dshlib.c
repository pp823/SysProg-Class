#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include "dshlib.h"

// Global variable to store last return code for extra credit
static int last_return_code = 0;

int alloc_cmd_buff(cmd_buff_t *cmd_buff)
{
    if (!cmd_buff) {
        return ERR_MEMORY;
    }
    
    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX);
    if (!cmd_buff->_cmd_buffer) {
        return ERR_MEMORY;
    }
    
    cmd_buff->argc = 0;
    memset(cmd_buff->argv, 0, sizeof(cmd_buff->argv));
    
    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff)
{
    if (!cmd_buff) {
        return ERR_MEMORY;
    }
    
    if (cmd_buff->_cmd_buffer) {
        free(cmd_buff->_cmd_buffer);
        cmd_buff->_cmd_buffer = NULL;
    }
    
    cmd_buff->argc = 0;
    memset(cmd_buff->argv, 0, sizeof(cmd_buff->argv));
    
    return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff)
{
    if (!cmd_buff) {
        return ERR_MEMORY;
    }
    
    cmd_buff->argc = 0;
    memset(cmd_buff->argv, 0, sizeof(cmd_buff->argv));
    if (cmd_buff->_cmd_buffer) {
        cmd_buff->_cmd_buffer[0] = '\0';
    }
    
    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff)
{
    if (!cmd_line || !cmd_buff) {
        return ERR_MEMORY;
    }
    
    clear_cmd_buff(cmd_buff);
    
    char *src = cmd_line;
    char *dst = cmd_buff->_cmd_buffer;
    bool in_quotes = false;
    bool in_arg = false;
    int arg_count = 0;
    
    // Trim leading spaces
    while (*src && isspace(*src)) {
        src++;
    }
    
    while (*src && arg_count < CMD_ARGV_MAX - 1) {
        if (*src == '"' && !in_quotes) {
            in_quotes = true;
            src++;
        } else if (*src == '"' && in_quotes) {
            in_quotes = false;
            src++;
        } else if (isspace(*src) && !in_quotes) {
            if (in_arg) {
                *dst = '\0';
                dst++;
                in_arg = false;
            }
            // Skip multiple spaces
            while (*src && isspace(*src)) {
                src++;
            }
        } else {
            if (!in_arg) {
                cmd_buff->argv[arg_count] = dst;
                arg_count++;
                in_arg = true;
            }
            *dst = *src;
            dst++;
            src++;
        }
    }
    
    if (in_arg) {
        *dst = '\0';
    }
    
    cmd_buff->argv[arg_count] = NULL;
    cmd_buff->argc = arg_count;
    
    return OK;
}

Built_In_Cmds match_command(const char *input)
{
    if (!input || strlen(input) == 0) {
        return BI_NOT_BI;
    }
    
    if (strcmp(input, EXIT_CMD) == 0) {
        return BI_CMD_EXIT;
    }
    
    if (strcmp(input, "cd") == 0) {
        return BI_CMD_CD;
    }
    
    if (strcmp(input, "dragon") == 0) {
        return BI_CMD_DRAGON;
    }
    
    if (strcmp(input, "rc") == 0) {
        return BI_RC;
    }
    
    return BI_NOT_BI;
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd)
{
    if (!cmd || cmd->argc == 0) {
        return BI_NOT_BI;
    }
    
    Built_In_Cmds cmd_type = match_command(cmd->argv[0]);
    
    switch (cmd_type) {
        case BI_CMD_EXIT:
            return BI_CMD_EXIT;
            
        case BI_CMD_CD:
            if (cmd->argc == 1) {
                // No argument - do nothing as per assignment
                last_return_code = 0;
            } else if (cmd->argc == 2) {
                if (chdir(cmd->argv[1]) == 0) {
                    last_return_code = 0;
                } else {
                    last_return_code = 1;
                }
            } else {
                last_return_code = 1;
            }
            return BI_EXECUTED;
            
        case BI_CMD_DRAGON:
            print_dragon();
            return BI_EXECUTED;
            
        case BI_RC:
            printf("%d\n", last_return_code);
            return BI_EXECUTED;
            
        default:
            return BI_NOT_BI;
    }
}

int exec_cmd(cmd_buff_t *cmd)
{
    if (!cmd || cmd->argc == 0) {
        return ERR_MEMORY;
    }
    
    pid_t pid = fork();
    
    if (pid < 0) {
        // Fork failed
        return ERR_EXEC_CMD;
    } else if (pid == 0) {
        // Child process
        execvp(cmd->argv[0], cmd->argv);
        
        // If execvp returns, it failed
        int exec_errno = errno;
        exit(exec_errno);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
            last_return_code = WEXITSTATUS(status);
            
            // Handle specific error codes for extra credit
            if (last_return_code == ENOENT) {
                printf("Command not found in PATH\n");
            } else if (last_return_code == EACCES) {
                printf("Permission denied\n");
            } else if (last_return_code != 0) {
                printf("Command failed with error code %d\n", last_return_code);
            }
            
            return OK;
        } else {
            return ERR_EXEC_CMD;
        }
    }
}

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
int exec_local_cmd_loop()
{
    char input_buffer[SH_CMD_MAX];
    cmd_buff_t cmd;
    int rc = 0;
    
    // Initialize command buffer
    if (alloc_cmd_buff(&cmd) != OK) {
        return ERR_MEMORY;
    }
    
    while (1) {
        printf("%s", SH_PROMPT);
        
        if (fgets(input_buffer, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        
        // Remove trailing newline
        input_buffer[strcspn(input_buffer, "\n")] = '\0';
        
        // Skip empty input
        if (strlen(input_buffer) == 0) {
            continue;
        }
        
        // Parse command
        rc = build_cmd_buff(input_buffer, &cmd);
        if (rc != OK) {
            printf("Error parsing command\n");
            continue;
        }
        
        // Check if command was parsed
        if (cmd.argc == 0) {
            printf("%s", CMD_WARN_NO_CMD);
            continue;
        }
        
        // Check for built-in commands
        Built_In_Cmds builtin_result = exec_built_in_cmd(&cmd);
        
        if (builtin_result == BI_CMD_EXIT) {
            break;
        } else if (builtin_result == BI_NOT_BI) {
            // Execute external command
            rc = exec_cmd(&cmd);
            if (rc != OK) {
                printf("%s", CMD_ERR_EXECUTE);
            }
        }
        // Built-in commands that were executed successfully don't need further action
    }
    
    free_cmd_buff(&cmd);
    return OK_EXIT;
}
