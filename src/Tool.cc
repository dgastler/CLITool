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

#include <string>
#include <vector>
#include <iostream>
#include <sstream>


//TCLAP parser
#include <tclap/CmdLine.h>

#include <CLI.hh>
#include <Launcher.hh>
#include <ToolException/ToolException.hh>


int main(int argc, char* argv[]) 
{
  //Create CLI
  CLI cli;

  try {
    TCLAP::CmdLine cmd("Tool for talking to AMC13 modules.",
		       ' ',
		       "AMC13Tool v2");
    
    //AMC 13 connections
    TCLAP::MultiArg<std::string> amc13Connections("c",                   //one char flag
						  "connect",             // full flag name
						  "connection file",     //description
						  false,                 //required
						  "string",              // type
						  cmd);

    //Script files
    TCLAP::MultiArg<std::string> scriptFiles("X",                   //one char flag
					     "script",              // full flag name
					     "script filename",     //description
					     false,                 //required
					     "string",              // type
					     cmd);
    //Parse the command line arguments
    cmd.parse(argc,argv);

    
    //setup connections
    for(std::vector<std::string>::const_iterator it = amc13Connections.getValue().begin(); 
	it != amc13Connections.getValue().end();
	it++){
	cli.ProcessString("connect " + *it);
      }

    //Load scripts
    for(std::vector<std::string>::const_iterator it = scriptFiles.getValue().begin(); 
	it != scriptFiles.getValue().end();
	it++){
	cli.ProcessFile(*it);
      }

  } catch (TCLAP::ArgException &e) {
    fprintf(stderr, "Error %s for arg %s\n",
	    e.error().c_str(), e.argId().c_str());
    return 0;
  }


  //============================================================================
  //Main loop
  //============================================================================

  //Create Command launcher
  Launcher launcher;

  bool running = true;
  while (running) {
      //Get parsed command
      std::vector<std::string> command = cli.GetInput(&launcher);

      //Launch command function
      try {
	if(launcher.EvaluateCommand(command) < 0){
	    //Shutdown tool
	    running = false;
	  }     
      }catch (ToolException::exBase & e){
	std::string errorstr(e.what());
	std::cout << "\n\n\nCaught Tool exception: " << errorstr << std::endl;
	std::cout << ToolException::LimitStringLines(e.Description());
	cli.ClearInput(); //Clear any scripted commands
      }catch (std::exception  & e){
	std::string errorstr(e.what());
	errorstr.erase(std::remove(errorstr.begin(), 
				   errorstr.end(), 
				   '\n'), 
		       errorstr.end());
	
	std::cout << "\n\n\nCaught std::exception " << errorstr << ". Shutting down.\n";
	running = false;
	cli.ClearInput(); //Clear any scripted commands
      }
    }
  return 0;
}
