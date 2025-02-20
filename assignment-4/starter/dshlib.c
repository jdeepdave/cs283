#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"
#include "dragon.h"
#include <errno.h>

int last_return_code = 0;

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

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    if (cmd->argc == 0) {
        return BI_NOT_BI;
    }

    if (strcmp(cmd->argv[0], "exit") == 0) {
        exit(0);
    }

    if (strcmp(cmd->argv[0], "cd") == 0) {
        if (cmd->argc < 2) {
            return BI_EXECUTED;
        }
        if (chdir(cmd->argv[1]) != 0) {
            perror("cd failed");
            return BI_RC;
        }
        return BI_EXECUTED;
    }

    if (strcmp(cmd->argv[0], "dragon") == 0) {
        print_dragon(); 
        return BI_EXECUTED;
    }

    if (strcmp(cmd->argv[0], "rc") == 0) {
        printf("%d\n", last_return_code);
        return BI_EXECUTED;
    }

    return BI_NOT_BI;
}

int exec_local_cmd_loop() {
    char input[SH_CMD_MAX];
    cmd_buff_t cmd;

    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(input, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        input[strcspn(input, "\n")] = '\0';

        if (build_cmd_buff(input, &cmd) == WARN_NO_CMDS) {
            printf(CMD_WARN_NO_CMD);
            continue;
        }

        if (exec_built_in_cmd(&cmd) == BI_EXECUTED) {
            continue;
        }

     
        exec_cmd(&cmd);
    }
    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    memset(cmd_buff, 0, sizeof(cmd_buff_t));

    char *token;
    int count = 0;
    bool in_quotes = false;
    char *ptr = cmd_line;

    while (*ptr) {
        while (*ptr == SPACE_CHAR && !in_quotes) ptr++;

        if (*ptr == '\"') { 
            in_quotes = !in_quotes; 
            ptr++;
        }

        if (count >= CMD_ARGV_MAX - 1) {
            return ERR_CMD_ARGS_BAD;
        }

        token = ptr;
        while (*ptr && (in_quotes || *ptr != SPACE_CHAR)) {
            if (*ptr == '\"') {
                in_quotes = !in_quotes;
                break;
            }
            ptr++;
        }

        if (*ptr) {
            *ptr = '\0';
            ptr++;
        }

        cmd_buff->argv[count++] = token;
    }

    cmd_buff->argc = count;
    cmd_buff->argv[count] = NULL;

    return (count > 0) ? OK : WARN_NO_CMDS;
}

int exec_cmd(cmd_buff_t *cmd) {
    pid_t pid = fork();
    int status;

    if (pid < 0) {
        perror("fork failed");
        last_return_code = ERR_EXEC_CMD;
        return ERR_EXEC_CMD;
    }

    if (pid == 0) {
        execvp(cmd->argv[0], cmd->argv);

        switch (errno) {
            case ENOENT:
                fprintf(stderr, "Error: Command not found in PATH\n");
                exit(2);
            case EACCES:
                fprintf(stderr, "Error: Permission denied\n");
                exit(126);
            default:
                perror("exec failed");
                exit(errno);
        }
    } else {
        waitpid(pid, &status, 0);
        last_return_code = WEXITSTATUS(status);
        return last_return_code;
    }
}

