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

//Error string for bad allocation of description
static const char descriptionError[] = "Description allocation failed.\n";

exBase::exBase() throw() : descriptionSize(255),
			   stackSize(1024), 
			   PID(-1)
{
  //Allocate memory (throw if failed)
  stackBuffer = (char *) malloc(stackSize+1);  
  descriptionBuffer = (char *) malloc(descriptionSize+1);

  descriptionUsed = 0;
  stackUsed = 0;

  //Null terminate the strings to empty
  //also make the char after the nominal end of string null terminated
  if(descriptionBuffer != NULL)
    {
      descriptionBuffer[0] = '\0';
      descriptionBuffer[descriptionSize] = '\0';
    }
  else
    {
      descriptionSize = 0;
    }
  if(stackBuffer != NULL)
    {
      stackBuffer[0] = '\0';
      stackBuffer[stackSize] = '\0';
    }
  else
    {
      stackSize=0;
    }

  GenerateStackTrace();
}

exBase::~exBase() throw()
{
  if(stackBuffer != NULL)
    free(stackBuffer);
  if(descriptionBuffer != NULL)
    free(descriptionBuffer);
}

void exBase::Copy(const exBase & rh) throw()
{
  //Copy description buffer
  this->descriptionUsed = 0;
  this->descriptionSize = rh.descriptionSize;
  //Check if the allocation had worked for rh
  if(rh.descriptionBuffer == NULL)
    {
      //It didn't work, make sure our copy is also NULL
      this->descriptionBuffer = NULL;
    }
  else
    {
      //rh has a valid description, build ours
      this->descriptionBuffer = (char *) malloc(descriptionSize+1);
      if(this->descriptionBuffer != NULL)
	{
	  memcpy(this->descriptionBuffer,rh.descriptionBuffer,descriptionSize);
	  this->descriptionUsed = rh.descriptionUsed;
	}
    }

  //Copy stack trace buffer
  this->stackUsed = 0;
  this->stackSize = rh.stackSize;
  //Check if the allocation worked for rh
  if(rh.stackBuffer == NULL)
    {
      //It didn't work, make sure our copy is also NULL
      this->stackBuffer = NULL;      
    }
  else
    {//rh was valid, so construct ours
      this->stackBuffer = (char *) malloc(stackSize+1);
      if(this->stackBuffer != NULL)
	{
	  memcpy(this->stackBuffer,rh.stackBuffer,stackSize);
	  this->stackUsed = rh.stackUsed;
	}
    }

  //Copy PID
  this->PID = rh.PID;
}

void exBase::Append(const char * buffer) throw()
{
  //Compute the appended buffer's size
  size_t appendedSize = strlen(buffer);

  //Make sure we stay in the bounds of our buffer
  if( appendedSize > (descriptionSize - descriptionUsed))
    {
      appendedSize = (descriptionSize - descriptionUsed);
    }

  //Append the new string (n keeps us safe from buffer overflow)
  strncpy(descriptionBuffer+descriptionUsed,
	  buffer,
	  appendedSize);

  //update the description size
  descriptionUsed += appendedSize;

  //enforce the null terminator 
  //(the buffer is one larger than the size so we always have space for a NULL)
  //This is untrue if malloc failed initially, so we watch out for that. 
  if(descriptionBuffer != NULL)
    descriptionBuffer[descriptionUsed] = '\0';
}

const char * exBase::Description() const throw() 
{
  if(descriptionBuffer == NULL) //If there was a bad alloc, return an error string
    return descriptionError;
  return descriptionBuffer;
}
