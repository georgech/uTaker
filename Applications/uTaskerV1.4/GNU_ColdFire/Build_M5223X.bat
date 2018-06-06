SET PATH=%PATH%;C:/Program Files/CodeSourcery/Sourcery G++ Lite/bin
SET PATH=%PATH%;../../../tools
cs-make -f make_uTaskerV1.4_GNU_M5223X all
uTaskerCombine "../../uTaskerBoot/GNU_M522XX/uTasker_boot.bin" uTaskerV1.4-BM.bin 0x1000 uTaskerBM.bin
m68k-elf-objcopy --input-target=binary --output-target=srec uTaskerBM.bin uTaskerV1.4_BM.s19
uTaskerConvert.exe uTaskerV1.4-BM.bin H_Upload.bin -0x1234 -a748b6531124