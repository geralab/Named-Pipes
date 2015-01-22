
/*
 * Gerald Blake
 * CS 4323 Program 1
 * CWID:11034027
 * This program will read from a file or string
 * Using the Client/Server model
 * It will send the information to a server via named pipes
 * for processing. The server will detect if a given
 * token is a file or string and print statistics to the command line
 * it shall print frequency per line, an overall occurrence of given token,
 * and the line upon which the token has occured
 *
 */
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include<string>
#include<string.h>
#include<iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include<cctype>
#include<ctime>
#include<sstream>
#include<vector>
//pipe file path
int client_to_server=0;
char *thePipe = (char*)"client_to_server_fifo";
int server_to_client=0;
char *thePipe2 = (char*)"server_to_client_fifo";
//buffer
char str[BUFSIZ];
//prototypes
std::ostringstream convert;
std::string returnAlpha(std::string text);
std::string toLower(std::string text);
std::string toUpper(std::string text);
std::string getDateTime();
bool check(bool &done,std::string text);
std::vector<std::string> gtokens(std::string text,const char * delimeters);
void clearStream();
//begin main
int main()
{
	printf("\nCLIENT:PID - %i CLIENT STARTED ON %s\n\n",getpid(),(getDateTime()).c_str());
    char* serverPath = (char*)"Server";
	 //create the FIFO (named thePipe) 
    mkfifo(thePipe, 0666);
    mkfifo(thePipe2, 0666);
	pid_t pid = fork(); //Create a child process 
	switch (pid) 
	{
		case -1: //Error 
			std::cerr << "fork failed.\n";
			exit(1);
		case 0: //Child process 
			execl(serverPath, NULL); //Execute the program 
			std::cerr << "execl() failed!"; //execl doesn't return unless there's an error 
			exit(1);	
	}
	
   bool done = false;
   printf("SERVER:PID - %i SERVER STARTED ON %s\n ",pid,(getDateTime()).c_str());
   //client loop
   while(!done)
   {
       //input variables
       std::string fname,target;
       //write str to the FIFO 
		client_to_server = open(thePipe, O_WRONLY);
		server_to_client = open(thePipe2, O_RDONLY);
	   //get name or string input
	   printf("CLIENT:PID - %i Enter file name or string: ",getpid());
	   std::getline(std::cin,fname);
	   if(check(done,fname))
	   {
			memset(str, 0, sizeof(str));
			continue;
	   }
	   //get target input
	   printf("\nCLIENT:PID - %i Enter target: ",getpid());
	   std::getline(std::cin,target);
	   if(check(done,target)) 
	   {
			memset(str, 0, sizeof(str));
			continue;
	   }
	   printf("\nCLIENT:PID %i - INPUTFILE/STRING>>%s<<\n ",getpid(),fname.c_str());
	   printf("CLIENT:PID %i - TARGET>>%s<<\n",getpid(),target.c_str());
	   
	   //check if authorized....
	   strcpy(str,"coolclient");
	   //send client authorization to server
	
	   write(client_to_server, str, BUFSIZ);
	   //get server authorization from server
	 
	   read(server_to_client,str,sizeof(str));
	
	   //check authorization phrase
	   if(toLower(returnAlpha(str)) == "coolserver")
	   {
			printf("\nCLIENT:PID - %i SYNCHRONIZED TO SERVER \n",getpid());
	   }
	   else
	   {
			printf("\nCLIENT:PID - %i NOT AUTHORIZED \n",getpid());	
			if(check(done,"server-eof"))
			{
			    memset(str, 0, sizeof(str));
				continue;
				
			}
			
	   }
	   
	   strcpy(str,fname.c_str());
       write(client_to_server, str, sizeof(str));
	   //perror("Write:");
	
	   strcpy(str,target.c_str());
	   write(client_to_server, str, sizeof(str));
	   //perror("Write:"); // error check
	 
       printf("\n");    
	   read(server_to_client,str,sizeof(str));
	
	   printf("\n");
	   //perror("Read:"); // error check
	   printf("\n");
	   
	   std::vector<std::string> tokens = gtokens(std::string(str),"\n");
	   for(int i = 0; i < tokens.size();i++)
	   {
			printf("\nCLIENT:PID - %i Message from server: %s\n\n",getpid(),(tokens[i]).c_str());
	   }
	   
   }
   clearStream();
   printf("\nCLIENT:PID - %i CLIENT TERMINATED ON %s\n\n",getpid(),(getDateTime()).c_str());
   clearStream();
   close(client_to_server);
   close(server_to_client);
   unlink(thePipe);
   unlink(thePipe2);
   //remove the FIFO
  
   return 0;
}//end main
//return trimed string
std::string returnAlpha(std::string text)
{
   std::string temp = "";
   for(int i = 0; i < text.size();i++)
   {
		if(!isspace(text[i]))
			temp+=text[i];
   }
   return temp;
}
//return lowercase string
std::string toLower(std::string text)
{
   for(int i = 0; i < text.size();i++)
   {
		text[i] = tolower(text[i]);
   }
   return text;
}
//return uppercase string
std::string toUpper(std::string text)
{
   for(int i = 0; i < text.size();i++)
   {
		text[i] = toupper(text[i]);
   }
   return text;
}
//check if sentinal Server-EOF detected
bool check(bool &done,std::string text)
{
	if(toLower(returnAlpha(text))=="server-eof")
	{
		strcpy(str,"server-eof");
		write(client_to_server, str, sizeof(str));
		done = true;
		memset(str, 0, sizeof(str));
	}
	return done;
}
//return date and time
std::string getDateTime()
{
    time_t rawtime;
	struct tm * timeinfo;
	char buffer [80];

	time (&rawtime);
	timeinfo = localtime (&rawtime);

    strftime (buffer,80,"%A %B %e, %Y at %r",timeinfo);
   // puts (buffer);
	
    return std::string(buffer);
}

//splits string into tokens at the occurence of a deleimeters
std::vector<std::string> gtokens(std::string text,const char * delimeters)
{
    std::vector<std::string> tokens;
    int len = text.length() + 1;
    char *str = new char[len];
    strcpy(str,text.c_str());
    char * pch;
    pch = strtok (str,delimeters);
    while (pch != NULL)
    {
        tokens.push_back(pch);
        pch = strtok (NULL, delimeters);
    }
    return tokens;
}
void clearStream()
{
	convert.str("");
    convert.clear();
}