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

#ifndef __TOOLEXCEPTION_H__
#define __TOOLEXCEPTION_H__

#include <exception> //for inheritance from std::exception
  
#include <string.h> //for memcpy, strlen.

#include <string> //std::string

#include <sstream> // for ostream

#include <stdlib.h> // for malloc

#include <stdio.h>

//For SYS_gettid()
//#define _GNU_SOURCE          /* See feature_test_macros(7) */
#include <unistd.h>
#include <sys/syscall.h>    /* For SYS_xxx definitions */

//Backtrace code
#include <execinfo.h>

//abi demangling
#include <cxxabi.h>


//Error strings
#define DescriptionError "Description allocation failed.\n"

namespace ToolException
{
  //Function to limit the output of long exception descriptions
  std::string LimitStringLines(std::string source,
			       size_t beginLineCount = 5,
			       size_t endLineCount= 2);

  //Macro for derived ToolException classes
#define ToolExceptionClass( ClassName , ClassDescription )		\
  class ClassName : public ToolException::exBase {			\
  public:								\
    ClassName() throw() {Init();}					\
    ClassName(const ClassName & rh) throw() {Init();Copy(rh);}		\
    ClassName & operator=(const ClassName & rh) throw()			\
      {Init();Copy(rh);return *this;}					\
    ~ClassName() throw() {}						\
    void ThrowAsDerivedType() const { throw ClassName(*this); }		\
    const char * what() const throw() {return whatname;}		\
  private:								\
    void Init()								\
    {									\
      strcpy(whatname,ClassDescription);				\
    }									\
    char whatname[sizeof(ClassDescription)];				\
  };
  
  
  class exBase : public std::exception
    {
    public:
      //destructor
      virtual ~exBase() throw();
 
      //Returns a stack trace of where the ToolException happened
      const char * StackTrace() const throw();
      
      //Append info to the description message
      void Append(const char * buffer) throw();
      void Append(std::string str){Append(str.c_str());};
      const char * Description() const throw();

      //Return the what string
      virtual const char * what() const throw()  = 0;

      virtual void ThrowAsDerivedType() const = 0;

      pid_t GetPID(){return PID;};

    protected:
      //Constructor is protected so only derived classes can call it
      exBase() throw() ;
      //Copy function for base class internals (called by derived)
      void Copy(const exBase & rh) throw();
	    
    private:
      //Copy constructor/ assignment operators are private and not implemented
      exBase(const exBase & rh) throw();
      exBase & operator=(const exBase & rh) throw();

      //Stack trace code
      void GenerateStackTrace() throw();
      void AppendStackLine(const char * line) throw(); //adds a eol char if possible

      //Description internals
      size_t descriptionSize;
      size_t descriptionUsed;
      char * descriptionBuffer;

      //Stack trace internals
      size_t stackSize;
      size_t stackUsed;
      char * stackBuffer;

      pid_t PID;
    };


//Derived ToolExceptions

ToolExceptionClass(ToolException1,"ToolException number 1\n")
ToolExceptionClass(ToolException2,"ToolException number 2\n")


}



#endif
