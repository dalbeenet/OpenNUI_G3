/******************************************************************* 
FileName: main.cpp
Author  : vee
Date    : 2015-08-01 22:00:54
Desc    : Entry point of nui framework core
*******************************************************************/ 

#include <iostream>
#include <kernel/kernel.h>
#include <vee/delegate.h>
#include <conio.h>

using namespace vee;

void __cdecl f1(int64_t*)
{
    puts(__FUNCSIG__);
}

void __stdcall f2(int64_t*)
{
    puts(__FUNCSIG__);
}

int main()
{
    //TODO: Scheduler test
    xkernel::scheduler::initialize();
    xkernel::scheduler::request(vee::make_delegate<void(int64_t*)>(f1), nullptr);
    xkernel::scheduler::request(vee::make_delegate<void(int64_t*)>(f2), nullptr);
    getch();
    xkernel::scheduler::dispose();
    return 0;
}