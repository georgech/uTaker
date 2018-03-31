arm-none-eabi-objcopy  --output-target=binary uTaskerV1.4.elf "..\Applications\uTaskerV1.4\MCUXpresso\uTaskerV1.4_SSL_FLASH\uTaskerV1.4_SSL.bin"
arm-none-eabi-objcopy  --output-target=srec uTaskerV1.4.elf "..\Applications\uTaskerV1.4\MCUXpresso\uTaskerV1.4_SSL_FLASH\uTaskerV1.4_SSL.srec"
arm-none-eabi-objcopy  --output-target=ihex uTaskerV1.4.elf "..\Applications\uTaskerV1.4\MCUXpresso\uTaskerV1.4_SSL_FLASH\uTaskerV1.4_SSL.hex"

