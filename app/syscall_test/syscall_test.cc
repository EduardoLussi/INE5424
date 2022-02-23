#include <syscall/stub_thread.h>
#include <syscall/stub_mutex.h>

using namespace EPOS;

OStream cout;

Stub_Mutex * m = new Stub_Mutex();

int func_a();
int func_b();

int main()
{
    cout << "Hello Syscall!" << endl;
    m->lock();
    Stub_Thread * a = new Stub_Thread(&func_a);
    Stub_Thread * b = new Stub_Thread(&func_b);
    m->unlock();
    a->join();
    b->join();
    cout << "Bye Syscall!" << endl;
    return 0;
}

int func_a(){
    m->lock();
    cout << "Funcao A" << endl;
    m->unlock();
    cout << "Bye A" <<endl;
    return 0;
}

int func_b(){
    m->lock();
    cout << "Funcao B" <<endl;
    cout << "Bye B" <<endl;
    m->unlock();
    return 0;
}