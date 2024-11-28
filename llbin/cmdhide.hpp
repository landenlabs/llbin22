//-------------------------------------------------------------------------------------------------
//
// File: cmdhide.hpp    Author: Dennis Lang   Desc: Scramble (obfuscate) file and/or directory names
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
#include "commands.hpp"

typedef unsigned short ushort;

class CmdHideBase : public Command {
public:
    DIR_TYPES dtype; //   IS_DIR_END
    const char* validChars;
    ushort vLen;
    ushort key;
    ushort start;

protected:
    static const int CHR_MAP_SIZE = 256;
    char  encMap[CHR_MAP_SIZE];
    char  decMap[CHR_MAP_SIZE];

    bool buildMap();
    void decChar(const lstring& inStr, lstring& outStr);

public:
    CmdHideBase(const char* name, DIR_TYPES _dtype) : Command(name), dtype(_dtype), key(11), start(0) {}

    virtual  bool begin(StringList& fileDirList) = 0;
    virtual size_t add( lstring& file, DIR_TYPES dtype) = 0;
    virtual bool end() { return true; }
};


// Hide directory names by obfuscating.
class CmdHide : public CmdHideBase {

public:
    CmdHide(DIR_TYPES dtype = IS_DIR_END) : CmdHideBase("hide", dtype) {}
    virtual  bool begin(StringList& fileDirList);
    virtual size_t add( lstring& file, DIR_TYPES dtype);

    bool makeHideName(const struct stat& info, const lstring& oldPath, lstring& newPath);
    void encChar(const lstring& inStr, lstring& outStr);
};


// Reverse CmdHide obfuscation.
class CmdUnhide : public CmdHideBase {

public:
    CmdUnhide(DIR_TYPES dtype = IS_DIR_END) : CmdHideBase("unhide", dtype) {}
    virtual  bool begin(StringList& fileDirList);
    virtual size_t add( lstring& file, DIR_TYPES dtype);


    bool makeUnHideName(const struct stat& info, const lstring& oldPath, lstring& newPath);

};
