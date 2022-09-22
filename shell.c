#include <stdio.h>
#include <string.h>
#include "shell.h"
#include <stdint.h> 
#include <stdbool.h>
#include <stdlib.h> 
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

bool fromHistory = false;
char *splitArray[maxArray];
static char historyArray[historyLength][maxLength];
static int historyCounter = 0;
char *aliases[10][2];
int numTokens = 0;

void runShell(){
	
	//stage 3 path variables
	char *originalPath;
	char *cwd;
	
	originalPath = getenv("PATH");
	cwd = getenv("HOME");
	chdir(cwd);
	

	//stage 6 variables
	
	
	
	readFile();
	
	
	
	
	 if(getenv("HOME") != NULL) {
        chdir(getenv("HOME"));
	 }
	 
	while(1){
		
		
		//just incase i need to clear splitArray for some reason lol
		for(int x=0;x<maxArray;x++){
			splitArray[x] = NULL;
		}
		getInputCommand(cwd,originalPath);
		numTokens = 0;
	}
}

void writeFile() {
    FILE *file = fopen(historyName, "w");

    file = fopen(historyName, "w");
	if (file == NULL) {
		fprintf(stderr, "Can't open %s\n", historyName);
		exit(1);
	}

	
	for (int n = 0; n < 20; n++) {
        fputs(historyArray[n], file);
	}
	fclose(file);

}


void readFile() {
    FILE *file;
    file = fopen(historyName, "r");

    if (file == NULL) {
		perror("History: ");
		return;
    }

    char line[maxLength];
    while(fgets(line, sizeof(line), file)!=NULL) {
		
		writeHistory(line);
    }


    fclose(file);

}


void exitShell(char *originalPath){
    printf("\nExiting Program\n");
	printf("The restored path is %s\n",originalPath);
	setenv("PATH",originalPath,1);
	printf("Saved to file\n");
	writeFile();
	//sets path back to original path
    exit(EXIT_SUCCESS);
}

void getInputCommand(char *cwd,char *originalPath){
	//command input variables
	char *command = malloc(maxLength); 
	bool overLimit = false;

	
	
	//start of input method
		
		printf(">");
		
		//reads input and finds if its over the limit, stores in overLimit for checking later
		overLimit = readInput(command,originalPath);
			
		//compares input command to string "exit" and exits shell
		if(!strcmp(command,"exit\n")){		
			exitShell(originalPath);
		}
		
		//if command is empty should not continue process and start again
		if((command[0] == '\n' && command[1] == '\0')){
		//input over the 512 length is not accepted and will start the process again with an error message
		} else if(overLimit == true){
			printf("input was too long\n");
			//will clear buffer by reading all input to end of file
			char c;
			while ((c = getchar()) != '\n' && c != EOF) { }
		} else {
			tokenize(cwd,command);
			}
			
			
		
	free(command);
}

void tokenize(char *cwd,char *command){
	//stage 2 split input variables
	const char splitChars[] = "; |\n\t&<>";
	char *token;
	int count = 0;
			if((strncmp("!",command,1)==0)){
				fromHistory=true;
			}
			if(fromHistory == false){
				writeHistory(command);
			}
			fromHistory = false;
			
			//gets first token
			token = strtok(command, splitChars);
 
			//check for aliases
			int i = 0;
			while(aliases[i][0] != NULL){
				if(strcmp(token, aliases[i][0]) == 0){
					printf("alias found\n");
					tokenize(cwd, aliases[i][1]);
					return;
				}
				i++;
			}
			

			//goes through rest of tokens
			while(token != NULL){
				//creates space for each element in the input
				splitArray[count] = malloc(maxLength);
			
				//passes each token to the splitArray
				stpcpy(splitArray[count],token);
			
				//printf("array: %s\n",splitArray[count]);
				//printf("token: %s\n",token);
			
				count++;
				numTokens++;
				token = strtok(NULL, splitChars);
				//frees all input from splitArray
				free(splitArray[count]);
			}
		
			splitArray[count] = NULL;
			startCommand(cwd,command);
		
}
void startCommand(char *cwd,char *command){
	
	//covers input that only contained a string with the tokenizer chars
	if(splitArray[0] == NULL){	
	}
	//compares first input of splitArray to setpath and if 0 goes to setpath
	else if(strcmp("setpath",splitArray[0]) == 0){
		setPath();
	//compares first input of splitArray to setpath and if 0 goes to getPath
	} else if (strcmp("getpath",splitArray[0]) == 0){
		getPath();
	//compares first input of splitArray to setpath and if 0 goes to cd
	} else if (strcmp("cd",splitArray[0]) == 0){
		cd(cwd);
	//compares first input of splitArray to history and if 0 goes to printHistory
	} else if(strcmp("history",splitArray[0]) == 0){
		printHistory();
	//compares first input of splitArray to ! and if 0 goes to getHistory	
	} else if((strncmp("!",splitArray[0],1) == 0)){
		getHistory(cwd,command);
	}else if(strcmp("alias",splitArray[0]) == 0){
		addAlias();
	} else if(strcmp("unalias",splitArray[0]) == 0){
		removeAlias();
	}
	else{
	
	int status;
	
	pid_t pid = fork();
	
	//start a child process
	
	//an error occurs if pid < 0
	if(pid < 0){
		perror("An error has occured could not fork properly\n");
		exit(0);
	
	//child process
	} else if (pid == 0){
		if(execvp(splitArray[0],splitArray) == -1){
			perror("Invalid command:");
		}
		exit(0); //kill the fork()

		
	
	//parent process
	} else {
		while(wait(&status) != pid);
	}
	}
}

void getPath(){
	//compares second element (parameters after command) to see if it is null as getpath takes no parameters
	if(splitArray[1] != NULL){
		printf("Invalid command, getpath does not accept parameters\n");
	}
	printf("The current path is %s\n",getenv("PATH"));
}

void setPath(){
	//if splitArray has a value at index 2 throws error as setpath only needs one parameter for the chosen path
	if (splitArray[2] != NULL){
		printf("Error, setpath only needs one parameter\n");
	//needs one parameter so if index 1 is empty there is no new path to be set
	} else if (splitArray[1] == NULL) {
		printf("Error, setpath needs one parameter\n");
		
	} else {
		//sets new path, successful if 0 is returned else will throw an error and not set
		if (setenv("PATH", splitArray[1], 1) == 0) {
			printf("Path set to %s\n", splitArray[1]);
			
		} else {
			printf("Could not set path");
			perror(NULL);
		}
	}
}

void cd(char *cwd){
	//if no parameters are passed it will save the cwd back to the home directory
	if(splitArray[1] == NULL){
		chdir(getenv("HOME"));
		printf("The cwd is %s\n", getenv("HOME"));
	//checks if there are too may parameters, only takes 0 or 1 parameters
	} else if(splitArray[2] != NULL){
		printf("cd only takes in 1 parameter\n");
	//will change directory
	} else{
		if(chdir(splitArray[1]) < 0){
			fprintf(stderr,"%s: %s: ",splitArray[0],splitArray[1]);
			perror("");
		}
	}
}

void printHistory(){
	
	
	//input validation for having a parameter since printing history doesnt need one
	if (splitArray[1] != NULL) {
		printf("Error: print history does not take any parameters\n");

	//just prints history (historyArray taking one index is weird)
	} else {
		for (int x = 0; x < (historyCounter); x++){
			printf("%d: %s", (x + 1), historyArray[x]);
			//puts("%d: %s", (x + 1), historyArray[x]);
		}
	}
}


void getHistory(char *cwd,char *command){
	char *remaining;
	int index;
	bool acceptableCommand = false;
	
	
	//checks an additional parameter hasnt been entered
	if(splitArray[1]!=NULL){
		printf("Error, history commands take no parameters\n");
		return;
	}
	else {
		//checks input to !! and runs command
		if(strncmp("!!",splitArray[0],2)==0){
			//checks there isnt a number and returns if there is as !! doesnt take a number
			if(splitArray[0][2] != NULL){
				printf("Invalid command, !! takes no index\n");
				return;
			}
			
			//run command on !!
			if(historyCounter == 0){
					printf("There is no history to execute\n");
				} else {
					for(int x=0;x<maxArray;x++){
						splitArray[x] = NULL;
					}
					fromHistory=true;
					command = historyArray[historyCounter-1];
					tokenize(cwd,command);
				}
		}
		//Checks the input starts with !-
		else if(strncmp("!-",splitArray[0],2)==0){
			//checks the first number is a number and that there isnt 3 values
			if((splitArray[0][2] > '9' || splitArray[0][2] < '0' || splitArray[0][4] != NULL)){
				printf("Not an acceptable index\n");
				return;
			}
			
			//checks to see if the number given is a single digit
			if(splitArray[0][3] == NULL){
				splitArray[0]++;
				splitArray[0]++;
				int pos=0;
				pos = atoi(splitArray[0]);
				
				//fetch single digit history command
				if(pos > historyCounter){
					printf("Error too large for history size\n");
				} else {
					for(int x=0;x<maxArray;x++){
						splitArray[x] = NULL;
					}
					fromHistory=true;
					command = historyArray[historyCounter-(pos)];
					tokenize(cwd,command);
				}
				
			}
			
			//checks to see if input is a 2 digit number and is actually a number
			if(splitArray[0][3] != NULL && splitArray[0][3] >= '0' && splitArray[0][3] <= '9'){
				//additional check to return if it isnt a number
				if(splitArray[0][3] > '9' || splitArray[0][3] < '0'){
					printf("Not an acceptable index\n");
					return;
				}
				
				splitArray[0]++;
				splitArray[0]++;
				
				int pos=0;
				pos = atoi(splitArray[0]);	
				
				//fetch double digit history command
				if(pos > historyCounter){
					printf("Error too large for history size\n");
				} else {
					for(int x=0;x<maxArray;x++){
						splitArray[x] = NULL;
					}
					fromHistory=true;
					command = historyArray[historyCounter-(pos)];
					tokenize(cwd,command);
				}
			}
			
			
		}
		else if(splitArray[0][0] == '!' && splitArray[0][3] == NULL){
			if((splitArray[0][1] > '9' || splitArray[0][1] < '0' || splitArray[0][3] != NULL)){
				printf("Not an acceptable index\n");
				return;
			}
			
			//checks to see if the number given is a single digit
			if(splitArray[0][2] == NULL){
				splitArray[0]++;
				int pos=0;
				pos = atoi(splitArray[0]);
				
				//fetch single digit history command
				if(pos >= historyCounter){
					printf("Error too large for history size\n");
				} else {
					for(int x=0;x<maxArray;x++){
						splitArray[x] = NULL;
					}
					fromHistory=true;
					command = historyArray[pos-1];
					tokenize(cwd,command);
				}
				
			}
			
			//checks to see if input is a 2 digit number and is actually a number
			if(splitArray[0][2] != NULL && splitArray[0][2] >= '0' && splitArray[0][2] <= '9'){
				//additional check to return if it isnt a number
				if(splitArray[0][2] > '9' || splitArray[0][2] < '0'){
					printf("Not an acceptable index\n");
					return;
				}
				
				splitArray[0]++;
				
				int pos=0;
				pos = atoi(splitArray[0]);	
				
				//fetch double digit history command
				if(pos >= historyCounter){
					printf("Error too large for history size\n");
				} else {
					for(int x=0;x<maxArray;x++){
						splitArray[x] = NULL;
					}
					fromHistory=true;
					command = historyArray[pos-1];
					tokenize(cwd,command);
				}
			}
		} else {
			printf("Error: Unexpected form of !\n");
		}
	}
}


void writeHistory(char *command){
	
	//checks if history is full and adds it at the end if it isnt
	if (historyCounter < historyLength){
		strcpy(historyArray[historyCounter], command);
		historyCounter++;

	} else {
		//moves all commands down one and removes the last one if full
		for (int x = 0; x < historyLength; x++){
			strcpy(historyArray[x], historyArray[x + 1]);	
		}
		
		//add new command to end of the array
		strcpy(historyArray[19], command);
	}
}

bool readInput(char *input,char *originalPath){
	
	//a boolean to check if input is too long
	bool overSizeLimit = true;
	
	//checks to see if no input has been given or ctrl-d has been pressed
	if(fgets(input,maxLength,stdin) == NULL){		
		printf("\nNo command given");
		exitShell(originalPath);
	} 
	else {
		for (int x = 0; x < maxLength; x++){
			//checks to see if the input has ended meaning the input isnt over the 512 limit
			if (input[x] == '\n'){
				overSizeLimit = false;
			}
		}
	}

	return overSizeLimit;
}

char *removeChars(char *string1){
	int counter = 0;
	
	for(int x=0;x<strlen(string1);x++){
		if(string1[x] >= '0' && string1[x] <= '9'){
			string1[counter] = string1[x];
			counter++;
		}	
	}
	
	string1[counter] = '\0';
	
	return string1;
}

void addAlias(){
	char *aliasName = splitArray[1];
	char *aliasCommand = splitArray[2];
	bool previousAliasPresent = false; 
	int previousAlias = 0;

//printing all aliases
	if(splitArray[1] == NULL){
		for (int i = 0; i < 10; i++)
		{
			if(aliases[0][0] == NULL){
				printf("There are no alias to display");
				return;
			}
			if(aliases[i][0] == NULL){
				return;
			}
			printf("%s %s\n", aliases[i][0], aliases[i][1]);
		}
	}
// checking if aliases is full
	if(aliases[9][0] != NULL){
		printf("The maximum number of aliases has been reached");
		return;
	}

	for (int i = 3; i < numTokens; i++)
	{
		aliasCommand = strcat(aliasCommand, " ");
		aliasCommand = strcat(aliasCommand, splitArray[i]);
	}


	int i = 0;
	while(aliases[i][0] != NULL)
	{
		if(strcmp(aliasName, aliases[i][0]) == 0){
			previousAliasPresent = true;
			previousAlias = i;
			printf("An alias of this name has already been defined\n");
			break;
		}
		i++;
	}

	if(previousAliasPresent == true){
		aliases[previousAlias][0] = aliasName;
		aliases[previousAlias][1] = aliasCommand;
		printf("previous alias override successful\n");
		return;
	}
//creating new alias
	for (int i = 0; i < 10; i++)
		{
			if(aliases[i][1] == NULL){
				aliases[i][0] = aliasName;
				aliases[i][1] = aliasCommand;
				break;
			}
		}
//clearing values
	aliasCommand = "";
	aliasName = "";
	printf("alias saved\n");
}

void removeAlias(){
	char *aliasName = splitArray[1];
	int location = -1;
	int i = 0;
	while(aliases[i][0] != NULL)
	{
		if(strcmp(aliasName, aliases[i][0]) == 0){
			location = i;
			break;
		}
		i++;
	}
	if(location != -1){
		for (int i = location; i < 9; i++)
		{
			aliases[i][0] = aliases[i+1][0];
			aliases[i][1] = aliases[i+1][1];
		}
		
	}else{
		printf("That alias is not found");
	}
}

void writeAliasToFile(){
	FILE *file = fopen("a.aliases", "w");
	int i = 0;

	if(file == NULL){
		perror("Alias file write:");
	}else{
		while(aliases[i][0] != NULL)
		{
			fprintf(file, "alias %s %s\n", aliases[i][0], aliases[i][1]);
			i++;
		}
	fclose(file);	
	}
}

void readAliasFromFile(char *cwd){

	FILE *file = fopen("a.aliases", "r");
	if (file == NULL) {
		perror("Alias file read: ");
		return;
    }

    char line[maxLength];
	
    while(fgets(line, 512, file)!=NULL) {
		printf("%s", line);
		tokenize(cwd, line);

    }

    fclose(file);
}

