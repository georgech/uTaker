The uTasker project includes the "uTaskerV1.4" application (complete configurable application project with OS, TCP/IP stack, USB stack, utFAT file system, command line shell [on UART, USB-CDC and Telnet] and various peipheral demonstrations), the "uTasker Serial loader" (configurable standalone UART SREC, iHEX, USB-MSD, USB-CDC, Kboot, SD card, Ethernet boot loaders) and the Bare-Minimum uTasker boot loader.

It can be built with Visual Studio (eg. VS 2015 Community) - which allows project and device simulation - or with various IDEs and cross-compilers:
- IAR, Keil uVision, Rowley Crossworks, CooCox, Green Hills, MCUXpresso, KDS, S32 Design Studio, Atollic and GCC make file



Eclipse based IDEs:

It is possible to work with CodeWarrior10.x, Atollic, S32 Design Studio, MCUXpresso or Kinetis Design Studio.
These are however all based on Eclipse and need their project configuration files to be located at the top of the project directory structure, meaning that only one can be used at a time.

To work with the one of your choice, delete the original project files at the top of the project directory. Then copy the ones in this folder to that location, import into the Eclipse based IDE that you are to work with (matching this folder's contents).
Update the copy of the project files here if needed so that they can later be restored when working with the project again, after having changed to a different IDE in the meantime.

When working with Cortex-m4 parts with FPU and KDS select Float ABI selection to "FP instructions (hard)" plus FPU Type to "fpv6-sp-d16" for optimal performance, and remove when no FPU is available!


Documentation and videos:
http://www.utasker.com/docs/documentation.html
To get started downloading, importing, building see https://www.youtube.com/watch?v=K8ScSgpgQ6M&index=9&list=PLWKlVb_MqDQFZAulrUywU30v869JBYi9Q







