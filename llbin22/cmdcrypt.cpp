//-------------------------------------------------------------------------------------------------
//
// File: cmdcrypt.cpp   Author: Dennis Lang   Desc: Encrypt files
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

#include "ll_stdhdr.hpp"
#include "cmdcrypt.hpp"
#include "directory.hpp"

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


static const unsigned BLOCK_SIZE = 1024;
static std::map< lstring, lstring> EXT_ABBR;
static std::map< lstring, lstring> ABBR_EXT;
static const char* STD_EXT[] = {
    "png", "jpeg", "jpg", "gif", "webp", "swf", "svg",
    "mp4", "avi", "flv", "wmv", "webm", "wav",
    "txt", "htm", "html", "url",
    "zip", "rar", "pdf",
    "csv", "xlsx", "xls", "doc", "docx",
    "pdn",
    "c", "cpp", "h", "hpp", "csh", "bat",
    "xml", "json"
};

static std::set< lstring> EXT_FULL_SET;
static std::set< lstring> DEC_FULL_SET;
static const char* FULL_EXT[] = {
    "txt", "htm", "html", "url",
    "csv", "xlsx", "docx",
    "c", "cpp", "h", "hpp", "csh", "bat",
    "xml", "json"
};

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


//-------------------------------------------------------------------------------------------------
bool CmdCryptBase::begin(StringList& fileDirList) {

    for (auto ext : FULL_EXT) {
        EXT_FULL_SET.insert(ext);
    }

    // static std::map<const char*, const char*> EXT_ABBR;
    // static std::map<const char*, const char*> ABBR_EXT;

    lstring empty = "";
    for (auto ext : STD_EXT) {
        lstring abbr = ext;
        abbr.insert(0, abbr.substr(abbr.length() - 1));
        abbr.append("01");
        abbr.erase(3);
        abbr[2]++;

        if (ABBR_EXT[abbr].length() != 0) {
            abbr = ext;
            abbr.insert(0, abbr.substr(abbr.length() - 1));
            abbr.append("0123");
            abbr.erase(4);
            abbr[3]++;
            if (ABBR_EXT[abbr].length() != 0) {
                std::cerr << "Warning dup abbr " << abbr << " " << ext << " and " << ABBR_EXT[abbr] << std::endl;
            }
        }

        if (verbose) {
            std::cerr << "Ext=" << ext << " =>" << abbr << std::endl;
        }
        EXT_ABBR[ext] = abbr;
        ABBR_EXT[abbr] = ext;

        if (EXT_FULL_SET.find(ext) != EXT_FULL_SET.end()) {
            DEC_FULL_SET.insert(abbr);
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
size_t CmdCryptBase::add(lstring& fullname, DIR_TYPES dtype) {
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
        if (showFile)
            std::cout << fullname.c_str() << std::endl;

        struct stat info;
        if ((fullname.find(extension) == std::string::npos) == isEncrypt
        && stat(fullname, &info) == 0) {

            if (isWriteableFile(info)) {
                bool doAll = allOfFile(name);
                // std::cerr << " code=" << code << " " << name << " all=" << doAll << std::endl;
#ifdef USE_THREAD
                ThreadJob::StartThread(*this, fullname, doAll);
#else
                doJob(fullname, doAll);
#endif
                countDone++;
            } else {
                std::cerr <<  my_name << " not writable " << fullname << std::endl;
                countError++;
            }
        } else if (verbose) {
            std::cerr << "Ignored=" << fullname << std::endl;
        }
    }

    return fileCount;
}

//-------------------------------------------------------------------------------------------------
static bool makeEncryptName(const lstring& ext, lstring& out, const lstring& in) {
    lstring extn;
    CmdUtils::getExtn(extn, in);
    extn = extn.toLower();
    if (EXT_ABBR.find(extn) != EXT_ABBR.end()) {
        CmdUtils::removeExtn(out, in);
        out += ".";
        out += EXT_ABBR[extn];
        out += ext;
        return true;
    }
    return false;
}

//-------------------------------------------------------------------------------------------------
static bool makeDecryptName(lstring& out, const lstring& in) {
    lstring extn;
    CmdUtils:: getExtn(extn, CmdUtils::removeExtn(out, in));
    extn = extn.toLower();
    if (ABBR_EXT.find(extn) != ABBR_EXT.end()) {
        CmdUtils::removeExtn(out, out);
        out += ".";
        out += ABBR_EXT[extn];
        return true;
    }
    return false;
}

const Byte MASK1 = 0xB6; //     1011 0110 = B6
const Byte MASK2 = 0xD9; //     1101 1001 = D9
const size_t MASK1_END = 256;


//-------------------------------------------------------------------------------------------------
static void  maskBlock(Byte* block, size_t gotLen) {
    // Cannot get xcode to build with Byte typedef'd to unsigned char
    unsigned char* ublock = (unsigned char*)block;
    unsigned idx = 0;
    while (idx < gotLen && idx < MASK1_END) {
        ublock[idx++] ^= MASK1;
    }
    while (idx < gotLen) {
        ublock[idx++] ^= MASK2;
    }
}

// ================================================================================================
//-------------------------------------------------------------------------------------------------
void CmdDecrypt::doJob(const lstring& fullname, bool allOfFile)  {
    Byte block[BLOCK_SIZE];
    lstring newname;
    if (makeDecryptName(newname, fullname)) {
        size_t fileLen = CmdUtils::fileLength(fullname);
        std::basic_fstream<Byte> fileStream(fullname, ios::out | ios::in | ios::binary);

        size_t offset = 0;
        size_t gotLen = 0;
        do {
            fileStream.seekg(offset);
            gotLen = fileStream.read(block, BLOCK_SIZE).gcount();
            if (gotLen > 0 && gotLen <= BLOCK_SIZE) {
                maskBlock(block, gotLen);
                fileStream.clear();
                fileStream.seekp(offset);
                if (! dryRun) {
                    fileStream.write(block, gotLen);
                }
                offset += gotLen;
                // std::cerr<< " dec " << fullname << " at=" << offset << " len=" << gotLen << std::endl;
            }
        } while (gotLen > 0 && allOfFile && offset < fileLen);
        bool doRename = ! fileStream.bad() && ! dryRun;
        fileStream.close();
        if (doRename) {
            rename(fullname, newname);
        } else if (! dryRun) {
            countError++;
            std::cerr << my_name << " failed on " << fullname << std::endl;
        }
    }
}

//-------------------------------------------------------------------------------------------------
bool CmdDecrypt::end() {
#ifdef USE_THREAD
    ThreadJob::EndThreads();
#endif
    return true;
}

bool CmdDecrypt::allOfFile(const lstring& name) {
    lstring extn,  name2;
    CmdUtils::getExtn(extn, CmdUtils::removeExtn(name2, name));    // remove b22 ext first
    // std::cerr << "extn=" << extn << "\n";
    // for (lstring const& ext : DEC_FULL_SET) std::cerr << ext << ' ';
    // std::cerr << std::endl;

    return DEC_FULL_SET.find(extn) != DEC_FULL_SET.end();
}

// ================================================================================================
//-------------------------------------------------------------------------------------------------
void CmdEncrypt::doJob(const lstring& fullname, bool allOfFile)  {
    Byte block[BLOCK_SIZE];
    lstring newname;

    if (makeEncryptName(extension, newname, fullname)) {
        if (! dryRun) {
            size_t fileLen = CmdUtils::fileLength(fullname);
            std::basic_fstream<Byte> fileStream(fullname, ios::out | ios::in | ios::binary);

            size_t offset = 0;
            size_t gotLen = 0;
            do {
                fileStream.seekg(offset);
                gotLen = fileStream.read(block, BLOCK_SIZE).gcount();
                if (fileStream.bad() || gotLen == 0) {
                    std::cerr << errno << " readlen=" << gotLen << " " << fullname << std::endl;
                    // std::cerr << "FileSize=" << info.st_size  << " mode=" << info.st_mode << std::endl;
                    break;
                }
                if (gotLen > 0 && gotLen <= BLOCK_SIZE) {
                    maskBlock(block, gotLen);
                    fileStream.clear();
                    fileStream.seekp(offset);
                    fileStream.write(block, gotLen);
                }
                // std::cerr<< " enc " << fullname << " at=" << offset << " len=" << gotLen << std::endl;
                offset += gotLen;
            } while (gotLen > 0 && allOfFile && offset < fileLen);
            bool doRename = ! fileStream.bad();
            fileStream.close();
            if (doRename) {
                rename(fullname, newname);
            } else {
                countError++;
                std::cerr << my_name << " failed on " << fullname << std::endl;
            }
        }
    } else {
        lstring extn;
        CmdUtils::getExtn(extn, fullname);
        extn = extn.toLower();
        if (verbose)
            std::cerr << "Ignored extension=" << extn << " of " << fullname << std::endl;
        ignoredExtn[extn] = ignoredExtn[extn] + 1;
    }
}

//-------------------------------------------------------------------------------------------------
bool CmdEncrypt::end() {
#ifdef USE_THREAD
    ThreadJob::EndThreads();
#endif
    if (! ignoredExtn.empty()) {
        std::cout << "File extensions not encrypted (not supported):\n";
        std::cout << "Count Extension\n";
        for (auto iter = ignoredExtn.begin(); iter != ignoredExtn.end(); iter++) {
            std::cout << setw(5) << iter->second << " " << iter->first << std::endl;
        }
    }
    return true;
}


bool CmdEncrypt::allOfFile(const lstring& name) {
    lstring extn;
    return EXT_FULL_SET.find(CmdUtils::getExtn(extn, name)) != EXT_FULL_SET.end();
}
