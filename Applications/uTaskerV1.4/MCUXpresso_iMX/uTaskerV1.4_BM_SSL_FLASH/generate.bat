arm-none-eabi-objcopy  --output-target=binary uTaskerV1.4_BM.elf "..\Applications\uTaskerV1.4\MCUXpresso_iMX\uTaskerV1.4_BM_SSL_FLASH\uTaskerV1.4_BM_SSL.bin"
arm-none-eabi-objcopy  --output-target=srec uTaskerV1.4_BM.elf "..\Applications\uTaskerV1.4\MCUXpresso_iMX\uTaskerV1.4_BM_SSL_FLASH\uTaskerV1.4_BM_SSL.srec"
arm-none-eabi-objcopy  --output-target=ihex uTaskerV1.4_BM.elf "..\Applications\uTaskerV1.4\MCUXpresso_iMX\uTaskerV1.4_BM_SSL_FLASH\uTaskerV1.4_BM_SSL.hex"
rem Convert the binary file to an SD card upload file
rem "..\Tools\uTaskerConvert" "..\Applications\uTaskerV1.4\MCUXpresso_iMX\uTaskerV1.4_BM_SSL_FLASH\uTaskerV1.4_BM_SSL.bin" "..\Applications\uTaskerV1.4\MCUXpresso_iMX\uTaskerV1.4_BM_SSL_FLASH\software.bin -0x1234 -a748b6531124"

