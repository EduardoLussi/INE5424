// EPOS Component Declarations

#ifndef __stub_delay_h
#define __stub_delay_h

#include <architecture.h>
#include <syscall/message.h>
#include <time.h>

__BEGIN_API

__USING_UTIL

class Stub_Delay
{
private:
    long int id;
    typedef _SYS::Message Message;
    // typedef Message::ENTITY::SEMAPHORE SEMAPHORE;


public:
    Stub_Delay(const Microsecond & time){
        Message * msg = new Message(0, Message::ENTITY::DELAY, Message::DELAY_CREATE);
        msg->act();
        id = msg->result();
    }
};

__END_API

#endif
