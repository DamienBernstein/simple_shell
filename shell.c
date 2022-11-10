#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>

unsigned int count_args(char *str, char *delimeter)
{
unsigned int count;
char *buffer;
char *token;

buffer = malloc(sizeof(str));
strcpy(buffer, str);

count = 0;

token = strtok(buffer, delimeter);

while (token != NULL)
{
++count;
token = strtok(NULL, " ");
}
free(buffer);
return (count + 1);
}

void break_string(char *str, char *delimeter, char **ptr)
{
char *token, *str2;
int i, j;

i = 0;
j = 0;

str2 = malloc(sizeof(char) * (strlen(str) - 1));
while (str[j] != '\n' && str[j] != '\0')
{
str2[j] = str[j];
++j;
}


token = strtok(str2, delimeter);


while (token != NULL)
{
ptr[i] = malloc(sizeof(char) * strlen(token));
strcpy(ptr[i], token);
token = strtok(NULL, delimeter);
++i;
}

free(str2);

}


int searchforfunction(char *str, char *directory)
{
struct dirent *de;
DIR *dr = opendir(directory);

if (dr == NULL)
{
printf("Could not open current directory" );
return (0);
}

while ((de = readdir(dr)) != NULL)
{
if (strcmp(de->d_name, str) == 0)
{
closedir(dr);
return (1);
}
}

closedir(dr);
return (0);


}


int main(int ac, char **av, char **env)
{
char *buffer, **str, *path, *oldpath, **str2;
size_t size, characters;
int i, j, status, exists, pathtrue, slashcount;
unsigned int count;
pid_t child_pid;

slashcount = 0;
pathtrue = 0;
i = 0;
j = 0;
size = 32;

oldpath = "/bin/";


buffer = malloc(sizeof(char) * size);

do
{  
printf("$ ");
characters = getline(&buffer, &size, stdin);
if (characters == -1)
{
printf("\n");
return (0);
}
}
while (characters == 1);

count = count_args(buffer, " ");

str = malloc(sizeof(char *) * count);	
break_string(buffer, " ", str);
if (strcmp(str[0], "exit") == 0)
{
exit(0);
return (0);
}

while (str[0][j] != '\0')
{
if (str[0][j] == '/')
{
++slashcount;
pathtrue = 1;
}
++j;
}

if (pathtrue == 1)
{

str2  = malloc(sizeof(char *) * (slashcount + 2));
break_string(str[0], "/", str2);
exists = searchforfunction(str2[slashcount - 1], "/bin/");
if (exists == 1)
{
child_pid = fork();

if (child_pid == 0)
{
execve(str[0], str, env);
}

if (child_pid != 0)
{
wait(&status);
fflush(stdin);
free(buffer);
free(str);
free(str2);
main(ac, av, env);

}
}
else 
{
printf("%s: not found\n", str[0]);
child_pid = fork();		

if (child_pid == 0)
{
execve(str[0], str, env);
}

if (child_pid != 0)
{
wait(&status);
fflush(stdin);
free(buffer);
free(str);
main(ac, av, env);

}
}
}
else
{
exists = searchforfunction(str[0], "/bin/");

if (exists == 1)
{	
path = calloc(strlen(oldpath) + strlen(str[0]) + 1, (sizeof(char)));

strcpy(path, oldpath);
strcat(path, str[0]);

str[0] = malloc(sizeof(path));
strcpy(str[0], path);

child_pid = fork();

if (child_pid == 0)
{
execve(path, str, env);
}
if (child_pid != 0)
{
wait(&status);
fflush(stdin);
free(buffer);
free(str);
main(ac, av, env);
}


}
else
{
printf("%s: command not found\n", str[0]);
free(buffer);
free(str);
main(ac, av, env);

}
return (0);
}
}
