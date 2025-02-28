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

extern void print_dragon();

int alloc_cmd_buff(cmd_buff_t *cmd_buff)
{
    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX);
    if (!cmd_buff->_cmd_buffer)
        return ERR_MEMORY;
    memset(cmd_buff->_cmd_buffer, 0, SH_CMD_MAX);
    cmd_buff->argc = 0;
    for (int i = 0; i < CMD_ARGV_MAX; i++)
        cmd_buff->argv[i] = NULL;
    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff)
{
    if (cmd_buff->_cmd_buffer)
    {
        free(cmd_buff->_cmd_buffer);
        cmd_buff->_cmd_buffer = NULL;
    }
    cmd_buff->argc = 0;
    for (int i = 0; i < CMD_ARGV_MAX; i++)
        cmd_buff->argv[i] = NULL;
    return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff)
{
    cmd_buff->argc = 0;
    for (int i = 0; i < CMD_ARGV_MAX; i++)
        cmd_buff->argv[i] = NULL;
    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff)
{
    if (!cmd_buff->_cmd_buffer)
        return ERR_MEMORY;

    strncpy(cmd_buff->_cmd_buffer, cmd_line, SH_CMD_MAX - 1);
    cmd_buff->_cmd_buffer[SH_CMD_MAX - 1] = '\0';

    char *buffer = cmd_buff->_cmd_buffer;
    while (isspace((unsigned char)*buffer))
        buffer++;
    if (*buffer == '\0')
        return WARN_NO_CMDS;

    bool inQuotes = false;
    bool inToken = false;
    int argc = 0;
    char *dst = buffer;

    while (*buffer != '\0')
    {
        if (*buffer == '"')
        {
            inQuotes = !inQuotes;
            buffer++;
        }
        else if (!inQuotes && isspace((unsigned char)*buffer))
        {
            if (inToken)
            {
                *dst++ = '\0';
                inToken = false;
                argc++;
                if (argc >= CMD_ARGV_MAX - 1)
                    break;
            }
            buffer++;
        }
        else
        {
            if (!inToken)
            {
                cmd_buff->argv[argc] = dst;
                inToken = true;
            }
            *dst++ = *buffer++;
        }
    }
    if (inToken)
    {
        *dst++ = '\0';
        argc++;
    }
    cmd_buff->argv[argc] = NULL;
    cmd_buff->argc = argc;
    if (argc == 0)
        return WARN_NO_CMDS;
    return OK;
}

int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    clist->num = 0;
    char *token = strtok(cmd_line, PIPE_STRING);
    while (token != NULL)
    {
        while (isspace((unsigned char)*token))
            token++;
        char *end = token + strlen(token) - 1;
        while (end > token && isspace((unsigned char)*end))
        {
            *end = '\0';
            end--;
        }
        if (strlen(token) == 0)
        {
            token = strtok(NULL, PIPE_STRING);
            continue;
        }
        if (clist->num >= CMD_MAX)
            return ERR_TOO_MANY_COMMANDS;
        int rc = alloc_cmd_buff(&clist->commands[clist->num]);
        if (rc != OK)
            return rc;
        rc = build_cmd_buff(token, &clist->commands[clist->num]);
        if (rc != OK)
            return rc;
        clist->num++;
        token = strtok(NULL, PIPE_STRING);
    }
    if (clist->num == 0)
        return WARN_NO_CMDS;
    return OK;
}

int free_cmd_list(command_list_t *clist)
{
    for (int i = 0; i < clist->num; i++)
        free_cmd_buff(&clist->commands[i]);
    clist->num = 0;
    return OK;
}

Built_In_Cmds match_command(const char *input)
{
    if (!input)
        return BI_NOT_BI;
    if (strcmp(input, EXIT_CMD) == 0)
        return BI_CMD_EXIT;
    if (strcmp(input, "cd") == 0)
        return BI_CMD_CD;
    if (strcmp(input, "dragon") == 0)
        return BI_CMD_DRAGON;
    return BI_NOT_BI;
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd)
{
    Built_In_Cmds which = match_command(cmd->argv[0]);
    switch (which)
    {
        case BI_CMD_EXIT:
            return BI_CMD_EXIT;
        case BI_CMD_CD:
            if (cmd->argc > 1)
            {
                if (chdir(cmd->argv[1]) != 0)
                    perror("cd error");
            }
            return BI_EXECUTED;
        case BI_CMD_DRAGON:
            print_dragon();
            return BI_EXECUTED;
        default:
            return BI_NOT_BI;
    }
}

int exec_cmd(cmd_buff_t *cmd)
{
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork error");
        return ERR_EXEC_CMD;
    }
    if (pid == 0)
    {
        execvp(cmd->argv[0], cmd->argv);
        perror("execvp");
        _exit(127);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
    }
    return OK;
}

int execute_pipeline(command_list_t *clist)
{
    int n = clist->num;
    if (n == 0)
        return WARN_NO_CMDS;

    int num_pipes = n - 1;
    int pipefds[2 * num_pipes];
    for (int i = 0; i < num_pipes; i++)
    {
        if (pipe(pipefds + i * 2) < 0)
        {
            perror("pipe");
            return ERR_MEMORY;
        }
    }

    pid_t pids[CMD_MAX];
    for (int i = 0; i < n; i++)
    {
        pids[i] = fork();
        if (pids[i] < 0)
        {
            perror("fork");
            return ERR_EXEC_CMD;
        }
        if (pids[i] == 0)
        {
            
            if (i > 0)
            {
                if (dup2(pipefds[(i - 1) * 2], STDIN_FILENO) < 0)
                {
                    perror("dup2 stdin");
                    _exit(127);
                }
            }
            
            if (i < n - 1)
            {
                if (dup2(pipefds[i * 2 + 1], STDOUT_FILENO) < 0)
                {
                    perror("dup2 stdout");
                    _exit(127);
                }
            }
            
            for (int j = 0; j < 2 * num_pipes; j++)
                close(pipefds[j]);

            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            perror("execvp");
            _exit(127);
        }
    }

    for (int i = 0; i < 2 * num_pipes; i++)
        close(pipefds[i]);

    for (int i = 0; i < n; i++)
        waitpid(pids[i], NULL, 0);

    return OK;
}

int exec_local_cmd_loop()
{
    int rc = 0;
    char input_line[SH_CMD_MAX];

    cmd_buff_t single_cmd;
    rc = alloc_cmd_buff(&single_cmd);
    if (rc != OK)
    {
        fprintf(stderr, "Error: Could not allocate command buffer.\n");
        return rc;
    }

    while (1)
    {
        printf("%s", SH_PROMPT);
        if (fgets(input_line, SH_CMD_MAX, stdin) == NULL)
        {
            printf("\n");
            break;
        }
        input_line[strcspn(input_line, "\n")] = '\0';
        if (strlen(input_line) == 0)
            continue;

        if (strchr(input_line, PIPE_CHAR) != NULL)
        {
            command_list_t clist;
            clist.num = 0;
            rc = build_cmd_list(input_line, &clist);
            if (rc == WARN_NO_CMDS)
                continue;
            else if (rc != OK)
                continue;
            execute_pipeline(&clist);
            free_cmd_list(&clist);
        }
        else
        {
            clear_cmd_buff(&single_cmd);
            rc = build_cmd_buff(input_line, &single_cmd);
            if (rc == WARN_NO_CMDS)
                continue;
            else if (rc != OK)
                continue;
            Built_In_Cmds bic = match_command(single_cmd.argv[0]);
            if (bic == BI_CMD_EXIT)
                break;
            else if (bic != BI_NOT_BI)
            {
                exec_built_in_cmd(&single_cmd);
            }
            else
            {
                exec_cmd(&single_cmd);
            }
        }
    }
    free_cmd_buff(&single_cmd);
    return 0;
}
