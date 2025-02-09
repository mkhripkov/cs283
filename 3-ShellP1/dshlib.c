#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

static char* trimWhitespace(char *str)
{
    char *end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0)
        return str;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end+1) = '\0';
    return str;
}

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    char *commandToken;
    int cmdCount = 0;

    commandToken = strtok(cmd_line, PIPE_STRING);
    while (commandToken != NULL) {
        if (cmdCount >= CMD_MAX)
            return ERR_TOO_MANY_COMMANDS;

        char *trimmed = trimWhitespace(commandToken);

        if (strlen(trimmed) == 0) {
            commandToken = strtok(NULL, PIPE_STRING);
            continue;
        }

        char *arg;
        char *saveptr = NULL;
        arg = strtok_r(trimmed, " ", &saveptr);
        if (arg == NULL) {
            commandToken = strtok(NULL, PIPE_STRING);
            continue;
        }

        if (strlen(arg) >= EXE_MAX)
            return ERR_CMD_OR_ARGS_TOO_BIG;
        strcpy(clist->commands[cmdCount].exe, arg);

        char combinedArgs[ARG_MAX] = "";
        int first_arg = 1;
        while ((arg = strtok_r(NULL, " ", &saveptr)) != NULL) {
            if (strlen(arg) >= ARG_MAX)
                return ERR_CMD_OR_ARGS_TOO_BIG;
            if (!first_arg)
                strcat(combinedArgs, " ");
            strcat(combinedArgs, arg);
            first_arg = 0;
        }
        strcpy(clist->commands[cmdCount].args, combinedArgs);
        cmdCount++;

        commandToken = strtok(NULL, PIPE_STRING);
    }
    clist->num = cmdCount;
    if (cmdCount == 0)
        return WARN_NO_CMDS;

    return OK;
}