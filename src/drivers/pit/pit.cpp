#include <drivers/pit/pit.h>

#define CHANNEL_0_PORT 0x40
#define COMMAND_PORT   0x43

uint64_t PIT::ticks_count = 0;

void PIT::on_tick()
{
    PIT::ticks_count++;
}

void PIT::initialize()
{

}

