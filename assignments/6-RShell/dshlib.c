#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

/**** 
 **** FOR REMOTE SHELL USE YOUR SOLUTION FROM SHELL PART 3 HERE
 **** THE MAIN FUNCTION CALLS THIS ONE AS ITS ENTRY POINT TO
 **** EXECUTE THE SHELL LOCALLY
 ****
 */

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user inpu
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
    char cmd_buff[SH_CMD_MAX];
    command_list_t cmd_list;
    
    while(1) {
        printf("%s", SH_PROMPT);
        
        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';
        
        if (strlen(cmd_buff) == 0) {
            continue;
        }
        
        int result = build_cmd_list(cmd_buff, &cmd_list);
        
        if (result == WARN_NO_CMDS) {
            printf("%s", CMD_WARN_NO_CMD);
            continue;
        }
        
        if (result == ERR_TOO_MANY_COMMANDS) {
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
            free_cmd_list(&cmd_list);
            continue;
        }
        
        if (result != OK) {
            printf("error parsing command\n");
            free_cmd_list(&cmd_list);
            continue;
        }
        
        printf(CMD_OK_HEADER, cmd_list.num);
        
        result = execute_pipeline(&cmd_list);
        
        if (result == OK_EXIT) {
            free_cmd_list(&cmd_list);
            return OK;
        }
        
        free_cmd_list(&cmd_list);
    }
    
    return OK;
}

int alloc_cmd_buff(cmd_buff_t *cmd_buff)
{
    if (cmd_buff == NULL) {
        return ERR_MEMORY;
    }
    
    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX);
    if (cmd_buff->_cmd_buffer == NULL) {
        return ERR_MEMORY;
    }
    
    cmd_buff->argc = 0;
    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        cmd_buff->argv[i] = NULL;
    }
    
    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff)
{
    if (cmd_buff == NULL) {
        return OK;
    }
    
    if (cmd_buff->_cmd_buffer != NULL) {
        free(cmd_buff->_cmd_buffer);
        cmd_buff->_cmd_buffer = NULL;
    }
    
    return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff)
{
    if (cmd_buff == NULL) {
        return ERR_MEMORY;
    }
    
    for (int i = 0; i < cmd_buff->argc; i++) {
        if (cmd_buff->argv[i] != NULL) {
            free(cmd_buff->argv[i]);
            cmd_buff->argv[i] = NULL;
        }
    }
    
    cmd_buff->argc = 0;
    
    if (cmd_buff->_cmd_buffer != NULL) {
        cmd_buff->_cmd_buffer[0] = '\0';
    }
    
    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff)
{
    if (cmd_line == NULL || cmd_buff == NULL) {
        return ERR_MEMORY;
    }
    
    clear_cmd_buff(cmd_buff);
    
    char *cmd_copy = strdup(cmd_line);
    if (cmd_copy == NULL) {
        return ERR_MEMORY;
    }
    
    char *token = strtok(cmd_copy, " ");
    while (token != NULL && cmd_buff->argc < CMD_ARGV_MAX - 1) {
        cmd_buff->argv[cmd_buff->argc] = strdup(token);
        if (cmd_buff->argv[cmd_buff->argc] == NULL) {
            free(cmd_copy);
            return ERR_MEMORY;
        }
        cmd_buff->argc++;
        token = strtok(NULL, " ");
    }
    
    cmd_buff->argv[cmd_buff->argc] = NULL;
    
    free(cmd_copy);
    return OK;
}

int close_cmd_buff(cmd_buff_t *cmd_buff)
{
    return free_cmd_buff(cmd_buff);
}

int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    if (cmd_line == NULL || clist == NULL) {
        return ERR_MEMORY;
    }
    
    clist->num = 0;
    
    char *cmd_copy = strdup(cmd_line);
    if (cmd_copy == NULL) {
        return ERR_MEMORY;
    }
    
    char *start = cmd_copy;
    char *end;
    
    while (*start != '\0') {
        while (*start == ' ') start++;
        
        if (*start == '\0') break;
        
        end = start;
        while (*end != '\0' && *end != '|') end++;
        
        if (*end == '|') {
            *end = '\0';
            end++;
        }
        
        char *cmd_start = start;
        char *cmd_end = cmd_start + strlen(cmd_start) - 1;
        while (cmd_end > cmd_start && *cmd_end == ' ') {
            *cmd_end = '\0';
            cmd_end--;
        }
        
        if (strlen(cmd_start) > 0) {
            if (clist->num >= CMD_MAX) {
                free(cmd_copy);
                return ERR_TOO_MANY_COMMANDS;
            }
            
            if (alloc_cmd_buff(&clist->commands[clist->num]) != OK) {
                free(cmd_copy);
                return ERR_MEMORY;
            }
            
            strncpy(clist->commands[clist->num]._cmd_buffer, cmd_start, SH_CMD_MAX - 1);
            clist->commands[clist->num]._cmd_buffer[SH_CMD_MAX - 1] = '\0';
            
            if (build_cmd_buff(clist->commands[clist->num]._cmd_buffer, &clist->commands[clist->num]) != OK) {
                free(cmd_copy);
                return ERR_MEMORY;
            }
            
            clist->num++;
        }
        
        start = end;
    }
    
    free(cmd_copy);
    
    if (clist->num == 0) {
        return WARN_NO_CMDS;
    }
    
    return OK;
}

int free_cmd_list(command_list_t *cmd_lst)
{
    if (cmd_lst == NULL) {
        return OK;
    }
    
    for (int i = 0; i < cmd_lst->num; i++) {
        free_cmd_buff(&cmd_lst->commands[i]);
    }
    
    cmd_lst->num = 0;
    return OK;
}

Built_In_Cmds match_command(const char *input)
{
    if (input == NULL) {
        return BI_NOT_BI;
    }
    
    if (strcmp(input, EXIT_CMD) == 0) {
        return BI_CMD_EXIT;
    }
    
    if (strcmp(input, "dragon") == 0) {
        return BI_CMD_DRAGON;
    }
    
    if (strcmp(input, "cd") == 0) {
        return BI_CMD_CD;
    }
    
    return BI_NOT_BI;
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd)
{
    if (cmd == NULL || cmd->argc == 0 || cmd->argv[0] == NULL) {
        return BI_NOT_BI;
    }
    
    Built_In_Cmds cmd_type = match_command(cmd->argv[0]);
    
    switch (cmd_type) {
        case BI_CMD_EXIT:
            printf("exiting...\n");
            return BI_CMD_EXIT;
            
        case BI_CMD_DRAGON:
            printf("       __.-------.._      \n");
            printf("     /'  _       _  `\\    \n");
            printf("    /    o`----'o    \\     \n");
            printf("   |                  |    \n");
            printf("   |    ,,,,,.        |    \n");
            printf("   '._.'(    `._.'    |    \n");
            printf("      /  `--'  \\       \n");
            printf("     |          |       \n");
            printf("     |          |       \n");
            printf("    /            \\      \n");
            printf("   /              \\     \n");
            printf("  |                |    \n");
            printf("  '----------------'     \n");
            return BI_EXECUTED;
            
        case BI_CMD_CD:
            if (cmd->argc > 1) {
                if (chdir(cmd->argv[1]) != 0) {
                    perror("cd");
                }
            } else {
                char *home = getenv("HOME");
                if (home != NULL) {
                    if (chdir(home) != 0) {
                        perror("cd");
                    }
                }
            }
            return BI_EXECUTED;
            
        default:
            return BI_NOT_BI;
    }
}

int exec_cmd(cmd_buff_t *cmd)
{
    if (cmd == NULL || cmd->argc == 0 || cmd->argv[0] == NULL) {
        return ERR_EXEC_CMD;
    }
    
    Built_In_Cmds result = exec_built_in_cmd(cmd);
    if (result != BI_NOT_BI) {
        if (result == BI_CMD_EXIT) {
            return OK_EXIT;
        }
        return OK;
    }
    
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return ERR_EXEC_CMD;
    }
    
    if (pid == 0) {
        if (execvp(cmd->argv[0], cmd->argv) == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    } else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return ERR_EXEC_CMD;
        }
        
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            return ERR_EXEC_CMD;
        }
    }
    
    return OK;
}

int execute_pipeline(command_list_t *clist)
{
    if (clist == NULL || clist->num == 0) {
        return WARN_NO_CMDS;
    }
    
    if (clist->num == 1) {
        return exec_cmd(&clist->commands[0]);
    }
    
    int pipes[clist->num - 1][2];
    pid_t pids[clist->num];
    
    for (int i = 0; i < clist->num - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            for (int j = 0; j < i; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            return ERR_EXEC_CMD;
        }
    }
    
    for (int i = 0; i < clist->num; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork");
            for (int j = 0; j < clist->num - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            return ERR_EXEC_CMD;
        }
        
        if (pids[i] == 0) {
            if (i > 0) {
                dup2(pipes[i-1][0], STDIN_FILENO);
                close(pipes[i-1][0]);
                close(pipes[i-1][1]);
            }
            
            if (i < clist->num - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
                close(pipes[i][0]);
                close(pipes[i][1]);
            }
            
            for (int j = 0; j < clist->num - 1; j++) {
                if (j != i - 1) {
                    close(pipes[j][0]);
                }
                if (j != i) {
                    close(pipes[j][1]);
                }
            }
            
            if (execvp(clist->commands[i].argv[0], clist->commands[i].argv) == -1) {
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        }
    }
    
    for (int i = 0; i < clist->num - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    
    for (int i = 0; i < clist->num; i++) {
        int status;
        if (waitpid(pids[i], &status, 0) == -1) {
            perror("waitpid");
            return ERR_EXEC_CMD;
        }
    }
    
    return OK;
}
