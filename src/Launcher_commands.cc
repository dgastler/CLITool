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

void Launcher::LoadCommandList()
{  
  // general commands
  AddCommand("load",NULL,"load a script file");//Built-in
  AddCommand("help",&Launcher::Help,"",&Launcher::autoComplete_Help);
  AddCommandAlias( "h", "help");
  AddCommand("quit",&Launcher::Quit,"Close program");
  AddCommandAlias("q", "quit");
  AddCommand("exit",&Launcher::Quit,"Close program");
  AddCommand("echo",&Launcher::Echo,"echo to screen");  
}
  
  
int Launcher::Quit(std::vector<std::string>)
{
  //Quit CLI so return -1
  return -1;
}
  
int Launcher::Echo(std::vector<std::string> strArg) 
{
  for(size_t iArg = 0; iArg < strArg.size();iArg++){
      printf("%s ",strArg[iArg].c_str());
    }
  printf("\n");
  return 0;
}
  
  
int Launcher::Help(std::vector<std::string> strArg) 
{
  if(strArg.size() > 0){
      int cmd = FindCommand(strArg[0]);
      if(cmd >= 0){
	  printf("\t%s:\t%s\n",commandName[cmd].c_str(),commandHelp[cmd].c_str());
	}
      return 0;
    }
  printf("Commands:\n");  
  for(size_t i = 0; i < commandName.size();i++){
      printf(" %-12s:   %s\n",commandName[i].c_str(),commandHelp[i].c_str());
    }
  return 0;
}

std::string Launcher::autoComplete_Help(std::vector<std::string> const & line,std::string const & currentToken ,int state)
{  
  if(line.size() > 0){
    if((line.size() > 1) && (currentToken.size() == 0)){
	return std::string("");
      }
    static size_t pos;
    if(!state) {
      //Check if we are just starting out
      pos = 0;
    } else {
      //move forward in pos
      pos++;
    }
    for(;pos < commandName.size();pos++){
	if(commandName[pos].find(currentToken) == 0){
	  return commandName[pos];
	}
      }
  }
  //not found
  return std::string("");  
}
