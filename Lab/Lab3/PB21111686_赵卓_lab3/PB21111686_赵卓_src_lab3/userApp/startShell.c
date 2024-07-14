#include "vga.h"
#include "uart.h"
#include "string.h"
#include "myPrintk.h"

typedef struct myCommand
{
    char name[80];
    char help_content[200];
    int (*func)(int argc, char *argv[]);
} myCommand;

int func_help(int argc, char *argv[]);
int func_cmd(int argc, char *argv[]);

myCommand cmds[] = {
    {"cmd", "cmd : List all command\n\0", func_cmd},
    {"help", "Usage: help [command]\n\0Display info about[command]\n\0", func_help},
    {"NULL", "NULL", 0}};


int devide_command(const char *str, char **str_devided)
{
    int devidednum = 0;
    int i = 0;

    while (*str++ == ' ')
        ;
    str--;

    while (*str)
    {
        if (*str == ' ')
        {
            while (*str++ == ' ')
                ;
            if (*--str)
            {
                str_devided[devidednum++][i] = '\0';
                i = 0;
            }
        }
        else
            str_devided[devidednum][i++] = *str++;
    }
    str_devided[devidednum++][i] = '\0';

    return devidednum;
}

int execute_command(int argc, char *argv[])
{

    if (!myStrlen(argv[0])) 
        return 1;

    for (int i = 0; myStrcmp(cmds[i].name, "NULL") != 0; i++)
    {
        if (!myStrcmp(argv[0], cmds[i].name))
        {
            cmds[i].func(argc, argv);
            return 0;
        }
    }

    myPrintk(0x7, "The cmd is not defined\n");
    return 1;
}

int func_help(int argc, char *argv[])
{

    if (argc == 1)
    {
        myPrintk(0x2, "Usage: help [command]\nDisplay info about[command]\n");
        return 1;
    }

    for (int i = 0; myStrcmp(cmds[i].name, "NULL") != 0; i++)
    {
        if (!myStrcmp(argv[1], cmds[i].name))
        {
            myPrintk(0x2, "%s", cmds[i].help_content);
            return 1;
        }
    }

    myPrintk(0x7, "The cmd is not defined\n");
    return 0;
}

int func_cmd(int argc, char *argv[])
{

    for (int i = 0; myStrcmp(cmds[i].name, "NULL") != 0; i++)
        myPrintk(0x2, "%s\n", cmds[i].name);

    return 0;
}

void get_input_cmd(char *buf)
{

    char *str = buf;
    unsigned char input;

    while ((input = uart_get_char()) != '\r')
    {
    handle_input:
        if (input == '\r')
            break;

        else
        {
            *str++ = input;
            myPrintk(0x7, "%c", input);
            uart_put_char(input);
        }
    }
    *str = '\0';

    myPrintk(0x7, " -pseudo_terminal\0");
    myPrintk(0x7, "\n");
    uart_put_chars("\r\n");
}

void startShell(void)
{

    char BUF[256];
    char agrv[8][8];
    char *cmd[8];
    int argc;
    for (int i = 0; i < 8; i++)
        cmd[i] = agrv[i];

    while (1)
    {

        myPrintk(0x7, "Student>>");

        get_input_cmd(BUF);

        argc = devide_command(BUF, cmd);

        execute_command(argc, cmd);
    }

    return;
}
