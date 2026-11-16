#include "irq.h"
#include "../drivers/io/io.h"

void irq_disable(IRQ irq) {
    int port = IRQ_MASTER_PORT;
    IRQ relative_irq = irq;

    if (irq >= PIC_SLAVE_STARTING_IRQ) {
        port = IRQ_SLAVE_PORT;
        relative_irq = irq - PIC_SLAVE_STARTING_IRQ;
    }

    int pic_value = inb(port);
    pic_value |= (1 << relative_irq);
    outb(port, pic_value);
}

void irq_enable(IRQ irq) {
    int port = IRQ_MASTER_PORT;
    IRQ relative_irq = irq;

    if (irq >= PIC_SLAVE_STARTING_IRQ) {
        port = IRQ_SLAVE_PORT;
        relative_irq = irq - PIC_SLAVE_STARTING_IRQ;
    }

    int pic_value = inb(port);
    pic_value &= ~(1 << relative_irq);
    outb(port, pic_value);
}