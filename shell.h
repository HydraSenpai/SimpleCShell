#include <stdio.h>

#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h> 
#include <unistd.h>
#include <stdio.h>
#define DELIMITERS " |<>&;\t\n"
#define length1 512
#define maxLength 512
#define maxArray 50
#define historyLength 20
#define historyName ".hist_list"
 
void tokenize(char *cwd,char *command);

void exitShell(char *savedPath);

void getInputCommand(char *cwd,char *originalPath);

void startCommand(char *cwd,char *command);

void setPath();

void getPath();

bool readInput(char *input,char *savedPath);

void cd(char *cwd);

void runShell();

void saveHistory();

void writeHistory(char *command);

void printHistory();

void getHistory(char *cwd,char *command);

void writeFile();

void readFile();

char *removeChars(char *string1);

void addAlias();

void removeAlias();

void writeAliasToFile();

void readAliasFromFile(char *cwd);


