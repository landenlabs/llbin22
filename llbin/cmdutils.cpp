//-------------------------------------------------------------------------------------------------
//
// File: cmdutils.cpp   Author: Dennis Lang   Desc: Command utility functions
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

#include "cmdutils.hpp"


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




namespace CmdUtils {

//-------------------------------------------------------------------------------------------------
const string Q2 = "\"";
const string quote(const string& str) {
    return (str.find(" ") == string::npos) ? str : (Q2 + str + Q2);
}

#ifdef HAVE_WIN

//-------------------------------------------------------------------------------------------------
std::string GetErrorMsg(DWORD error)
{
    std::string errMsg;
    if (error != 0)
    {
        LPTSTR pszMessage;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            error,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&pszMessage,
            0, NULL);

        errMsg = pszMessage;
        LocalFree(pszMessage);
        int eolPos = (int)errMsg.find_first_of('\r');
        errMsg.resize(eolPos);
        errMsg.append(" ");
    }
    return errMsg;
}

//-------------------------------------------------------------------------------------------------
// Has statics - not thread safe.
const char* RunExtension(std::string& exeName)
{
    // Cache results - return previous match
    static const char* s_extn = NULL;
    static std::string s_lastExeName;
    if (s_lastExeName == exeName)
        return s_extn;
    s_lastExeName = exeName;

    /*
    static char ext[_MAX_EXT];
    _splitpath(exeName.c_str(), NULL, NULL, NULL, ext);

    if (ext[0] == '.')
        return ext;
    */

    // Expensive - search PATH for executable.
    char fullPath[MAX_PATH];
    static const char* s_extns[] = { NULL, ".exe", ".com", ".cmd", ".bat", ".ps" };
    for (unsigned idx = 0; idx != ARRAYSIZE(s_extns); idx++)
    {
        s_extn = s_extns[idx];
        DWORD foundPathLen = SearchPath(NULL, exeName.c_str(), s_extn, ARRAYSIZE(fullPath), fullPath, NULL);
        if (foundPathLen != 0)
            return s_extn;
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------------
bool RunCommand(const char* command, DWORD* pExitCode, int waitMsec)
{
    std::string tmpCommand(command);
    /*
    const char* pEndExe = strchr(command, ' ');
    if (pEndExe == NULL)
        pEndExe = strchr(command, '\0');
    std::string exeName(command, pEndExe);

    const char* exeExtn = RunExtension(exeName);
    static const char* s_extns[] = { ".cmd", ".bat", ".ps" };
    if (exeExtn != NULL)
    {
        for (unsigned idx = 0; idx != ARRAYSIZE(s_extns); idx++)
        {
            const char* extn = s_extns[idx];
            if (strcmp(exeExtn, extn) == 0)
            {
                // Add .bat or .cmd to executable name.
                tmpCommand = exeName + extn + pEndExe;
                break;
            }
        }
    }
    */

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    memset(&pi, 0, sizeof(pi));

    // Start the child process.
    if (!CreateProcess(
        NULL,   // No module name (use command line)
        (LPSTR)tmpCommand.c_str(), // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory
        &si,            // Pointer to STARTUPINFO structure
        &pi))           // Pointer to PROCESS_INFORMATION structure
    {
        DWORD err = GetLastError();
        if (pExitCode)
            *pExitCode = err;
        std::cerr << "Failed " << tmpCommand << " " << GetErrorMsg(err) << std::endl;
        return false;
    }

    // Wait until child process exits.
    DWORD createStatus = WaitForSingleObject(pi.hProcess, waitMsec);

    if (pExitCode)
    {
        *pExitCode = createStatus;
        if (createStatus == 0 && !GetExitCodeProcess(pi.hProcess, pExitCode))
            *pExitCode = (DWORD)-1;
    }

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return true;
}



#else
//-------------------------------------------------------------------------------------------------
bool RunCommand(const char* command, DWORD* pExitCode, int waitMsec)
{
    std::vector<char> buffer(128);
    std::string result;

    auto pipe = popen(command, "r"); // get rid of shared_ptr
    if (!pipe)
        throw std::runtime_error("popen() failed!");

    while (!feof(pipe)) {
        if (fgets(buffer.data(), 128, pipe) != nullptr)
            result += buffer.data();
    }

    auto rc = pclose(pipe);

    if (pExitCode != NULL) {
        *pExitCode = rc;
    }
    if (rc == EXIT_SUCCESS) { // == 0
    } else if (rc == EXIT_FAILURE) {  // EXIT_FAILURE is not used by all programs, maybe needs some adaptation.
    }
    return true;
}
#endif


//-------------------------------------------------------------------------------------------------
// Extract name part from path, name includes extension
lstring& getName(lstring& outName, const lstring& inPath) {
    size_t nameStart = inPath.rfind(SLASH_CHAR) + 1;
    if (nameStart == 0)
        outName = inPath;
    else
        outName = inPath.substr(nameStart);
    return outName;
}

//-------------------------------------------------------------------------------------------------
// Extract name part from path
lstring& getDir(lstring& outDir, const lstring& inPath) {
    size_t nameStart = inPath.rfind(SLASH_CHAR)+1;
    if (nameStart == 0)
        outDir = "";
    else
        outDir = inPath.substr(0, nameStart);
    return outDir;
}

//-------------------------------------------------------------------------------------------------
// Extract directory and name parts from path
void getDirName(lstring& outDir, lstring& outName, const lstring& inPath) {
    size_t nameStart = inPath.rfind(SLASH_CHAR) +1;
    if (nameStart == 0) {
        outDir = "";
        outName = inPath;
    } else {
        outDir = inPath.substr(0, nameStart);
        outName = inPath.substr(nameStart);
    }
}

//-------------------------------------------------------------------------------------------------
// Extract directory, name and extension parts from path
void getPathParts(lstring& outDir, lstring& outName,  lstring& outExt, const lstring& inPath) {
    size_t nameStart = inPath.rfind(SLASH_CHAR) +1;
    if (nameStart == 0) {
        outDir = "";
        outName = inPath;
    } else {
        outDir = inPath.substr(0, nameStart);
        outName = inPath.substr(nameStart);
    }
    
    size_t extPos= outName.rfind(EXTN_CHAR) + 1;
    if (extPos == 0)
        outExt = "";
    else {
        extPos--;
        outExt = outName.substr(extPos);
        outName.resize(extPos);
    }
}

//-------------------------------------------------------------------------------------------------
// Return just extension, not including dot.
lstring& getExtn(lstring& outExt, const lstring& inPath) {
    size_t nameStart = inPath.rfind(EXTN_CHAR) + 1;
    if (nameStart == 0)
        outExt = "";
    else
        outExt = inPath.substr(nameStart, -1);
    return outExt;
}

//-------------------------------------------------------------------------------------------------
// Extract name part from path.
lstring& removeExtn(lstring& outName, const lstring& inPath)
{
    size_t extnPos = inPath.rfind(EXTN_CHAR);
    if (extnPos == std::string::npos)
        outName = inPath;
    else
        outName = inPath.substr(0, extnPos);
    return outName;
}

//-------------------------------------------------------------------------------------------------
// Return true if inName matches pattern in patternList
bool FileMatches(const lstring& inName, const PatternList& patternList, bool emptyResult)
{
    if (patternList.empty() || inName.empty())
        return emptyResult;

    for (size_t idx = 0; idx != patternList.size(); idx++)
        if (std::regex_match(inName.begin(), inName.end(), patternList[idx]))
            return true;

    return false;
}

//-------------------------------------------------------------------------------------------------
// TODO - move to directory.h
size_t fileLength(const lstring& path) {
    struct stat info;
    return (stat(path, &info) == 0) ? info.st_size : 0;
}

//-------------------------------------------------------------------------------------------------
bool deleteFile(const char* path) {
    
#if defined(_WIN32) || defined(_WIN64)
    SetFileAttributes(path, FILE_ATTRIBUTE_NORMAL);
    if (0 == DeleteFile(path)) {
        DWORD err = GetLastError();
        if (err != ERROR_FILE_NOT_FOUND) {  // 2 = ERROR_FILE_NOT_FOUND
            std::cerr << err << " error trying to delete " << path << std::endl;
            return false;
        }
    }
#else
    unlink(path);
#endif
    return true;
}

// End namespace
}
