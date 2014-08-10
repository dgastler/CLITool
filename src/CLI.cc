/*
Copyright (C) 2014 Daniel Gastler

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <CLI.hh>
#include <cstdlib>

CLI::CLI()
{
  // the include & load commands
  includeString = "include";
  loadString = "load";
  
  //The prompt style
  prompt = ">";

  //The comment character
  commentChar = '#';
  
  //Filelevel (for recursion control)
  fileLevel = 0;
}


CLI::~CLI(){/*no cleanup needed*/}

int CLI::ProcessFile(std::string filename)
{
  if(fileLevel > 4){ //Prevent
    fprintf(stderr,"Too many file include levels.\n");
    return -1;
  }
  fileLevel++;
  
  int foundCommands = 0;
  //open script file
  if(filename.find(" ") != std::string::npos){
    filename = filename.substr(0,filename.find(" "));
  }
  std::ifstream inFile(filename.c_str(),std::ifstream::in);
  //Check that the file opened and fail if it didn't
  if(inFile.fail()){
    fprintf(stderr,"Bad file: %s\n",filename.c_str());
    fileLevel--;
    return -1;
  }
  //read file until we are at the end
  while(!inFile.eof()){
    std::string line;
    //Read one line of the file
    std::getline(inFile,line);
    
    //Process this line
    int lineCommandCount = ProcessLine(line);
    //Check for a parse error
    if(lineCommandCount == -1){
      fileLevel--;
      return -1;
    }
    //Update the command count
    foundCommands+=lineCommandCount;
  }
  fileLevel--;
  return foundCommands;
}
  
int CLI::ProcessLine(std::string line)
{
  //count of found commads in this string
  int foundCommands = 0;
  
  //eat white space at the beginning of the line
  while((line.size() > 0) && (line[0] == ' ')){
    line.erase(0,1); //remove the first char
  }
  
  //Check that we still have a line
  if(line.empty()){
    //move on if we don't
    return 0;
  }
  
  //check if this line is commented out
  if(line[0] == commentChar){
    return 0;  // move on to the next line
  }
  
  //Check for an include
  if((line.find(includeString) == 0) || //if we find includeString at the beginning of the file
     (line.find(loadString) == 0)) {//if we find load
    //extract the filename and pass it to ProcessFile for parsing
    std::string includeFilename;
    if(line.find(includeString) == 0) {
      includeFilename = line.substr(includeString.size());
    } else {
      includeFilename = line.substr(loadString.size());
    }	
    //remove any white space
    while((includeFilename.size() > 0) && (includeFilename[0] == ' ')){
      includeFilename.erase(0,1); //remove the first char
    }      
    
    //process the include file
    int foundSubCommands = ProcessFile(includeFilename);
    
    //Check if we found anything or got an error
    if(foundSubCommands < 0){
      fprintf(stderr,"Bad subfile(%d): %s, bailing out!\n",fileLevel,line.c_str());
      return -1;
    }
    foundCommands+=foundSubCommands;
  } else { //We have some other text, so treat it as a command    
    //Add a command
    Commands.push_back(line);
    foundCommands++;     
  }
  
  return foundCommands;
}

  
std::vector<std::string> CLI::GetInput(Launcher * launcher)
{
  //Command string
  std::string currentCommand("");
  
  //Connect to the Launcher's auto-complete if it is used.
  rl_attempted_completion_function = CLISetAutoComplete(launcher);
     
  //Load a command
  
  //Check if we have a command from a loadedfile
  if(Commands.size()){
    //load the first command into currentCommand
    currentCommand = Commands.front();
    //delete that first command
    Commands.pop_front();
  } else {
    //std::getline(std::cin,currentCommand);
    //Get user input from gnu readline
    char * rlLine = readline(prompt.c_str());
    if( rlLine == NULL){
      fprintf(stderr,"EOF on prompt!\n");	  
      exit(0);
    } else {
      //convert readline string to std::string
      currentCommand = std::string(rlLine);      
      free(rlLine);  //Free readline's string
      
      //Parse the command string
      ProcessLine(currentCommand);
      if(Commands.size()){
	//load the first command into currentCommand
	currentCommand = Commands.front();
	//delete that first command
	Commands.pop_front();
      }
    }
  }
  if(!currentCommand.empty()){
    //Add the command to the history
    add_history(currentCommand.c_str());
  }
  
  //Parse the command
  return SplitString(currentCommand);  
}
  
    
int CLI::ProcessString(std::string command)
{
  int foundCommands = 0;
    
  while(!command.empty()){
    //Find first split char
    size_t pos = command.find('\n');
    if(pos != std::string::npos){
      //Process this line
      int foundSubCommands = ProcessLine(command.substr(0,pos));
      if(foundSubCommands == -1){
	return -1;
      }
      foundCommands += foundSubCommands;
      
      //Remove it from the command
      command = command.substr(pos+1,std::string::npos);
    } else {
      //Process this line
      int foundSubCommands = ProcessLine(command);
      if(foundSubCommands == -1){
	return -1;
      }
      foundCommands += foundSubCommands;
      
      //Empty the string
      command.clear();
    }
  }
  return ProcessLine(command);
}

