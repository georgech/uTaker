@REM This batch file has been generated by the IAR Embedded Workbench
@REM C-SPY Debugger, as an aid to preparing a command line for running
@REM the cspybat command line utility using the appropriate settings.
@REM
@REM You can launch cspybat by typing the name of this batch file followed
@REM by the name of the debug file (usually an ELF/DWARF or UBROF file).
@REM Note that this file is generated every time a new debug session
@REM is initialized, so you may want to move or rename the file before
@REM making changes.
@REM 


"C:\Program Files\IAR Systems\Embedded Workbench 6.0 Kickstart_3\common\bin\cspybat" "C:\Program Files\IAR Systems\Embedded Workbench 6.0 Kickstart_3\arm\bin\armproc.dll" "C:\Program Files\IAR Systems\Embedded Workbench 6.0 Kickstart_3\arm\bin\armpemicro.dll"  %1 --plugin "C:\Program Files\IAR Systems\Embedded Workbench 6.0 Kickstart_3\arm\bin\armbat.dll" --flash_loader "C:\Program Files\IAR Systems\Embedded Workbench 6.0 Kickstart_3\arm\config\flashloader\Freescale\FlashK60Xxxx.board" --backend -B "--endian=little" "--cpu=Cortex-M4" "--fpu=None" "-p" "C:\Program Files\IAR Systems\Embedded Workbench 6.0 Kickstart_3\arm\CONFIG\debugger\Freescale\iok60xxxx.ddf" "--drv_verify_download" "--semihosting=none" "--device=MK60DN512Zxxx10" "--pemicro_interface_type=OSJtag" "--pemicro_reset_delay=" "--drv_communication=USB1" 


