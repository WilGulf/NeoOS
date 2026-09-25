#include "../../include/stdint.h"
#include "io.h"

inline void outb(uint16_t port, uint8_t data) {
    __asm__ volatile(
        "outb %b0, %w1" : : "a"(data), "Nd"(port) : "memory"
    );
}

inline void outw(uint16_t port, uint16_t data) {
    __asm__ volatile(
        "outw %w0, %w1" : : "a"(data), "Nd"(port) : "memory"
    );
}

inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile (
        "inb %w1, %b0"
        : "=a"(ret)
        : "Nd"(port)
        : "memory");

    return ret;
}

inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile (
        "inw %w1, %w0"
        : "=a"(ret)
        : "Nd"(port)
        : "memory");
        
    return ret;
}