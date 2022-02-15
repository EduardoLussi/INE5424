// EPOS Segment Test Program
#include <time.h>
#include <process.h>
#include <machine.h>

using namespace EPOS;

int func_a();
int print_inf();

OStream cout;

int main()
{
    cout << "===== Test Task Fork "<< Task::self()->address_space()<<" =====" << endl;

    if (Task::self()->address_space() == 0x00000000fe087fc8) {
        Task(Task::self(), &func_a);
        Task(Task::self(), &func_a);
        cout << "Hello World! I'm Task: "<< Task::self()->address_space() << endl;
    }
    if (Task::self()->address_space() == 0x00000000fe077d70) {
        cout << "Konnichiwa I'm Task: "<< Task::self()->address_space() << endl;
        func_a();
    }
    if (Task::self()->address_space() == 0x00000000fe06fc50) {
        cout << "Annyeong haseyo I'm Task: "<< Task::self()->address_space() << endl;
        func_a();
    }

    print_inf();
    cout << "Sayonara, bye! o/" << endl;
    return 0;
}

int print_inf() {
    Task * task = Task::self();
    Address_Space * as = task->address_space();
    cout << "===========================================" << "\n"
         << "=Address Space page directory: " << as->pd() << "=\n"
         << "=Code Logical Addr: " << static_cast<void *>(task->code()) << "=\n"
         << "=Code Physical Addr: " << static_cast<void *>(as->physical(task->code())) << "=\n"
         << "=Code Size: " << task->code_segment()->size() << " bytes long" << "=\n"
         << "=Data Logical Addr: " << static_cast<void *>(task->data()) << "=\n"
         << "=Data Physical Addr: " << static_cast<void *>(as->physical(task->data())) << "=\n"
         << "=Data Size: " << task->data_segment()->size() << " bytes long" << "=\n"
         << "===========================================" << endl;
    return 0;
}

int func_a() {
    cout << "Konnichiwa I'm Task: "<< Task::self()->address_space() << endl;
    int step = 1000000;
    for (int i = 0; i < 100000000; ++i) {
      if (!(i % step)) {
        cout << "Checkpoint! ("<<Task::self()->address_space() <<")("<<i/step<<")" << endl;
        Alarm::delay(100000);
      }
    }
    cout << "Task " << Task::self()->address_space() << " says sayonara!" << endl;
    return 0;
}