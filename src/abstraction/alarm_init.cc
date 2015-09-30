// EPOS Alarm Abstraction Initialization

#include <alarm.h>
#include <system.h>

__BEGIN_SYS

void Alarm::init()
{
    db<Init, Alarm>(TRC) << "Alarm::init()" << endl;

    _timer = new (KERNEL) Alarm_Timer(handler);
}

__END_SYS
