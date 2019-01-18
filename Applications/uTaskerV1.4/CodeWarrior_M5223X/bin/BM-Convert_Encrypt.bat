SET PATH=%PATH%;..\..\..\..\tools
uTaskerCombine "..\..\..\uTaskerBoot\CodeWarrior_M5223X\bin\uTasker_boot.bin" uTasker_BM.bin 0x800 uTaskerBM_APP.bin
uTaskerConvert.exe uTasker_BM.bin H_Upload_Enc.bin -0x1235 -a748b6531124 -ff25a788f2e681338777 -afe1 -226a
