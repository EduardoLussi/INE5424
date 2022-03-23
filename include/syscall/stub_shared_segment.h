// EPOS Component Declarations

#ifndef __stub_shared_memory_h
#define __stub_shared_memory_h

#include <architecture.h>
#include <syscall/message.h>
#include <utility/list.h>
#include <memory.h>

__BEGIN_API

__USING_UTIL


class Stub_Shared_Segment
{
private:
    int _id;
    typedef _SYS::Message Message;
    typedef _SYS::MMU MMU;
    typedef _SYS::Segment Segment;


public:
    Stub_Shared_Segment(){}
    void set_id(int id) {_id = id;}
    int get_id() {return _id;}

    Stub_Shared_Segment(int port, unsigned int bytes) {
        Message * msg = new Message(0, Message::ENTITY::SHARED_SEGMENT, Message::SHARED_SEGMENT_CREATE, port, bytes);
        msg->act();
        _id = msg->result();
    }        

    int get_port() {
        Message * msg = new Message(_id, Message::ENTITY::SHARED_SEGMENT, Message::SHARED_SEGMENT_PORT);
        msg->act();
        return msg->result();
    }
};

__END_API

#endif
