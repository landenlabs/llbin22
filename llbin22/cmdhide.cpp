//-------------------------------------------------------------------------------------------------
//
// File: cmdhide.cpp    Author: Dennis Lang   Desc: Scramble (obfuscate) file and/or directory names
//
//-------------------------------------------------------------------------------------------------
//
// Author: Dennis Lang - 2021
// https://landenlabs.com
//
// This file is part of llbin22 project.
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

#include "cmdhide.hpp"

#include <iostream>


// Blacklist of filename characters <>:"/\|?*
const char VALID_CHARS[] = "abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ-0123456789 ~`!@#$%^&()+={}[];',";
ushort hits[sizeof(VALID_CHARS)];
char scrambled[sizeof(VALID_CHARS)];

const char HIDE_PREFIX[] = "#_";

//-------------------------------------------------------------------------------------------------
inline unsigned short next(ushort num, ushort off, ushort len) {
    return (num + off) % len;
}

//-------------------------------------------------------------------------------------------------
static const int NO_POS = -1;
inline int stridx(const char* str, char findC) {
    const char* pos = strchr(str, findC);
    return (pos != NULL) ? (int)(pos - str) : NO_POS;
}

//-------------------------------------------------------------------------------------------------
bool CmdHideBase::buildMap() {

    validChars = VALID_CHARS;
    vLen = (ushort) strlen(validChars);

    if (keyStr.length() > 0) {
        ushort keyNum = atoi(keyStr.c_str());
        if (keyNum > 0)
            key = keyNum;
    }

    if ((vLen % key) == 0) {
        std::cerr << "Invalid key " << key << " must not be even divider of " << vLen << std::endl;
        std::cerr << "Use -key=<primeNumber> to set alternate key, such as -key=13\n";
        return false;
    }

    ushort num = next(start, key, vLen);
    ushort idx = 0;
    ushort errCnt = 0;
    ushort prev = start;

    do {
        if (hits[num]++ != 0) {
            errCnt++;
        }
        scrambled[idx++] = validChars[prev];
        prev = num;
        // std::cout << idx << ": " << num  << std::endl;
        num = next(num, key, vLen);
    } while (num != start);
    scrambled[idx++] = validChars[prev];

    if (errCnt != 0 || idx != vLen) {
        errCnt++;
        std::cerr << "Error mapping for key " << key << std::endl;
        for (ushort idx = 0; idx < vLen; idx++) {
            if (hits[idx] != 1)
                std::cout << idx << ": " << hits[idx] << std::endl;
        }
    } else {
        // std::cout << "No errors for key " << off << std::endl;
    }

    std::cout << " In=[" << validChars << "]\n";
    std::cout << "Out=[" << scrambled << "]\n";
    if (strlen(validChars) != strlen(scrambled)) errCnt++;
    std::cout << "validLen=" << vLen << " key=" << key << " start=" << start << " errCnt=" << errCnt << std::endl;

    for (unsigned short c = 0x00; c < CHR_MAP_SIZE; c++) {
        int pos = stridx(validChars, c);
        if (pos != NO_POS) {
            unsigned char d = (unsigned char)scrambled[pos];
            encMap[c] = d;
            decMap[d] = c;
        } else {
            encMap[c] = c;
            decMap[c] = c;
        }
    }

    return (errCnt == 0);
}

//-------------------------------------------------------------------------------------------------
void CmdHideBase::decChar(const lstring& inStr, lstring& outStr) {

    size_t inLen = inStr.size();
    outStr.resize(inLen);

    for (unsigned idx = 0; idx < inLen; idx++) {
        char c = inStr[idx];
        c = decMap[(unsigned)c];
        const char* pos = strchr(validChars, c);
        if (pos != NULL) {
            unsigned vIdx = (unsigned)(pos - validChars);
            c = validChars[(vIdx - idx) % vLen];
        }
        outStr[idx] = c;
    }
}


// ===========================================================================
bool CmdHide::begin(StringList& fileDirList) {
    return buildMap();
}

//-------------------------------------------------------------------------------------------------
// Locate matching files which are not in exclude list.
// Locate duplcate files.
size_t CmdHide::add( lstring& fullname, DIR_TYPES _dtype) {
    size_t fileCount = 0;
    lstring name;
    DirUtil::getName(name, fullname);
    lstring oldPath = fullname;
    struct stat info;

    if (_dtype == dtype
            && ! name.empty()
            && ! CmdUtils::FileMatches(fullname, excludePathPatList, false)
            && CmdUtils::FileMatches(fullname, includePathPatList, true)
            && ! CmdUtils::FileMatches(name, excludeFilePatList, false)
            && CmdUtils::FileMatches(name, includeFilePatList, true)
            && name.find(HIDE_PREFIX) != 0
    && stat(fullname, &info) == 0
    ) {
        lstring newPath;
        if (makeHideName(info, oldPath, newPath)) {
            fileCount++;

            if (dryRun || rename(oldPath, newPath) == 0) {
                if (showFile) {
                    std::cout << "From: " << oldPath << std::endl
                        << "  to: " << newPath << std::endl;
                }
            } else {
                // int error = errno;
                std::cerr << "Failed Hide " << oldPath << " to " << newPath << std::endl;
            }
        } else {
            std::cerr << "Ignored Hide of " << oldPath << std::endl;
        }
    }

    return fileCount;
}

//-------------------------------------------------------------------------------------------------
bool CmdHide::makeHideName(const struct stat& info, const lstring& oldPath, lstring& newPath) {
    lstring oldDir, oldName, oldExtn, newName;
    CmdUtils::getPathParts(oldDir, oldName, oldExtn, oldPath);
    encChar(oldName, newName);
    newPath = oldDir + HIDE_PREFIX + newName + oldExtn;

    lstring tstName;
    decChar(newName, tstName);

    if (tstName != oldName) {
        std::cerr << "Obfuscate failed from=" << oldName << " enc=" << newName << " dec=" << tstName << std::endl;
        return false;
    } else {
        return true;
    }
}

//-------------------------------------------------------------------------------------------------
// TODO - alternate approach is to use a validChars array which does not include all the
// characters and xor the offset instead of offset.
void CmdHide::encChar(const lstring& inStr, lstring& outStr) {

    size_t inLen = inStr.size();
    outStr.resize(inLen);

    for (unsigned idx = 0; idx < inLen; idx++) {
        char c = inStr[idx];
        const char* pos = strchr(validChars, c);
        if (pos != NULL) {
            unsigned vIdx = (unsigned)(pos - validChars);
            c = validChars[(vIdx + idx) % vLen];
        }
        outStr[idx] = encMap[(unsigned)c];
    }
}

// ===========================================================================
bool CmdUnhide::begin(StringList& fileDirList) {
    return buildMap();
}

//-------------------------------------------------------------------------------------------------
// Locate matching files which are not in exclude list.
// Locate duplcate files.
size_t CmdUnhide::add(lstring& fullname, DIR_TYPES _dtype) {
    size_t fileCount = 0;
    lstring name;
    DirUtil::getName(name, fullname);
    lstring oldPath = fullname;
    struct stat info;

    if (_dtype == dtype
            && ! name.empty()
            && ! CmdUtils::FileMatches(fullname, excludePathPatList, false)
            && CmdUtils::FileMatches(fullname, includePathPatList, true)
            && ! CmdUtils::FileMatches(name, excludeFilePatList, false)
            && CmdUtils::FileMatches(name, includeFilePatList, true)
    && name.find(HIDE_PREFIX) == 0
    && stat(fullname, &info) == 0
    ) {
        lstring newPath;
        if (makeUnHideName(info, oldPath, newPath)) {
            fileCount++;

            if (dryRun || rename(oldPath, newPath) == 0) {
                if (showFile) {
                    std::cout << "From: " << oldPath << std::endl
                        << "  to: " << newPath << std::endl;
                }
            }  else {
                // int error = errno;
                std::cerr << "Ignored Unhide of " << fullname << std::endl;
            }
        }
    }

    return fileCount;
}

//-------------------------------------------------------------------------------------------------
bool CmdUnhide::makeUnHideName(const struct stat& info, const lstring& oldPath, lstring& newPath) {
    lstring oldDir, oldName, oldExtn, newName;
    CmdUtils::getPathParts(oldDir, oldName, oldExtn, oldPath);
    oldName.erase(0, sizeof(HIDE_PREFIX) -1);
    decChar(oldName, newName);
    newPath = oldDir + newName + oldExtn;
    return true;
}

