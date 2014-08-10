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

#include <Launcher.hh>

Launcher::Launcher()
{
  LoadCommandList(); //Load list
}
  
Launcher::~Launcher()
{
}
  
int Launcher::EvaluateCommand(std::vector<std::string> command)
{
  //Handle no command
  if(command.size() == 0) {
    return 0;
  }
    
  //Search for command
  int commandIndex = FindCommand(command[0]);
  if(commandIndex >= 0) {
    //Command found!
      
    //Build the string & uint64_t argument vector
    std::vector<std::string> argListString;
    for(size_t iArg = 1; iArg < command.size();iArg++) {
      argListString.push_back(command[iArg]);
    }
      
    //Call the function associated with this command
    //If the return value is negative, we will quit
    if(commandPtr[commandIndex] != NULL){
      return (*this.*(commandPtr[commandIndex]))(argListString);
    }
    return 0;
      
  } else { //command not found
    printf("Bad Command: ");
    if( command.size()){
      printf("%s\n",command[0].c_str());
    }else{
      printf("\n");
    }
    return 0;
  }
}

int Launcher::FindCommand(std::string command)
{
  for(size_t iCmd = 0; iCmd < commandName.size();iCmd++){
    //Check against each internal command
    if((command.size() == commandName[iCmd].size()) &&
       (command.compare(commandName[iCmd]) == 0) ){
      return iCmd; //found command (return index)
    }
  }
  return -1; //Not found
}


void Launcher::AddCommandAlias( std::string alias, std::string existingCommand )
{
  int cmd = FindCommand( existingCommand);
  if( cmd >= 0) {
    AddCommand( alias, commandPtr[cmd], commandHelp[cmd], commandAutoComplete[cmd]);
  } else {
    fprintf( stderr, "Tried to add alias %s to command %s which doesn't exist!\n",
	     alias.c_str(), existingCommand.c_str());
  }
}

void Launcher::AddCommand(std::string name, 
			  int (Launcher::* fPtr)(std::vector<std::string>),
			  std::string help, 
			  std::string (Launcher::* acPtr)(std::vector<std::string> const &,std::string const &,int))
{
  //Checks for consistency 
  assert(commandName.size() == commandPtr.size());
  assert(commandName.size() == commandHelp.size());
  assert(commandName.size() == commandAutoComplete.size());
    
  commandName.push_back(name);
  commandPtr.push_back(fPtr);
  commandHelp.push_back(help);
  commandAutoComplete.push_back(acPtr);
}

std::string Launcher::AutoCompleteCommand(std::string const & line,int state) 
{
  static size_t pos;
  if(!state) {
    //Check if we are just starting out
    pos = 0;
  } else {
    //move forward in pos
    pos++;
  }
  for(;pos < commandName.size();pos++){
    if(commandName[pos].find(line) == 0){
      return commandName[pos];
    }
    //compare
  }
  //not found
  return std::string("");
}

std::string Launcher::AutoCompleteSubCommand(std::vector<std::string> const & line,
					     std::string const & currentToken,int state) 
{
  //find command
  if(line.size() >0){
    int iCommand = FindCommand(line[0]);
    if(iCommand >= 0){
      //Call the command's auto complete function
      if (commandAutoComplete[iCommand] != NULL) {
	return (*this.*(commandAutoComplete[iCommand]))(line,currentToken,state);
      }
      return std::string("");
    }
  }
  return std::string("");
}
