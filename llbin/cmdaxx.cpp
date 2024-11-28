//-------------------------------------------------------------------------------------------------
//
// File: cmdaxx.cpp      C U R R R E N T L Y   N O T   U S E D
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

// ==========================================
// |  C U R R R E N T L Y   N O T   U S E D |
// ==========================================

#include "cmdaxx.hpp"

/*
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <ctype.h>
#include <vector>
#include <map>
#include <algorithm>
#include <regex>
#include <exception>
#include <errno.h>

#include "ll_stdhdr.hpp"
#include "commands.hpp"
#include "directory.hpp"
#include "split.hpp"
#include "cmdutils.hpp"


const lstring DECRYPT_EXE = "C:\\Program Files\\Axantum\\AxCrypt\\AxCrypt.exe";
const lstring AXX = ".axx";
const lstring EMPTY = "";

// TODO -  setup so some file types get full file encoding, others only partial.
class Binfo {
public:
    const char* extn;
    size_t startPos = 0;
    size_t endPos = -1;
};




//-------------------------------------------------------------------------------------------------
class Decrypt {
public:
    const lstring cmd;
    lstring path;
    lstring extn;
    lstring outFile;
    struct stat pathStat;
    struct stat outStat;

    Decrypt(const lstring& CMD) : cmd(CMD) {}

    bool decryptFile(const lstring& fullname, const lstring& outPrefix);
};

//-------------------------------------------------------------------------------------------------
bool Decrypt::decryptFile(const lstring& fullname, const lstring& outPrefix) {

    path = lstring(fullname);
    ReplaceAll(path, AXX, EMPTY);
    size_t pos = path.find_last_of("-");
    if (pos + 3 <= path.length()) {
        path[pos] = '.';
        extn = path + pos;
        int statErr = stat(fullname, &pathStat);

        if (statErr == 0) {
            outFile = outPrefix + extn;
            lstring fullCmd = cmd + CmdUtils::quote(outFile) + " " + CmdUtils::quote(fullname);
            DWORD exitCode = 0;

            CmdUtils::deleteFile(outFile);
            if (CmdUtils::RunCommand(fullCmd, &exitCode, 2000)) {
                if (exitCode == 0) {
                    int statErr = stat(outFile, &outStat);
                    return (statErr == 0);
                }
            }
        }
    }
    return false;
}

*/
