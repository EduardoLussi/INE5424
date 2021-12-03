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

void A() {
    cout << "Thread A activated" << endl;
    // Busy wait 200000
    cout << "Thread A finished" << endl;
}

void B() {
    cout << "Thread B activated" << endl;
    // Busy wait 400000
    cout << "Thread B finished" << endl;
}

void C() {
    cout << "Thread C activated" << endl;
    // Busy wait 1000000
    cout << "Thread C finished" << endl;
}

int main()
{
    tasks[0] = new RT_Thread(&A, 1000000, 1000000);
    tasks[1] = new RT_Thread(&B, 1500000, 1500000);
    tasks[2] = new RT_Thread(&C, 3500000, 3500000);
}
