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
        return BI_EXIT;
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

void apply_redirection(redirection_t *redir) {
    if (redir->input_file) {
        int fd = open(redir->input_file, O_RDONLY);
        dup2(fd, STDIN_FILENO);
        close(fd);
    }
    if (redir->output_file) {
        int fd = open(redir->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
    if (redir->append_file) {
        int fd = open(redir->append_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
}

void parse_redirection(cmd_buff_t *cmd, redirection_t *redir) {
    for (int i = 0; i < cmd->argc; i++) {
        if (strcmp(cmd->argv[i], "<") == 0) {
            redir->input_file = cmd->argv[i + 1];
            cmd->argv[i] = NULL;
            cmd->argc = i;
            break;
        } else if (strcmp(cmd->argv[i], ">") == 0) {
            redir->output_file = cmd->argv[i + 1];
            cmd->argv[i] = NULL;
            cmd->argc = i;
            break;
        } else if (strcmp(cmd->argv[i], ">>") == 0) {
            redir->append_file = cmd->argv[i + 1];
            cmd->argv[i] = NULL;
            cmd->argc = i;
            break;
        }
    }
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

int exec_pipeline(char **commands, int num_cmds) {
    int pipes[2 * (num_cmds - 1)];
    pid_t pids[num_cmds];

    for (int i = 0; i < num_cmds - 1; i++) {
        pipe(pipes + i * 2);
    }

    for (int i = 0; i < num_cmds; i++) {
        cmd_buff_t cmd;
        redirection_t redir = {0};
        build_cmd_buff(commands[i], &cmd);
        parse_redirection(&cmd, &redir);

        pid_t pid = fork();
        if (pid == 0) {
            if (i != 0) dup2(pipes[(i - 1) * 2], STDIN_FILENO);
            if (i != num_cmds - 1) dup2(pipes[i * 2 + 1], STDOUT_FILENO);

            for (int j = 0; j < 2 * (num_cmds - 1); j++) close(pipes[j]);

            if ((i == 0 && redir.input_file) || (i == num_cmds - 1 && (redir.output_file || redir.append_file))) {
                apply_redirection(&redir);
            }

            execvp(cmd.argv[0], cmd.argv);
            perror("exec failed");
            exit(EXIT_FAILURE);
        }
        pids[i] = pid;
    }

    for (int i = 0; i < 2 * (num_cmds - 1); i++) close(pipes[i]);

    int status;
    for (int i = 0; i < num_cmds; i++) {
        waitpid(pids[i], &status, 0);
        last_return_code = WEXITSTATUS(status);
    }
    return last_return_code;
}

int exec_local_cmd_loop() {
    char input[SH_CMD_MAX];
    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(input, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        input[strcspn(input, "\n")] = '\0';

        char *commands[CMD_MAX];
        int num_cmds = 0;
        char *token = strtok(input, PIPE_STRING);
        while (token && num_cmds < CMD_MAX) {
            commands[num_cmds++] = token;
            token = strtok(NULL, PIPE_STRING);
        }

        if (num_cmds == 0) {
            printf(CMD_WARN_NO_CMD);
            continue;
        }
	if (num_cmds == 1) {
            cmd_buff_t cmd;
            build_cmd_buff(commands[0], &cmd);
            Built_In_Cmds bi_status = exec_built_in_cmd(&cmd);
            if (bi_status == BI_EXECUTED) continue;
            if (bi_status == BI_EXIT) {
                printf("exiting...\n");
                break;
            }
	    last_return_code = exec_cmd(&cmd);
        } else {
            exec_pipeline(commands, num_cmds);
        }
    }
    return OK;
}

