#include <stdint.h>
#include <stddef.h>
#include <stivale.h>

// We need to tell the stivale bootloader where we want our stack to be.
// We are going to allocate our stack as an uninitialised array in .bss.
static uint8_t stack[4096];

// The stivale specification says we need to define a "header structure".
// This structure needs to reside in the .stivalehdr ELF section in order
// for the bootloader to find it. We use this __attribute__ directive to
// tell the compiler to put the following structure in said section.
__attribute__((section(".stivalehdr"), used))
static struct stivale_header stivale_hdr = {
    // Let's tell the bootloader where our stack is.
    // We need to add the sizeof(stack) since in x86(_64) the stack grows
    // downwards.
    .stack = (uintptr_t)stack + sizeof(stack),
    // These flags can be used to tell the bootloader to enable or disable
    // certain features. We turn on bit 0 as we are going to ask for a
    // graphical framebuffer. Not setting this bit will make the
    // bootloader default to a CGA-compatible text mode instead.
    .flags = (1 << 0),
    // We set all the framebuffer specifics to 0 as we want the bootloader
    // to pick the best it can.
    .framebuffer_width  = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp    = 0,
    // The entry_point member is used to specify an alternative entry
    // point that the bootloader should jump to instead of the executable's
    // ELF entry point. We do not care about that so we leave it zeroed.
    .entry_point = 0
};

// The following will be our kernel's entry point.
void _start(struct stivale_struct *stivale_struct) {
    // Let's get the address of the framebuffer.
    uint8_t *fb_addr = (uint8_t *)stivale_struct->framebuffer_addr;

    // Let's try to paint a few pixels white in the top left, so we know
    // that we booted correctly.
    for (size_t i = 0; i < 128; i++) {
        fb_addr[i] = 0xff;
    }

    // We're done, just hang...
    for (;;) {
        asm ("hlt");
    }
}
