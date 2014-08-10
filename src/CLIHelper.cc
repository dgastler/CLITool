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

#include <CLIHelper.hh>

#include <Launcher.hh>

#include <new> //for bad_alloc
#include <stdlib.h>

//gnu readline
#include <readline/readline.h>
#include <readline/history.h>


//Auto-completing state variables
static Launcher * launcher;
static std::string command;

static char ** helperFunction(char const * text, int start, int end);
static char * helperFunctionCommand(char const * text,int state);
static char * helperFunctionSubCommand(char const * text,int state);


std::vector<std::string> SplitString(std::string command)
{
  std::vector<std::string> commandArgv;
  while(!command.empty()){
    //Find first split char
    size_t pos = command.find(' ');
    if(pos != std::string::npos){
      //Add this string to our list
      commandArgv.push_back(command.substr(0,pos));
      //Remove it from the command
      command = command.substr(pos+1,std::string::npos);
    }else{
      //This is the last command, so add it to the list
      commandArgv.push_back(command);
      //Empty the string
      command.clear();
    }
  }
  //return this command
  return commandArgv;
}

//Set launcher state
char ** (*CLISetAutoComplete(Launcher * _launcher))(char const *, int, int)
{
  //Load the local launcher pointer
  launcher = _launcher;
  return helperFunction;
}



//===============================================================================
// Readline helper functions
//===============================================================================

//Helper function for calling the AutoComplete function from Launcher
static char * helperFunctionCommand(char const * text,int state)
{
  if(launcher != NULL){
    //Auto complete from launcher
    std::string complete = launcher->AutoCompleteCommand(text,state);
    if(complete.empty()){
      return NULL;
    }
    
    //Allocate the return string (with NULL)
    char * ret = (char *) malloc(complete.size()+1);
    if(ret == NULL){
      std::bad_alloc e;
      throw e;
    }
    //copy string
    memcpy(ret,complete.c_str(),complete.size());
    ret[complete.size()] = '\0'; //Null terminate
    return ret;
  }
  return NULL;
}

//Helper function for calling the AutoComplete function from Launcher
static char * helperFunctionSubCommand(char const * text,int state)
{
  if(launcher != NULL){
    //Auto complete from launcher
    std::string complete = launcher->AutoCompleteSubCommand(SplitString(rl_line_buffer),text,state);
    if(complete.empty()){
      return NULL;
    }
    
    //Allocate the return string (with NULL)
    char * ret = (char *) malloc(complete.size()+1);
    if(ret == NULL){
      std::bad_alloc e;
      throw e;
    }
    //copy string
    memcpy(ret,complete.c_str(),complete.size());
    ret[complete.size()] = '\0'; //Null terminate
    return ret;
  }
  return NULL;
}


//helper function for calling... our helper functions
//It determines if we are auto-completing a command or a sub command 
//and then calls the appropriate matching functions
static char ** helperFunction(char const * text, int start, int end)
{
  //Check if we are at the beginning of the line
  if(start == 0){
    
    //Reset state
    command.clear();
    
    //Find matches
    return  rl_completion_matches(text,helperFunctionCommand);
  } 
  
  //Determine if the command has been completed
  command.assign(rl_line_buffer);
  size_t commandEndPosition = command.find(' ');
  if(commandEndPosition == std::string::npos){
    command.clear();
  } else {
    command=command.substr(0,commandEndPosition);
  }
  
  //Match subcommands
  if (command.size() > 0){
    return rl_completion_matches(text,helperFunctionSubCommand);
  }
  //auto complete filenames
  return NULL;
}

