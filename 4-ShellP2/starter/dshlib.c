#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"
#include "errno.h"

static int last_rc = 0; // global variable to store last rc


// helper functions for memory management

int alloc_cmd_buff(cmd_buff_t *cmd_buff)
{
    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX);
    if (!cmd_buff->_cmd_buffer)
    {
        return ERR_MEMORY;
    }
    memset(cmd_buff->_cmd_buffer, 0, SH_CMD_MAX);
    cmd_buff->argc = 0;
    for (int i = 0; i < CMD_ARGV_MAX; i++)
    {
        cmd_buff->argv[i] = NULL;
    }
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
    {
        cmd_buff->argv[i] = NULL;
    }
    return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff)
{
    cmd_buff->argc = 0;
    for (int i = 0; i < CMD_ARGV_MAX; i++)
    {
        cmd_buff->argv[i] = NULL;
    }
    return OK;
}


int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff)
{
    if (!cmd_buff->_cmd_buffer)
    {
        return ERR_MEMORY;
    }

    strncpy(cmd_buff->_cmd_buffer, cmd_line, SH_CMD_MAX - 1);
    cmd_buff->_cmd_buffer[SH_CMD_MAX - 1] = '\0';

    char *buffer = cmd_buff->_cmd_buffer;

    while (isspace((unsigned char)*buffer))
    {
        buffer++;
    }
    if (*buffer == '\0')
    {
        return WARN_NO_CMDS;
    }

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
                {
                    break;
                }
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
    {
        return WARN_NO_CMDS;
    }
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
    if (strcmp(input, "rc") == 0)
        return BI_RC;

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
                {
                    perror("cd error");
                }
            }
            return BI_EXECUTED;
        case BI_CMD_DRAGON:
            print_dragon();
            return BI_EXECUTED;
        case BI_RC:
            printf("%d\n", last_rc);
            return BI_EXECUTED;
        default:
            return BI_NOT_BI;
    }
}

int exec_cmd(cmd_buff_t *cmd)
{
    pid_t f_result;
    int c_result;

    f_result = fork();
    if (f_result < 0)
    {
        perror("fork error");
        last_rc = -1;
        return ERR_EXEC_CMD;
    }

    if (f_result == 0)
    {
        execvp(cmd->argv[0], cmd->argv);
        int e = errno;
        _exit(e);
    }
    else
    {
        wait(&c_result);
        int child_exit_code = WEXITSTATUS(c_result);
        last_rc = child_exit_code;
        if (child_exit_code != 0)
        {
            switch(child_exit_code)
            {
                case ENOENT:
                    printf("Command not found in PATH\n");
                    break;
                case EACCES:
                    printf("Permission denied to execute command\n");
                    break;
                default:
                    printf("Permission denied to execute command\n");
                    break;
            }
            printf("%s", SH_PROMPT);
        }
    }
    return OK;
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
    int rc = 0;
    cmd_buff_t cmd;
    rc = alloc_cmd_buff(&cmd);
    if (rc != OK)
    {
        fprintf(stderr, "Error: Could not allocate command buffer.\n");
        return rc;
    }

    while (1)
    {
        printf("%s", SH_PROMPT);
        if (fgets(cmd._cmd_buffer, SH_CMD_MAX, stdin) == NULL)
        {
            printf("\n");
            break;
        }
        cmd._cmd_buffer[strcspn(cmd._cmd_buffer, "\n")] = '\0';
        if (strlen(cmd._cmd_buffer) == 0)
        {
            continue;
        }
        clear_cmd_buff(&cmd);
        rc = build_cmd_buff(cmd._cmd_buffer, &cmd);
        if (rc == WARN_NO_CMDS)
        {
            continue;
        }
        else if (rc != OK)
        {
            continue;
        }
        Built_In_Cmds bic = match_command(cmd.argv[0]);
        if (bic == BI_CMD_EXIT)
        {
            break;
        }
        else if (bic != BI_NOT_BI)
        {
            exec_built_in_cmd(&cmd);
        }
        else
        {
            exec_cmd(&cmd);
        }
    }
    free_cmd_buff(&cmd);
    return 0;
}