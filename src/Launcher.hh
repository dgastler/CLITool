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

#ifndef __LAUNCHER_HH__
#define __LAUNCHER_HH__


#include <string>
#include <vector>

#include <stdint.h> //unit****
#include <cstdio> //printf
#include <assert.h> //assert

//class CLI; //Forward decloration for friendship  

class Launcher {
public:
  Launcher();
  ~Launcher();
    
  int EvaluateCommand(std::vector<std::string> command);

  //For CLI to auto complete commands
  std::string AutoCompleteCommand(std::string const & line,int state);  
  std::string AutoCompleteSubCommand(std::vector<std::string> const & line,
				     std::string const & currentToken,int state);
  
private:
  //A mapping between command string and function to call
  //The called function returns an int and takes an AMC13 class, a vector of argument strings and a vector of uint64_t conversions of those strings
  void AddCommand(std::string name, 
		  int (Launcher::*)(std::vector<std::string>),
		  std::string help, 
		  std::string (Launcher::*)(std::vector<std::string> const &,std::string const &,int)=NULL);
  void AddCommandAlias( std::string alias, std::string existingCommand );
  int FindCommand(std::string command);
    
  std::vector<std::string> commandName;
  std::vector<int (Launcher::*)(std::vector<std::string>)> commandPtr;
  std::vector<std::string> commandHelp;
  std::vector<std::string (Launcher::*)(std::vector<std::string> const &,std::string const &,int)> commandAutoComplete;
  
    
  //====================================================
  //Add your commands here
  //====================================================
    
  //Here is where you update the map between string and function
  void LoadCommandList();
    
  //Add new command functions here
  int Help(std::vector<std::string>);	   
  std::string autoComplete_Help(std::vector<std::string> const &,std::string const &,int);
  int Quit(std::vector<std::string>);	   
  int Echo(std::vector<std::string>);	   
};

#endif
  
