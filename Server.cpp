
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
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include<string>
#include<sstream>
#include <ctime>
#include <fstream>
#include<vector>

std::ostringstream convert;
std::ifstream input;

int client_to_server=0;
char *thePipe = (char*)"client_to_server_fifo";
int server_to_client=0;
char *thePipe2 = (char*)"server_to_client_fifo";
char buf[BUFSIZ];

//prototypes
std::vector<std::string> gtokens(std::string text,const char * delimeters);
std::string returnAlpha(std::string text);
std::string toLower(std::string text);
std::string toUpper(std::string text);
std::string getDateTime();
bool check(bool &done, std::string text);
void calcStat(std::string file,std::string target);
void clearStream();
//begin main
int main()
{
   
    
    //open, read, and display the message from the FIFO
	
   
    bool done = false;
    while (!done)
    {
		
			client_to_server = open(thePipe, O_RDONLY);
		
		
			server_to_client = open(thePipe2, O_WRONLY);
		
	   //check if authorized....
	   read(client_to_server, buf, BUFSIZ);
	   if(check(done,buf)) continue;
	   if(toLower(returnAlpha(buf)) == "coolclient")
	   {
			printf("\nSERVER:PID - %i SYNCHRONIZED TO CLIENT \n",getpid());
			strcpy(buf,"coolserver");
			write(server_to_client,buf,BUFSIZ);
	   }
	   else
	   {
			printf("\nSERVER:PID - %i NOT AUTHORIZED \n",getpid());
			done = true;
			continue;
	   }
	   //read from client
	   read(client_to_server, buf, BUFSIZ);
	   std::string file = std::string(buf);
	   
	 
		//read from client
	   read(client_to_server, buf, BUFSIZ);
	   std::string target = std::string(buf);
	
	
		
		calcStat(file,target);
		//Pass information to client
		write(server_to_client,buf,BUFSIZ);
	
		
		//clean buffer
		memset(buf, 0, sizeof(buf));
		
    }
    //return pipes to os
	input.close();
	//clear ostringstream stream
	clearStream();
	memset(buf, 0, sizeof(buf));
	strcpy(buf,"");
	printf("\n\nSERVER:PID - %i SERVER TERMINATED ON %s\n\n\n",getpid(),(getDateTime()).c_str());
	close(client_to_server);
	close(server_to_client);
    unlink(thePipe);
    unlink(thePipe2);
    return 0;
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
//return trimmed stirng 
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
//return upper case string
std::string toUpper(std::string text)
{
   for(int i = 0; i < text.size();i++)
   {
		text[i] = toupper(text[i]);
   }
   return text;
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
//calculate assignment related statistics
void calcStat(std::string file,std::string target)
{
    input.open(file.c_str());
    std::string data="";
    int lineNumber = 1;
    int occurrencesInLine = 0;
    int totalOccurrences = 0;
	std::string e ="";
	//SEARCH FILE FOR TEXT IF FILE
	if(input.is_open())
	{
		while(getline(input,data))
		{
			//e+=data+"\n";
			std::vector<std::string> tokens = gtokens(data," ");
			for(int i = 0; i < tokens.size();i++)
			{
				if(toLower(returnAlpha(tokens[i])) == toLower(returnAlpha(target)))
				{
					occurrencesInLine++;
					
				}
				//e+=tokens[i]+"\n";
			}
			totalOccurrences+=occurrencesInLine;
			//e+="\n";
			convert << "TARGET>>"<<target << "<< Appeared on line " << lineNumber <<",    " <<occurrencesInLine<<" times\n";
			lineNumber++;
			occurrencesInLine=0;
		}
		 convert << "TARGET>>"<<target << "<< Occured a total of "<<totalOccurrences<<" times\n";
		 //printf("Tokens: %s",e.c_str());
		 strcpy(buf,(convert.str()).c_str());
		 clearStream();
	 }
	 else
	 {
	   //SEARCH STRING OF TEXT
	  
		std::vector<std::string> theString = gtokens(file," ");
		for(int i = 0; i < theString.size();i++)
		{
			if(toLower(returnAlpha(theString[i])) == toLower(returnAlpha(target)))
			{
					occurrencesInLine++;
					
			}
		}
		 convert << "TARGET>>"<<target << "<< Occured a total of "<<occurrencesInLine<<" times in the string\n";
		 //printf("Tokens: %s",e.c_str());
		 strcpy(buf,(convert.str()).c_str());
		 clearStream();
	 }
     input.close();
}

//check if sentinal Server-EOF detected
bool check(bool &done, std::string text)
{
	if(toLower(returnAlpha(text))=="server-eof")
	{
		done = true;
		memset(buf, 0, sizeof(buf));
		
	}
	return done;
}


void clearStream()
{
	convert.str("");
    convert.clear();
}