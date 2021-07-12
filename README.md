1. Please change the path with your hardware path on Raspberry Pi
   Type:  `sudo ls -l /dev/ttyUSB*` on terminal on your Rpi to get your UART device path.
          `sudo ls -l /dev/ttyS0` if you have connected UART device on your Rpi TX RX

2. Please Set Baud-rate.
3. g++ compiler needs to compile your program.
4. run your program with default name `./a.out` or if you have generated alias name output file like 'abc' then run `./abc` on your terminal.
