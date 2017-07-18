The FT800 emulator from FTDI is supplied as VS 2012 and VS 2015 versions.
Copy the corresponding library file from one of the sub-foldere to here in case a different version is needed.
Take the ft8xxemu.dll file from the appropriate sub-directory and copy it to the simulator's working directory to ensure that its can be found at run-time (eg. copy to \Applications\uTaskerV1.4\Simulator\uTasker___Win32_uTasker_Kinetis_build)

When using VS 2012 version ensure that FT8XXEMU_VERSION_API 9 is set in WinSimMain.cpp.
When using VS 2015 version ensure that FT8XXEMU_VERSION_API 10 is set in WinSimMain.cpp.
In case they don't match the library used the FT800 emulator will not start and an exception will result to indicate the mismatch so that it can be quickly identified.
