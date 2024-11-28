//-------------------------------------------------------------------------------------------------
//
// File: cmdutils.hpp   Author: Dennis Lang   Desc: Command utility functions
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
#include "lstring.hpp"

#include <string>
#include <vector>
#include <regex>

typedef std::vector<std::regex> PatternList;

namespace CmdUtils {
const std::string quote(const std::string& str);

std::string GetErrorMsg(DWORD error);
const char* RunExtension(std::string& exeName);
bool RunCommand(const char* command, DWORD* pExitCode, int waitMsec);
// size_t isWriteableFile(const struct stat& info);

lstring& getName(lstring& outName, const lstring& inPath);
lstring& getDir(lstring& outDir, const lstring& inPath);
void getDirName(lstring& outDir, lstring& outName, const lstring& inPath);
void getPathParts(lstring& outDir, lstring& outName,  lstring& outExt, const lstring& inPath);
lstring& getExtn(lstring& outExt, const lstring& inPath);

lstring& removeExtn(lstring& outName, const lstring& inPath);
bool FileMatches(const lstring& inName, const PatternList& patternList, bool emptyResult);

size_t fileLength(const lstring& path);
bool deleteFile(const char* path);
}
