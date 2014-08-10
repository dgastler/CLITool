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

#ifndef AMC13_CLI_HH
#define AMC13_CLI_HH

#include <string>
#include <deque>
#include <vector>

#include <fstream>
#include <iostream>
 
//gnu readline
#include <readline/readline.h>
#include <readline/history.h>

#include <CLIHelper.hh>

class CLI
{
public:
  CLI();
  ~CLI();
  
  //Load a script file
  int ProcessFile(std::string filename);
  
  //Inject a command string
  int ProcessString(std::string command);
  
  //Get the next command (std::cin or from file)
  //From file first, then parse cin
  std::vector<std::string> GetInput(Launcher * launcher = NULL);
  
  //Clear input (if there is a queue of input)
  void ClearInput(){Commands.clear();}
  
private:
  //Get a command string
  int ProcessLine(std::string line);
  
  //Interally stored commands
  std::deque<std::string> Commands;
  
  //include file recusion overflow stopper
  int fileLevel;
  
  //include command
  std::string prompt;
  std::string includeString;
  std::string loadString;
  char commentChar;
};
#endif
