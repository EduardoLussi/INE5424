// EPOS Component Declarations

#ifndef __stub_segment_h
#define __stub_segment_h

#include <architecture.h>
#include <syscall/message.h>
#include <memory.h>

__BEGIN_API

__USING_UTIL

class Stub_Segment
{
private:
    long int id;
    typedef _SYS::Message Message;

public:
    Stub_Segment(){}
    void set_id(long int _id) {id = _id;}
    int get_id() {return id;}

    Stub_Segment(unsigned int bytes, MMU::Flags flags){
        Message * msg = new Message(0, Message::ENTITY::SEGMENT, Message::SEGMENT_CREATE, bytes, flags);
        msg->act();
        id = msg->result();
    }

    Stub_Segment(Segment::Phy_Addr phy_addr, unsigned int bytes, MMU::Flags flags){
        Message * msg = new Message(0, Message::ENTITY::SEGMENT, Message::SEGMENT_CREATE_PHY, phy_addr, bytes, flags);
        msg->act();
        id = msg->result();
    }

    unsigned int size () {
        Message * msg = new Message(id, Message::ENTITY::SEGMENT, Message::SEGMENT_SIZE);
        msg->act();
        int r = msg->result();
        return reinterpret_cast<int>(r);
    }

    Segment::Phy_Addr phy_address() {
        Message * msg = new Message(id, Message::ENTITY::SEGMENT, Message::SEGMENT_PHY_ADDRESS);
        msg->act();
        int r = msg->result();
        //return reinterpret_cast<Segment::Phy_Addr>(r);
        return r;
    }

    void reflag(MMU::Flags flags) {
        Message * msg = new Message(0, Message::ENTITY::SEGMENT, Message::SEGMENT_REFLAG, flags);
        msg->act();
    }

    int resize (int amount) {
        Message * msg = new Message(id, Message::ENTITY::SEGMENT, Message::SEGMENT_RESIZE, amount);
        msg->act();
        long int r = msg->result();
        return r;
    }
};

__END_API

#endif
