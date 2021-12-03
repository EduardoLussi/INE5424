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

void task0() {
    cout << "Thread A activated" << endl;
    for (int i = 0; i < 99999; i++);
    cout << "Thread A finished" << endl;
}

void task1() {
    cout << "Thread B activated" << endl;
    for (int i = 0; i < 99999; i++);
    cout << "Thread B finished" << endl;
}

void task2() {
    cout << "Thread C activated" << endl;
    for (int i = 0; i < 99999; i++);
    cout << "Thread C finished" << endl;
}

int main()
{
    tasks[0] = new RT_Thread(&task0, 1000000, 1000000);
    tasks[1] = new RT_Thread(&task1, 1500000, 1500000);
    tasks[2] = new RT_Thread(&task2, 3500000, 3500000);
}
