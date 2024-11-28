//-------------------------------------------------------------------------------------------------
//
// File: cmdcrypt.hpp   Author: Dennis Lang   Desc: Encrypt files.
//
//-------------------------------------------------------------------------------------------------
//
// Author: Dennis Lang - 2021
// http://landenlabs.com
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

#include <set>

// Optional to enabled threaded encrypt/decriptions
#include "threadjob.hpp"

// Threads Enc   Dec
//   5      75    78
//   2      52    88
//   2      79    74
//   2      47    69
//   0      54    92

class CmdCryptBase : public Command {
protected:
    bool isEncrypt;

public:
    CmdCryptBase(const char* name) : Command(name) {}
    CmdCryptBase(const string& name) : Command(name) {}

    virtual bool begin(StringList& fileDirList);
    virtual size_t add( lstring& file, DIR_TYPES dtype);
    virtual bool allOfFile(const lstring& extn) = 0;
};


class CmdDecrypt : public CmdCryptBase {
public:
    CmdDecrypt() : CmdCryptBase("decrypt")  { isEncrypt = false; }

    // virtual size_t add( lstring& file, DIR_TYPES dtype);
    virtual bool end();
    virtual bool allOfFile(const lstring& extn);
    void doJob(const lstring& fullname, bool fullFile);
};


class CmdEncrypt : public CmdCryptBase {
    std::map<std::string, unsigned int> ignoredExtn;

public:
    CmdEncrypt() : CmdCryptBase("encrypt") { isEncrypt = true; }

    // virtual size_t add( lstring& file, DIR_TYPES dtype);
    virtual bool end();
    virtual bool allOfFile(const lstring& extn);
    void doJob(const lstring& fullname, bool fullFile);
};

