//-------------------------------------------------------------------------------------------------
//
// File: directory.cpp  Author: Dennis Lang   Desc: Get files from directories
//
//-------------------------------------------------------------------------------------------------
//
// Author: Dennis Lang - 2022
// https://landenlabs.com
//
// This file is part of llbin22 project.
//
// ----- License ----
//
// Copyright (c) 2022  Dennis Lang
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

#include "directory.hpp"

#include <iostream>
#include <errno.h>
#include <stdio.h>


#ifdef HAVE_WIN

#include <windows.h>

const lstring ANY("\\*");
lstring Directory_files::SLASH = "\\";

//-------------------------------------------------------------------------------------------------
// Return true if attribute is a Directory
inline static bool isDir(DWORD attr) {
    return (attr != -1) && ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

//-------------------------------------------------------------------------------------------------
// Return 'clean' full path, remove extra slahes.
lstring& Directory_files::getFullPath(lstring& fname) {
    char fullPath[MAX_PATH];
    DWORD len1 = GetFullPathName(fname, ARRAYSIZE(fullPath), fullPath, NULL);
    fname = fullPath;
    return fname;
}

//-------------------------------------------------------------------------------------------------
Directory_files::Directory_files(const lstring& dirName) :
    my_dir_hnd(INVALID_HANDLE_VALUE),
    my_dirName(dirName) {
}

//-------------------------------------------------------------------------------------------------
Directory_files::~Directory_files() {
    if (my_dir_hnd != INVALID_HANDLE_VALUE)
        FindClose(my_dir_hnd);
}

//-------------------------------------------------------------------------------------------------
void Directory_files::close() {
    if (my_dir_hnd != INVALID_HANDLE_VALUE) {
        FindClose(my_dir_hnd);
        my_dir_hnd = INVALID_HANDLE_VALUE;
    }
}

//-------------------------------------------------------------------------------------------------
bool Directory_files::begin() {
    close();

    lstring dir = my_dirName;
    if (dir.empty())
        dir = ".";    // Default to current directory

    DWORD attr = GetFileAttributes(dir);
    if (isDir(attr)) {
        dir += ANY;
    } else { // if (attr != INVALID_FILE_ATTRIBUTES)
        getFullPath(my_dirName);
        // Peel off one subdir from reference name.
        size_t pos = my_dirName.find_last_of(":/\\");
        if (pos != std::string::npos)
            my_dirName.resize(pos);
    }

    my_dir_hnd = FindFirstFile(dir, &my_dirent);
    bool is_more = (my_dir_hnd != INVALID_HANDLE_VALUE);

    while (is_more
        && (isDir(my_dirent.dwFileAttributes)
        && strspn(my_dirent.cFileName, ".") == strlen(my_dirent.cFileName) )) {
        is_more = (FindNextFile(my_dir_hnd, &my_dirent) != 0);
    }

    return is_more;
}

//-------------------------------------------------------------------------------------------------
bool Directory_files::more() {
    if (my_dir_hnd == INVALID_HANDLE_VALUE)
        return begin();

    bool is_more = false;
    if (my_dir_hnd != INVALID_HANDLE_VALUE) {
        // Determine if there any more files
        //   skip any dot-directories.
        do {
            is_more = (FindNextFile(my_dir_hnd, &my_dirent) != 0);
        } while (is_more
            && (isDir(my_dirent.dwFileAttributes)
            && strspn(my_dirent.cFileName, ".") == strlen(my_dirent.cFileName)));
    }

    return is_more;
}

//-------------------------------------------------------------------------------------------------
bool Directory_files::is_directory() const {
    return (my_dir_hnd != INVALID_HANDLE_VALUE && isDir(my_dirent.dwFileAttributes));
}

//-------------------------------------------------------------------------------------------------
const char* Directory_files::name() const {
    return (my_dir_hnd != INVALID_HANDLE_VALUE) ? my_dirent.cFileName : NULL;
}

//-------------------------------------------------------------------------------------------------
lstring& Directory_files::fullName(lstring& fname) const {
    fname = my_dirName + SLASH + name();
    // return GetFullPath(fname);
    return fname;
}

//-------------------------------------------------------------------------------------------------
bool Directory_files::exists( const char* path) {
    const DWORD attr = GetFileAttributes(path);
    return (attr != INVALID_FILE_ATTRIBUTES);
}

#else

#include <stdlib.h>
#include <unistd.h>

lstring Directory_files::SLASH = "/";

//-------------------------------------------------------------------------------------------------
Directory_files::Directory_files(const lstring& dirName) {
#if 0
    realpath(dirName.c_str(), my_fullname);
    my_baseDir = my_fullname;
#else
    my_baseDir = dirName;
#endif
    my_pDir = opendir(my_baseDir);
    my_is_more = (my_pDir != NULL);
}

//-------------------------------------------------------------------------------------------------
Directory_files::~Directory_files() {
    if (my_pDir != NULL) {
        closedir(my_pDir);
        my_pDir = NULL;
        my_is_more = false;
    }
}

//-------------------------------------------------------------------------------------------------
bool Directory_files::more() {
    if (my_is_more) {
        my_pDirEnt = readdir(my_pDir);
        my_is_more = my_pDirEnt != NULL;
        if (my_is_more) {
            if (my_pDirEnt->d_type == DT_DIR) {
                while (my_is_more &&
                (my_pDirEnt->d_name[0] == '.' && ! isalnum(my_pDirEnt->d_name[1]))) {
                    more();
                }
            }
        }
    }

    return my_is_more;
}

//-------------------------------------------------------------------------------------------------
bool Directory_files::is_directory() const {
    return my_pDirEnt->d_type == DT_DIR;
}

//-------------------------------------------------------------------------------------------------
lstring& Directory_files::fullName(lstring& fname) const {
    return join(fname, my_baseDir, my_pDirEnt->d_name, my_pDirEnt->d_namlen);
}


//-------------------------------------------------------------------------------------------------
bool Directory_files::exists(const char* path) {
    return access(path, F_OK) == 0;
}

//-------------------------------------------------------------------------------------------------
// Return 'clean' full path, remove extra slahes.
lstring& Directory_files::getFullPath(lstring& path) {
    char realpathBuffer[PATH_MAX];
    realpath(path.c_str(), realpathBuffer);
    path = realpathBuffer;
    return path;
}
#endif

//-------------------------------------------------------------------------------------------------
lstring& Directory_files::join(lstring& outFull, const char* dir, const char* name, unsigned nameLen) {
    outFull.resize(0);
    outFull.reserve(strlen(dir) +1 + strlen(name));
    outFull.append(dir).append(SLASH).append(name, nameLen);
    // return realpath(fname.c_str(), my_fullname);
    return outFull;
    // return GetFullPath(fname);
}
