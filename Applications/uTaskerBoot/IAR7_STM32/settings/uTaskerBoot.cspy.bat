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


"C:\Program Files\IAR Systems\Embedded Workbench 6.0 Kickstart_3\common\bin\cspybat" "C:\Program Files\IAR Systems\Embedded Workbench 6.0 Kickstart_3\arm\bin\armproc.dll" "C:\Program Files\IAR Systems\Embedded Workbench 6.0 Kickstart_3\arm\bin\armstlink.dll"  %1 --plugin "C:\Program Files\IAR Systems\Embedded Workbench 6.0 Kickstart_3\arm\bin\armbat.dll" --backend -B "--endian=little" "--cpu=Cortex-M4" "--fpu=None" "-p" "C:\Program Files\IAR Systems\Embedded Workbench 6.0 Kickstart_3\arm\CONFIG\debugger\ST\iostm32f4xxx.ddf" "--drv_verify_download" "--semihosting=none" "--device=STM32F4xxx" "--stlink_interface=SWD" "--stlink_reset_strategy=0,0" "--drv_swo_clock_setup=72000000,0,2000000" 

