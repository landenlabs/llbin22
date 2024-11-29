//-------------------------------------------------------------------------------------------------
//
//  llbin      may-2021       Dennis Lang
//
//  Manipulate binary files (encrypt/decript)
//
//  TODO -
//   1. Add option to output hash / md5 per file (just first block and/or entire file)
//   2. Encrpt/Decrpt - expand extension mapping to define how many blocks to hash,
//      for text files (.txt, .cpp, .hpp, .c, .h) etc  hash entire file,
//      for zip do front and back.
//-------------------------------------------------------------------------------------------------
//
// Author: Dennis Lang - 2021
// https://landenlabs.com/
//
// This file is part of llbin project.
//
// ----- TODO -----
//   Add -force to overwrite readonly
//   Add -any to encode any file extension
//   Add class for each extension, to determine length of encoding
//   Add rename only
//   Add option to change mask/encoding
//   Add password to decode
//
// ----- License ----
//
// Copyright (c) 2021 Dennis Lang
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

// 4291 - No matching operator delete found
#pragma warning( disable : 4291 )
#define _CRT_SECURE_NO_WARNINGS

// Project files
#include "ll_stdhdr.hpp"
#include "commands.hpp"
#include "cmdcrypt.hpp"
#include "cmdhide.hpp"
#include "cmdutils.hpp"
#include "parseutil.hpp"
#include "directory.hpp"

#include <stdio.h>
#include <ctype.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <map>
#include <algorithm>
#include <regex>
#include <exception>


using namespace std;

//-------------------------------------------------------------------------------------------------
// Recurse over directories, locate files.
static size_t InspectFiles(Command& command,  lstring& dirname, unsigned depth) {
    static std::vector<size_t> counts(10, 0);

    if (Command::abortFlag) {
        return 0;
    }

    Directory_files directory(dirname);
    lstring fullname;

    size_t fileCount = 0;

    struct stat filestat;
    try {
        if (stat(dirname, &filestat) == 0 && S_ISREG(filestat.st_mode)) {
            fileCount += command.add(dirname, IS_FILE);
            return fileCount;
        }
    } catch (exception ex) {
        // Probably a pattern, let directory scan do its magic.
    }

    while (! Command::abortFlag && directory.more())  {
        directory.fullName(fullname);

        if (directory.is_directory()) {
            counts[depth + 1] = 0;
            fileCount += command.add(fullname, IS_DIR_BEG); // add directory, fullname may change
            fileCount += InspectFiles(command, fullname, depth + 1);
            fileCount += command.add(fullname, IS_DIR_END); // add directory.
        } else if (fullname.length() > 0) {
            fileCount += command.add(fullname, IS_FILE);
        }

        if (fileCount >= counts[depth] + 10) {
            counts[depth] = fileCount;
            std::cerr << "\r ";
            for (unsigned idx = 0; idx <= depth; idx++)
                std::cerr << counts[idx] << " ";
            std::cerr << " " << dirname << "      ";
        }
    }

    return fileCount;
}

//-------------------------------------------------------------------------------------------------
void showHelp(char* name) {
    const char* helpMsg =  "  Dennis Lang v2.3 (landenlabs.com) " __DATE__   "\n"
        "\nDes: 'Manipulate Binary Files\n"
        "Use: llbin [options] directories...   or  files\n"
        "\n"
        " Options (only first unique characters required, options can be repeated): \n"
        "\n"
        "   -_y_encrypt                   ; Encrypt and rename file foo.ext to foo-e.b22 \n"
        "   -_y_decrypt                   ; Decrypt and rename file foo-e.b22 foo.ext \n"
        "\n"
        "   -_y_hideDir                   ; Hide (obfuscate) directory names \n"
        "   -_y_unhideDir                 ; Reverse obfuscated directory names \n"
        "   -_y_hideFile                  ; Hide (obfuscate) file names \n"
        "   -_y_unhideFile                ; Reverse obfuscated file names \n"
        "\n"
        "   -_y_includeFile=<filePattern> ; Include files by regex match \n"
        "   -_y_excludeFile=<filePattern> ; Exclude files by regex match \n"
        "   -_y_IncludePath=<pathPattern> ; Include path by regex match \n"
        "   -_y_ExcludePath=<pathPattern> ; Exclude path by regex match \n"
        "   -_y_verbose \n"
        "   -_y_norun \n"
        "\n"
        " Optional:\n"
        // Not implemented yet.
        //     "   -_y_key=<crypt_key>       ; Use with -_y_encrypt, -_y_decrypt \n"
        "   -_y_ext=<extension>       ; Use with -_y_encrypt to set file extension \n"
        "\n"
        " Example: \n"
        "   llbin -_y_inc=a*.png -_y_inc=*.jpg -_y_ex=foo.jpg -_y_key=123456 -_y_encrypt dir1/subdir dir2 \n"
        "   llbin -_y_inc=a*.aax -_y_inc=b*.aax -_y_ex=foo.* -_y_encrypt dir1/subdir dir2/subdir dir3/subdir \n"
        "   llbin -_y_inc=*b22 -_y_decryp dir1/subdir dir2/subdir dir3/subdir \n"
        "\n"
        "\n";
    std::cerr << Colors::colorize("\n_W_") << name << Colors::colorize(helpMsg);
}

//-------------------------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    ParseUtil   parser;
    CmdEncrypt  encryptFile;
    CmdDecrypt  decryptFile;

    CmdHide     hideDir(IS_DIR_END);
    CmdUnhide   unhideDir(IS_DIR_END);
    CmdHide     hideFile(IS_FILE);
    CmdUnhide   unhideFile(IS_FILE);

    BinNone     doNothing;

    Command*    commandPtr = &doNothing;
    StringList  fileDirList;


    if (argc == 1)  {
        showHelp(argv[0]);
    } else {
        Command::init();

        bool doParseCmds = true;
        string endCmds = "--";
        for (int argn = 1; argn < argc; argn++) {
            if (*argv[argn] == '-' && doParseCmds) {
                lstring argStr(argv[argn]);
                Split cmdValue(argStr, "=", 2);
                if (cmdValue.size() == 2) {
                    lstring cmd = cmdValue[0];
                    lstring value = cmdValue[1];
                    
                    if (cmd.length() > 1 && cmd[0] == '-')
                        cmd.erase(0);   // allow -- prefix on commands
                    
                    const char* cmdName = cmd + 1;
                    switch (cmd[(unsigned)1]) {
                    case 's':   // -separator="blah"
                        if (parser.validOption("separator", cmdName)) {
                            commandPtr->separator = ParseUtil::convertSpecialChar(value);
                        }
                        break;
                    case 'e':   // -excludeFile=<patFile>
                        if (parser.validOption("extension", cmdName, false))  {
                            commandPtr->extension = value;
                        }  else  {
                            parser.validPattern(commandPtr->excludeFilePatList, value, "excludeFile", cmdName);
                        }
                        break;
                    case 'E':   // -ExcludeDir=<patPath>
                        parser.validPattern(commandPtr->excludePathPatList, value, "ExcludeDir", cmdName);
                        break;
                    case 'i':   // -includeFile=<patFile>
                        parser.validPattern(commandPtr->includeFilePatList, value, "includeFile", cmdName);
                        break;
                    case 'I':   // -IncludeDir=<patPath>
                        parser.validPattern(commandPtr->includePathPatList, value, "includeDir", cmdName);
                        break;
                    case 'k':   // decrypt key
                        if (parser.validOption("key", cmdName))  {
                            commandPtr->keyStr = value;
                        }
                        break;
                    default:
                        parser.showUnknown(argStr);
                        break;
                    }
                } else {
                    const char* cmdName = argStr + 1;
                    switch (*cmdName) {
                    case 'd': //
                        if (parser.validOption("decrypt", cmdName)) {
                            commandPtr = &decryptFile.share(*commandPtr);
                        }
                        break;
                    case 'e': //
                        if (parser.validOption("encrypt", cmdName)) {
                            commandPtr = &encryptFile.share(*commandPtr);
                        }
                        break;
                    case 'h':
                        if (parser.validOption("help", cmdName, false)) {
                            showHelp(argv[0]);
                        } else  if (parser.validOption("hidedir", cmdName, false)) {
                            commandPtr = &hideDir.share(*commandPtr);
                        } else  if (parser.validOption("hidefile", cmdName)) {
                            commandPtr = &hideFile.share(*commandPtr);
                        }
                        break;
                    case 'n':
                        if (parser.validOption("norun", cmdName)) {
                            commandPtr->dryRun = true;
                        }
                        break;
                    case 'u':
                        if (parser.validOption("unhidedir", cmdName, false)) {
                            commandPtr = &unhideDir.share(*commandPtr);
                        } else  if (parser.validOption("unhidefile", cmdName)) {
                            commandPtr = &unhideFile.share(*commandPtr);
                        }
                        break;
                    case 'v':
                        commandPtr->verbose = true;
                        commandPtr->showFile = true;
                        break;
                    case '?':
                        showHelp(argv[0]);
                        break;
                    default:
                        parser.showUnknown(argStr);
                    }

                    if (endCmds == argv[argn]) {
                        doParseCmds = false;
                    }
                }
            } else {
                // Store file directories
                fileDirList.push_back(argv[argn]);
            }
        }
        
        if (commandPtr->begin(fileDirList)) {
            time_t startT;
            lstring outTmStr;
            std::cerr << Colors::colorize("\n_G_ +Start ") << ParseUtil::fmtDateTime(outTmStr, startT) << Colors::colorize("_X_\n");

            if (parser.patternErrCnt == 0 && parser.optionErrCnt == 0 && fileDirList.size() != 0)  {
                if (fileDirList.size() == 1 && fileDirList[0] == "-") {
                    lstring filePath;
                    while (std::getline(std::cin, filePath)) {
                        size_t filesChecked = InspectFiles(*commandPtr, filePath, 0);
                        std::cerr << "\n  Files Checked=" << filesChecked << std::endl;
                    }
                } else {
                    for (lstring& filePath : fileDirList)  {
                        size_t filesChecked = InspectFiles(*commandPtr, filePath, 0);
                        std::cerr << "\n  Files Checked=" << filesChecked << std::endl;
                    }
                }
            }

            commandPtr->end();
            time_t endT;
            ParseUtil::fmtDateTime(outTmStr, endT);
            std::cerr << Colors::colorize("_G_ +End ")
                << outTmStr
                << ", Elapsed "
                << std::difftime(endT, startT)
                << Colors::colorize(" (sec)_X_\n");
        }

        std::cerr << std::endl;
    }

    return 0;
}
