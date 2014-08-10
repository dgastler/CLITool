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

#include <ToolException/ToolException.hh>

using namespace ToolException;

//Error string for stack string allocation error
static const char stackError[] = "Stack allocation failed.\n";

const char * exBase::StackTrace() const throw() 
{
  if(stackBuffer == NULL)
    return stackError;
  return stackBuffer;
}

void exBase::AppendStackLine(const char * line) throw()
{
  //Check for NULL line
  if(line == NULL)
    return;

  //get raw line size
  size_t lineSize = strlen(line);
  if(lineSize >= (stackSize - stackUsed))
    lineSize = stackSize - stackUsed;
  //copy string
  strncpy(stackBuffer+stackUsed,line,lineSize);
  //update size
  stackUsed += lineSize;
  //Add eol
  if((stackSize - stackUsed) > 1)
    {
      stackBuffer[stackUsed] = '\n';
      stackUsed++;
    }
  //add null terminator (there is always room for one(unless NULL))
  if(stackBuffer != NULL)
    {
      stackBuffer[stackUsed] = '\0';
    }
}

void exBase::GenerateStackTrace() throw()
{
  //Get the thread/process ID for this stack trace
  PID = syscall(SYS_gettid);  

  //Get the stack trace (void *s)
  const size_t maxStackSize = 20;
  void * functionStack[maxStackSize];
  size_t stackSize;

  //Fill functionStack with void * pointers to return address of the corresponding stack frame
  //filed up to maxStackSize with most recent calls
  stackSize = backtrace(functionStack,maxStackSize);

  //Get the stack trace strings  (we have to free this)
  char ** functionNames = backtrace_symbols(functionStack,stackSize);
  if(functionNames == NULL)
    {
      fprintf(stderr,"Error in exBase::GenerateStackTrace(). backtrace_symbols failed\n");
      return;
    }

  char * demangledName = NULL;
  //Start stack trace from 3 to skip the exception class's functions
  for(size_t iFunction = 3; iFunction < stackSize; iFunction++)
    {
      //based off of http://panthema.net/2008/0901-stacktrace-demangled/
      //Be paranoid about these pointers and make sure they are valid
      if(functionNames[iFunction] == NULL)
	{
	  AppendStackLine("Unknown");
	  continue;
	}

      char * mangledStart = NULL;
      char * offsetStart = NULL;
      char * offsetEnd = NULL;
      
      //Find mangled and offset start/ends
      char * bufferEnd = functionNames[iFunction] + strlen(functionNames[iFunction]);
      for(char * buffer = functionNames[iFunction];buffer != bufferEnd;buffer++)
	{
	  //Look for function's opening '('
	  if(*buffer == '(')
	    mangledStart = buffer;
	  //Look for offset in function
	  else if(*buffer == '+')
	    offsetStart = buffer;
	  //Look for closing ')', but only if we've found an opening one
	  else if( (*buffer == ')') && (offsetStart != NULL))
	    {
	      offsetEnd = buffer;
	      break;
	    }
	}
      
      //if the parsing makes sense
      if( (mangledStart != NULL) &&
	  (offsetStart != NULL) &&
	  (offsetEnd != NULL) &&
	  (mangledStart < offsetStart)
	  )
	{
	  //Make manglesStart point to the beginning of the name
	  mangledStart++;
	  //put in a null terminator at offsetStart to end the mangled name
	  *offsetStart = '\0';
	  
	  //Make offset start point to the beginning of the offset
	  offsetStart++;
	  //put a null terminator at the end of the offset
	  *offsetEnd = '\0';

	  //demangle
	  size_t demangledSize = 0;
	  int demangledRet = 0;
	  char * retName = abi::__cxa_demangle(mangledStart,
					       demangledName,
					       &demangledSize,
					       &demangledRet);
	  //Append demangled line
	  if((demangledRet == 0) && (retName != NULL))
	    {
	      //copy possibly reallocated c-string
	      demangledName = retName;
	      AppendStackLine(demangledName);
	    }
	  else  //if ther was an error, append the mangled name 
	    {
	      AppendStackLine(mangledStart);
	    }
	}
      else //Bad parse, just print the line
	{
	  AppendStackLine(functionNames[iFunction]);
	}      
    }

  if(demangledName != NULL)
    free(demangledName);
  free(functionNames);
  
}

