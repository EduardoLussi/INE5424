// EPOS Semaphore Component Test Program

#include <machine/display.h>
#include <time.h>
#include <synchronizer.h>
#include <process.h>
#include <real-time.h>
#include <time.h>

using namespace EPOS;

RT_Thread* tasks[3];

OStream cout;

int s = 1000000; // Seconds

Chronometer* cA = new Chronometer();
void A() {
    cout << "Thread A running..." << endl;
    cA->start();
    while (1) { // Busy wait 5s until it is preempted
        cA->lap();
        if (int(cA->read()) >= int(5*s)) break;
    }
    cA->reset();
    cout << "Thread A finished" << endl;
}

Chronometer* cB = new Chronometer();
void B() {
    cout << "Thread B running..." << endl;
    cB->start();
    while (1) { // Busy wait 1s until it is preempted
        cB->lap();
        if (int(cB->read()) >= int(1*s)) break;
    }
    cB->reset();
    cout << "Thread B finished" << endl;
}

Chronometer* cC = new Chronometer();
void C() {
    cout << "Thread C running..." << endl;
    cC->start();
    while (1) { // Busy wait 0.2s until it is preempted
        cC->lap();
        if (int(cC->read()) >= int(0.2*s)) break;
    }
    cC->reset();
    cout << "Thread C finished" << endl;
}

int main()
{
    tasks[0] = new RT_Thread(&A, 10*s, 10*s);   // period = deadline = 10s
    tasks[1] = new RT_Thread(&B, 4*s, 4*s);     // period = deadline = 4s
    tasks[2] = new RT_Thread(&C, 1*s, 1*s);     // period = deadline = 1s
    // C should run first and can preempt A and B
    // B should run if C is not running and can preempt A
    // A should run if neither C or B is running
}