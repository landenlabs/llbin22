//-------------------------------------------------------------------------------------------------
//
//  threadjob.cpp
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

#include "threadjob.hpp"

static const int MAX_THREAD_JOBS = 10;
static RingBuffer<ThreadJob*> jobQueue(MAX_THREAD_JOBS);

//-------------------------------------------------------------------------------------------------
void ThreadJob::doJobThreadFnc() {
    // std::cerr << "Run Thread=" << thread1.get_id() << name << std::endl;
    command.doJob(name, allOfFile);
}

//-------------------------------------------------------------------------------------------------
bool ThreadJob::StartThread(Command& cmd, const lstring& name, bool allOfFile) {
    ThreadJob* jobPtr;

    if (jobQueue.Full()) {
        jobQueue.Get(jobPtr);
        // std::cerr << "Save Queue Full - join thread " << saveAuxPtr->name << std::endl;
        jobPtr->thread1.join();
        delete jobPtr;
    }

    jobPtr = new ThreadJob(cmd, name, allOfFile, false);
    return jobQueue.Put(jobPtr);
}

//-------------------------------------------------------------------------------------------------
void ThreadJob::EndThreads() {
    ThreadJob* jobPtr;
    while (! jobQueue.Empty()) {
        jobQueue.Get(jobPtr);
        jobPtr->thread1.join();
        delete jobPtr;
    }
}
