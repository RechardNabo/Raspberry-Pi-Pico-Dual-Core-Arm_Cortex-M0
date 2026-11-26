/* main.c — kit module #2 IR Receiver — PLACEHOLDER, not implemented.
 *
 * Honest status: the VS1838B outputs a demodulated 38kHz digital stream
 * (NEC protocol typically: 9ms lead, 4.5ms space, then 32 bits pulse-
 * distance encoded at ~562us granularity). Decoding it requires edge
 * timestamping at sub-100us precision — input capture infrastructure.
 *
 * On RP2040 the correct implementation is a PIO program (the signature
 * peripheral for exactly this job) or edge-triggered IRQ timestamps.
 * Neither exists in the driver library yet — the kit table marks this
 * module KIT_CLASS_UNSUPPORTED rather than pretending a GPIO poll at
 * human-perceptible rates constitutes an IR decoder.
 *
 * Implement after the PIO driver lands. See drivers/devices/kit/kit.c.
 */
int main(void)
{
    for (;;) { }
}
