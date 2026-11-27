/* pico.h — minimal shim: only what asm_helper.S / boot2 need from the SDK. */
#ifndef PICO_H
#define PICO_H
#define WRAPPER_FUNC_NAME(x) __wrap_##x
#endif
