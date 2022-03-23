// EPOS Application Scaffold and Application Component Implementation

#include <system.h>
#include <syscall/message.h>
#include <syscall/stub_thread.h>

__BEGIN_SYS

// Application class attributes
char Application::_preheap[];
Heap * Application::_heap;

__END_SYS

__BEGIN_API

// Global objects
__USING_UTIL
OStream cout;
OStream cerr;

__END_API

_API::Stub_Task* volatile _API::Stub_Task::_current = new _API::Stub_Task();

// Bindings
extern "C" {
    void _exit(int s) { _API::Stub_Thread::exit(s); for(;;); }
}

// Bindings
extern "C" {
    __USING_SYS;

    // Utility-related methods that differ from kernel and user space.
    // OStream
    void _print(const char * s) { 
        Message msg(0, Message::ENTITY::DISPLAY, Message::PRINT);
        msg.set_params(reinterpret_cast<unsigned long int>(s));
        msg.act();
    }
}
