# si7210_rw_register

The 3 programs of this project allow to write and read in registers of hall effect sensor si7210 using a raspberry pi 

## Getting Started

We will not explain here the electronic part. We will show you just how to use the programs available to you.

It is highly recommend to read the documentation at https://www.silabs.com/documents/public/data-sheets/si7210-datasheet.pdf brefore using this programs.

### Prerequisites

You need for read/write registers of si7210 this :
    - raspberry pi
    - si7210
    - test board (optional)
    - gcc compiler

### Installing

Copy the repository

```
git clone https://github.com/yocvito/si7210_rw_register.git
```

Compile the c files

```
gcc -o si7210_get si721_get.c
gcc -o si7210_set si721_set.c
gcc -o si7210_wr si721_wr.c
```

### Usage

si7210_get.c :

```
Usage:  ./i2cget -otp [register_addr]
        ./i2cget -pt
```

The argument -otp allow you to read a value in otp.
```
data on 0xC0 : 0x14
data on 0xC1 : 0xC0
data on 0xC2 : 0x96
data on 0xC3 : 0x00
data on 0xC4 : 0x0A
data on 0xC5 : 0x00
data on 0xC6 : 0x9F
data on 0xC7 : 0x12
data on 0xC8 : 0xE8
data on 0xC9 : 0xC4
data on 0xCA : 0xBF
data on 0xCB : 0x58
data on 0xCC : 0xB9
data on 0xCD : 0x00
data on 0xCE : 0x45
data on 0xCF : 0x36
data on 0xD0 : 0x00
data on 0xE1 : 0x00
data on 0xE2 : 0x00
data on 0xE3 : 0x00
data on 0xE4 : 0x00

(256 * Dspsigm[6:0] + Dspsigl[7:0] - 16384) * (0.00125 or 0.0125)
(256 * 0 + 22 - 16384) * (0.00125) = 20.452499
(256 * 0 + 22 - 16384) * (0.0125) = 204.524994
```

The argument -pt displays registers and magnetic field value every 2 seconds.
```
Data on otp at address 0x05 : 0x12
```

With no argument, you just print one time the registers.
```
data on 0xC0 : 0x14
data on 0xC1 : 0xC0
data on 0xC2 : 0x4A
data on 0xC3 : 0x00
data on 0xC4 : 0x0A
data on 0xC5 : 0x00
data on 0xC6 : 0x9F
data on 0xC7 : 0x12
data on 0xC8 : 0xE8
data on 0xC9 : 0xC4
data on 0xCA : 0xBF
data on 0xCB : 0x58
data on 0xCC : 0xB9
data on 0xCD : 0x00
data on 0xCE : 0x45
data on 0xCF : 0x36
data on 0xD0 : 0x00
data on 0xE1 : 0x06
data on 0xE2 : 0x01
data on 0xE3 : 0x00
data on 0xE4 : 0x00
```

si7210_set.c :

Used only without arguments, this command allow you to write registers.

The input has to be done on 8 bits.
```
Register value for 0xC0 : 00010100

Register value for 0xC1 :

```

si7210_wr.c :

Used with one argument : the register address

As si7210_set.c, you write the register and it displays just after this
```
Register value for 0xC0 : 00010100

Data on 0xC0 : 0x14
```