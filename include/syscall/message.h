// EPOS Thread Component Declarations

#ifndef __message_h
#define __message_h

#include <architecture.h>

__BEGIN_SYS

class Message
{
public:
    enum {
        PRINT,
        DELETE,

        THREAD_CREATE,
        THREAD_CREATE_CONFIG,
        //THREAD_PRIORITY,
        THREAD_PRIORITY1,
        THREAD_TASK,
        THREAD_JOIN,
        THREAD_PASS,
        THREAD_SUSPEND,
        THREAD_RESUME,
        THREAD_SELF,
        THREAD_YIELD,
        THREAD_EXIT,
        THREAD_WAIT_NEXT,

        TASK_CREATE,
        TASK_ADDRESS_SPACE,
        TASK_CODE_SEGMENT,
        TASK_DATA_SEGMENT,
        TASK_CODE,
        TASK_DATA,
        TASK_MAIN,
        TASK_ENTRY,
        TASK_SELF,
        TASK_ID,

        ADDRESS_SPACE_CREATE, 
        ADDRESS_SPACE_CREATE_PD, 
        ADDRESS_SPACE_ATTACH1,
        ADDRESS_SPACE_ATTACH2,
        ADDRESS_SPACE_DETACH1,
        ADDRESS_SPACE_DETACH2,
        ADDRESS_SPACE_PHYSICAL,

        SEGMENT_CREATE,
        SEGMENT_CREATE_PHY,
        SEGMENT_SIZE,
        SEGMENT_PHY_ADDRESS,
        SEGMENT_REFLAG,
        SEGMENT_RESIZE,

        MUTEX_CREATE,
        MUTEX_LOCK,
        MUTEX_UNLOCK,
        MUTEX_HANDLER,
        MUTEX_OPERATOR,
        
        SEMAPHORE_CREATE,
        SEMAPHORE_P,
        SEMAPHORE_V,
        SEMAPHORE_HANDLER,
        SEMAPHORE_OPERATOR,

        CONDITION_CREATE,
        CONDITION_WAIT,
        CONDITION_SIGNAL,
        CONDITION_BROADCAST,
        CONDITION_HANDLER,
        CONDITION_OPERATOR,

        CLOCK_CREATE,
        CLOCK_RESOLUTION,
        CLOCK_NOW,
        CLOCK_DATE,
        CLOCK_DATE1,

        ALARM_CREATE,
        ALARM_PERIOD,
        ALARM_PERIOD1,
        ALARM_RESET,
        ALARM_FREQUENCY,
        ALARM_DELAY,
        ALARM_TIMES,
        ALARM_TIMER_PERIOD,
        ALARM_TICKS,
        ALARM_LOCK,
        ALARM_UNLOCK,
        ALARM_HANDLER,
        ALARM_INIT,

        DELAY_CREATE,

        CHRONOMETER_CREATE,
        CHRONOMETER_FREQUENCY,
        CHRONOMETER_RESET,
        CHRONOMETER_START,
        CHRONOMETER_LAP,
        CHRONOMETER_STOP,
        CHRONOMETER_READ,

        SHARED_SEGMENT_CREATE,
        SHARED_SEGMENT_NEW,
        SHARED_SEGMENT_PORT,
    };
    enum ENTITY {
        DISPLAY,
        THREAD,
        TASK,
        ADDRESS_SPACE,
        SEGMENT,
        MUTEX,
        SEMAPHORE,
        CONDITION,
        CLOCK,
        ALARM,
        DELAY,
        CHRONOMETER,
        SHARED_SEGMENT,
    };
public:
    template<typename ... Tn>
    Message(unsigned long int id, unsigned long int entity, unsigned long int method, Tn ... an): _id(id), _entity(entity), _method(method) {set_params(an ...);}

    template<typename ... Tn>
    void get_params(Tn && ... an) {
        // Force a compilation error in case out is called with too many arguments
        typename IF<(SIZEOF<Tn ...>::Result <= 256), int, void>::Result index = 0;
        DESERIALIZE(_params, index, an ...);
    }
    template<typename ... Tn>
    void set_params(const Tn & ... an) {
        // Force a compilation error in case out is called with too many arguments
        typename IF<(SIZEOF<Tn ...>::Result <= 256), unsigned long int, void>::Result index = 0;
        SERIALIZE(_params, index, an ...);
    }

    void act() { CPU::syscall(this); }

    unsigned long int id(){return _id;}
    unsigned long int entity(){return _entity;}
    unsigned long int method(){return _method;}
    unsigned long int result(){return _result;}
    void result(unsigned long int r){_result = r;}
    char* params(){return _params;}

private:
    unsigned long int _id;
    unsigned long int _entity;
    unsigned long int _method;
    unsigned long int _result;
    char _params[256];

};


__END_SYS

#endif
