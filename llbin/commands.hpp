//-------------------------------------------------------------------------------------------------
//
// File: commands.hpp   Author: Dennis Lang   Desc: Process file scan
//
//-------------------------------------------------------------------------------------------------
//
// Author: Dennis Lang - 2021
// https://landenlabs.com
//
// This file is part of llbin project.
//
// ----- License ----
//
// Copyright (c) 2021  Dennis Lang
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once
#include "ll_stdhdr.hpp"

#include <vector>
#include <regex>
#include <iostream>
#include <map>

#include "directory.hpp"
#include "cmdutils.hpp"

// Helper types
typedef std::vector<lstring> StringList;

typedef unsigned int uint;
typedef std::vector<unsigned> IntList;
typedef char Byte;



//-------------------------------------------------------------------------------------------------
class Command {
public:
    // Runtime options
    PatternList includeFilePatList;
    PatternList excludeFilePatList;
    lstring keyStr;
    lstring extension = ".b22";

    bool showFile = false;
    bool verbose = false;
    bool dryRun = false;

    lstring separator = "\n";

    static volatile unsigned countDone;
    static volatile unsigned countError;
    static volatile bool abortFlag;

protected:
    lstring none;
    string my_name;

public:
    static void init();

    Command(const char* name) : my_name(name) {
    }
    Command(const string& name) : my_name(name) {
    }

    const string& name() const { return my_name; }

    virtual  bool begin(StringList& fileDirList)  {
        return fileDirList.size() > 0;
    }

    virtual size_t add( lstring& file, DIR_TYPES dtypes) = 0;

    // Call if threads used to complete the work normally done in add.
    virtual void doJob(const lstring& fullname, bool allOfFile) {
        std::cerr << "Thread job ignored for (e=encode/d=decode)[" << my_name << "] of " << fullname << std::endl;
    }

    virtual bool end() {
        return true;
    }

    Command& share(const Command& other) {
        includeFilePatList = other.includeFilePatList;
        excludeFilePatList = other.excludeFilePatList;
        keyStr = other.keyStr;
        extension = other.extension;

        showFile = other.showFile;
        verbose = other.verbose;
        dryRun = other.dryRun;

        separator = other.separator;
        return *this;
    }

};

//-------------------------------------------------------------------------------------------------
class BinNone : public Command {
public:
    BinNone() : Command("none") {}
    virtual size_t add( lstring& file, DIR_TYPES dtype);
};
