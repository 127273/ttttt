#include <mem.h>

using namespace myos;
using namespace myos::hardware;


MemHandlerBaseClass::MemHandlerBaseClass(uintptr_t base)
{
    mmio_address = base;
}

MemHandlerBaseClass::~MemHandlerBaseClass()
{

}