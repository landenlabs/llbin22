//-------------------------------------------------------------------------------------------------
//
//  threadjob.hpp
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

#define USE_THREAD

#ifdef USE_THREAD
#include <atomic>         // std::atomic
#include <thread>         // std::thread
#include <memory>         // unique_ptr

#include "ringbuffer.hpp"
#include "lstring.hpp"
#include "commands.hpp"


//-------------------------------------------------------------------------------------------------
// Class to manage thread jobs
// TODO - use a template instead of explicit type for Command
class ThreadJob {
public:
    Command&    command;
    lstring     name;
    std::thread thread1;
    bool        allOfFile;
    bool        verbose;
    
   
    ThreadJob(Command& _cmd, const lstring& _name, bool _allOfFile, bool _verbose = false) :
        command(_cmd),
        name(_name),
        allOfFile(_allOfFile),
        verbose(_verbose),
        thread1(&ThreadJob::doJobThreadFnc, this)
    { }
    
    ~ThreadJob()
    { }
    
    ThreadJob& operator=( ThreadJob& other) {
        if (this != &other) {
            command = other.command;
            name = other.name;
            allOfFile = other.allOfFile;
            verbose = other.verbose;
            thread1.swap(other.thread1);
        }
        return *this;
    }

    // Start thread to save image.
    static bool StartThread(Command& cmd, const lstring& _name, bool allOfFile);
    
    // Wait for pending threads to complete.
    static void EndThreads();
    
private:
    void doJobThreadFnc();
};
#endif
