//-------------------------------------------------------------------------------------------------
//
// File: commands.cpp   Author: Dennis Lang   Desc: Process file scan
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

#include <iostream>

#include "commands.hpp"

volatile unsigned Command::countDone = 0;
volatile unsigned Command::countError = 0;
volatile bool Command::abortFlag = false;

void Command::init() {
}

#ifdef HAVE_WIN
//-------------------------------------------------------------------------------------------------
static size_t isWriteableFile(const struct stat& info) {
    size_t mask = _S_IFREG + _S_IWRITE;
    return ((info.st_mode & mask) == mask);
}
#else
//-------------------------------------------------------------------------------------------------
static size_t isWriteableFile(const struct stat& info) {
    size_t mask = S_IFREG + S_IWRITE;
    return ((info.st_mode & mask) == mask);
}
#endif

// ===========================================================================
// Locate matching files which are not in exclude list.
size_t BinNone::add( lstring& fullname, DIR_TYPES dtype) {
    size_t fileCount = 0;
    lstring name;
    CmdUtils::getName(name, fullname);

    if (dtype == IS_FILE
            && ! name.empty()
            && ! CmdUtils::FileMatches(fullname, excludePathPatList, false)
            && CmdUtils::FileMatches(fullname, includePathPatList, true)
            && ! CmdUtils::FileMatches(name, excludeFilePatList, false)
            && CmdUtils::FileMatches(name, includeFilePatList, true)) {
        fileCount++;

        struct stat info;
        if ( stat(fullname, &info) == 0 && isWriteableFile(info)) {
            if (showFile)
                std::cout << fullname.c_str() << std::endl;
        } else {
            if (showFile)
                std::cout << "ReadOnly " << fullname.c_str() << std::endl;
        }
    }

    return fileCount;
}
