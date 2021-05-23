#ifndef __TYPES_H
#define __TYPES_H
#include <stdint.h>

#define NULL 0

__attribute__((unused))  static inline unsigned char inb(uint16_t _port)
{
    uint8_t result;
    __asm__ volatile("inb %1, %0" : "=a" (result) : "Nd" (_port));
    return result;
}

__attribute__((unused))  static inline void outb(uint16_t _port, uint8_t _data)
{
    __asm__ volatile("outb %0, %1" : : "a" (_data), "Nd" (_port));
}

__attribute__((unused))  static inline short inw(uint16_t _port)
{
    uint16_t result;
    __asm__ volatile("inw %1, %0" : "=a" (result) : "Nd" (_port));
    return result;
}

__attribute__((unused))  static inline void outw(uint16_t _port, uint16_t _data)
{
    __asm__ volatile("outw %0, %1" : : "a" (_data), "Nd" (_port));
}

__attribute__((unused))  static inline unsigned int inl(unsigned short port)
{
	unsigned int ret = 0;
                    /* inb or inl ???*/
	 __asm__ volatile("inl %1, %0" : "=a" (ret) : "Nd" (port));
	return ret;
}

__attribute__((unused)) static inline void outl(unsigned short port, unsigned int value)
{
	 __asm__ volatile("outl %0, %1" : : "a" (value), "Nd" (port));
}

__attribute__((unused)) static uint16_t mem_inb(uintptr_t offset)
{
    uint8_t *mem = (uint8_t *)(offset);
    return (mem[0]);
}

__attribute__((unused)) static uint16_t mem_inw(uintptr_t offset)
{
    uint16_t *mem = (uint16_t *) (offset);
    return (mem[0]);
}

__attribute__((unused))  static uint32_t mem_inl(uintptr_t offset)
{
    uint32_t *mem = (uint32_t *) (offset);
    return (mem[0]);
}

__attribute__((unused)) static uint32_t mem_inq(uintptr_t offset)
{
    uintptr_t *mem = (uintptr_t *)(offset);
    return (mem[0]);
}
__attribute__((unused))  static void mem_outb(uintptr_t address, uint8_t value)
{
    uint8_t *mem = (uint8_t *)(address);
    mem[0] = value;
}

__attribute__((unused))  static void mem_outw(uintptr_t address, uint16_t value)
{
    uint16_t *mem = (uint16_t *)(address);
    mem[0] = value;
}

__attribute__((unused))  static void mem_outl(uintptr_t address, uint32_t value)
{
    uint32_t *mem= (uint32_t *)(address);
    mem[0] = value;
}

__attribute__((unused))  static void mem_outq(uintptr_t address, uint64_t value)
{
    uint64_t *mem = (uint64_t *)(address);
    mem[0] = value;
}
#define mb()  __asm__ __volatile__ (""   : : :"memory")
#define sti() __asm__ __volatile__ ("sti": : :"memory")
#define cli() __asm__ __volatile__ ("cli": : :"memory")

#define UNUSED(x) (void)x
#endif