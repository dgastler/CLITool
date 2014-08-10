#Copyright (C) 2014 Daniel Gastler

#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:

#The above copyright notice and this permission notice shall be included in
#all copies or substantial portions of the Software.

#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
#THE SOFTWARE.


#############################################################
#Compiling stuff
#############################################################
O_LEVEL := -O0   #Levels above O1 inline all of our functions and make the stack trace boring

CXXFLAGS := $(O_LEVEL) -Wall -fPIC -ggdb -fno-strict-aliasing -pedantic -Werror
CXXFLAGS += -rdynamic #for stack trace
LDFLAGS := -Wall -ggdb $(O_LEVEL) -Werror -pedantic
LDFLAGS += -rdynamic #for stack trace
LIBRARY_FLAGS := -shared $(LDFLAGS)

BUILD_DIR := build
BIN_DIR := bin
INCLUDE_DIR := include
SOURCE_DIR := src

SOURCE_SUF := .cc
HEADER_SUF := .hh
OBJECT_SUF := .o
DEPEND_SUF := .d

############################################################
#dependancy handling
#############################################################
DEP_DIR=.dep

#helper functions
getHeaders = $(foreach file,$(wildcard $(1)/*$(HEADER_SUF)),$(patsubst $(SOURCE_DIR)/	%,$(INCLUDE_DIR)/%,$(file))) # $1 = path
getSources = $(wildcard $(1)/*$(SOURCE_SUF)) # $1 = path
getObjects = $(foreach file,$(1),$(patsubst src/%$(SOURCE_SUF),$(BUILD_DIR)/%$(OBJECT_SUF),$(file)))   # $1= sources
getDependencies = $(foreach file,$(1),$(patsubst src/%$(SOURCE_SUF),$(DEP_DIR)/%$(DEPEND_SUF),$(file))) # $1 = sources









#############################################################
#Exception Library
#############################################################
TOOL_EXCEPTION_LIB=lib/libToolException.so
TOOL_EXCEPTION_LIB_DIR=src/ToolException

#Source file base for files in the library
TOOL_EXCEPTION_LIB_H=$(call getHeaders,$(TOOL_EXCEPTION_LIB_DIR))
TOOL_EXCEPTION_LIB_S=$(call getSources,$(TOOL_EXCEPTION_LIB_DIR))
TOOL_EXCEPTION_LIB_O=$(call getObjects,$(TOOL_EXCEPTION_LIB_S))
TOOL_EXCEPTION_LIB_D=$(call getDependencies,$(TOOL_EXCEPTION_LIB_S))

CXXFLAGS += -I$(SOURCE_DIR)

#############################################################
#Test program
#############################################################
TOOL=$(BIN_DIR)/Tool
TOOL_DIR=$(SOURCE_DIR)

TOOL_H=$(call getHeaders,$(TOOL_DIR))
TOOL_S=$(call getSources,$(TOOL_DIR))
TOOL_O=$(call getObjects,$(TOOL_S))
TOOL_D=$(call getDependencies,$(TOOL_S))

LIBRARIES += -lcurses -lreadline










#############################################################
#Builds
#############################################################

all:  $(TOOL) $(TOOL_EXCEPTION_LIB)

clean :
	@rm -rf $(BUILD_DIR)/* >/dev/null
	@rm -rf lib/* >/dev/null
	@rm -rf $(TOOL)
	@rm -rf $(INCLUDE_DIR)/* > /dev/null

distclean : clean
	@rm -rf $(DEP_DIR) > /dev/null


#########################################
#Dependancy builds
#########################################
$(DEP_DIR)/%$(DEPEND_SUF) : ./src/%$(SOURCE_SUF)
	@mkdir -p $(@D) > /dev/null
	@echo "Building dependency list for $^"
	@$(CXX) $(CXXFLAGS)  -M -MG $^ -MT $(BUILD_DIR)/$*$(OBJECT_SUF) -MF $(@).tmp > /dev/null
	@awk '{print $0}END{print "	@mkdir -p $$(@D)"; print "	$(CXX) $$(CXXFLAGS) -c -o $$@ $(SOURCE_DIR)/$*$(SOURCE_SUF)"}' $@.tmp > $@
	@rm $@.tmp > /dev/null

-include $(TOOL_EXCEPTION_LIB_D) $(TOOL_D)


#########################################
#MYEXCEPTION Library
#########################################
$(TOOL_EXCEPTION_LIB) : $(TOOL_EXCEPTION_LIB_D) $(TOOL_EXCEPTION_LIB_O)
	@$(foreach file,$(TOOL_EXCEPTION_LIB_H), mkdir -p $(patsubst src%,$(INCLUDE_DIR)%,$(dir $(file))))   #make dirs for includes
	@$(foreach file,$(TOOL_EXCEPTION_LIB_H), ln -s $(file) $(patsubst src%,$(INCLUDE_DIR)%,$(file)))	   #symbolic links for include
	$(CXX) $(LIBRARY_FLAGS) $(TOOL_EXCEPTION_LIB_O) -o $@



#########################################
#test
#########################################
$(TOOL) :  $(TOOL_EXCEPTION_LIB) $(TOOL_D) $(TOOL_O)
	$(CXX) $(LDFLAGS) $(LIBRARY_PATH) -o $@ $(TOOL_O) $(TOOL_EXCEPTION_LIB_O) $(LIBRARIES)


