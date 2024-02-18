// alarm.cc
//	Routines to use a hardware timer device to provide a
//	software alarm clock.  For now, we just provide time-slicing.
//
//	Not completely implemented.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "alarm.h"
#include "main.h"

//----------------------------------------------------------------------
// Alarm::Alarm
//      Initialize a software alarm clock.  Start up a timer device
//
//      "doRandom" -- if true, arrange for the hardware interrupts to 
//		occur at random, instead of fixed, intervals.
//----------------------------------------------------------------------

Alarm::Alarm(bool doRandom)
{
    timer = new Timer(doRandom, this);
}

//----------------------------------------------------------------------
// Alarm::CallBack
//	Software interrupt handler for the timer device. The timer device is
//	set up to interrupt the CPU periodically (once every TimerTicks).
//	This routine is called each time there is a timer interrupt,
//	with interrupts disabled.
//
//	Note that instead of calling Yield() directly (which would
//	suspend the interrupt handler, not the interrupted thread
//	which is what we wanted to context switch), we set a flag
//	so that once the interrupt handler is done, it will appear as 
//	if the interrupted thread called Yield at the point it is 
//	was interrupted.
//
//	For now, just provide time-slicing.  Only need to time slice 
//      if we're currently running something (in other words, not idle).
//----------------------------------------------------------------------

void 
Alarm::CallBack() 
{
    Interrupt *interrupt = kernel->interrupt;
    MachineStatus status = interrupt->getStatus();

    int level = kernel->currentThread->getLevel();

    if(status != IdleMode){
        Aging();
        if(level == 2){
            if(kernel->scheduler->getQueue(1)->IsEmpty()){
                interrupt->YieldOnReturn();
            }
        }else{
            interrupt->YieldOnReturn();
        }
    }
    interrupt->YieldOnReturn();
    
    // if (status != IdleMode) {
	//     interrupt->YieldOnReturn();
    // }
}


// Starvation occurs when a process is unable to execute because it is continuously being bypassed by other processes in the ready queue.
//  To address this issue, aging is introduced as a mechanism to gradually increase the priority of a process that has been waiting 
// for a long time. This ensures that even if a process has a lower priority, it will eventually get a chance to execute.

void
Alarm::Aging(){
    for(int i=1;i<=3;i++){
        List<Thread *>*queue = kernel->scheduler->getQueue(i);
        ListIterator<Thread *> *it = new ListIterator<Thread *>(queue);

        double time;
        int prevPriority;
        int nextLevel;
        for(;!it->IsDone();it->Next()){
            Thread *thread = it->Item();
            thread->setTotalWaitingTime(thread->getTotalWaitingTime() +
                                     kernel->stats->totalTicks - thread->getWaitingTime());

            thread->setWaitingTime(kernel->stats->totalTicks);
            time = thread->getTotalWaitingTime();

            //aging
            if(time>=1500){

                // if wait time>1500 tick--> aging() activate, increase the priority 
                prevPriority = thread->getPriority();
                thread->setPriority(min(prevPriority + 10, 149));
                thread->setTotalWaitingTime(thread->getTotalWaitingTime() - 1500);
                nextLevel = thread->getLevel();

                // process changes its scheduling priority
                DEBUG(dbgExpr, "[C] Tick [" << kernel->stats->totalTicks <<
                                     "]: Thread [" << thread->getID() << 
                                     "] change its priority from [" << 
                                     prevPriority << "] to [" << 
                                     thread->getPriority() << "]");

                if(nextLevel<i){

                    // process is removed from a queue
                    queue->Remove(thread);
                    DEBUG(dbgExpr, "[B] Tick [" << kernel->stats->totalTicks << 
                                    "]: Thread [" << thread->getID() << 
                                    "] is removed from queue L[" << i << "]");

                    //process is inserted into a priority queue   
                    ((SortedList<Thread *> *)kernel->scheduler->getQueue(nextLevel))->Insert(thread);
                    DEBUG(dbgExpr, "[A] Tick [" << kernel->stats->totalTicks << 
                                    "]: Thread [" << thread->getID() << 
                                    "] is inserted into queue L[" << nextLevel << "]");
                }
            }
        }
    }
}