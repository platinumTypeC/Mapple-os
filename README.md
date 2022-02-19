# Mapple-os
Mapple-OS in an Attempt at a Simple Modern Operating System that, while not being a Unix-like or windows-like OS hopes to be
Capable of running any of the executables originally intended for one of the above-mentioned operating systems. It Uses
The UEFI boot loading system. So no Assembly is required instead the bootloader is written entirely in c while the kernel is
entirely in C++.

## Cloning the Repository
`git clone --recurse-submodules https://github.com/platinumTypeC/Mapple-os.git <folder-here>`

## Building The kernel

To build the kernel you need:
* GNU Make
* linux or wsl
* qemu installed(put qemu on path if you using wsl).

first run `make install` to install `gcc`, `binutils` and `mtools`(Note This only works if you are in a linux distribution 
with apt). then run `make linux`(`make wsl` for wsl) for compiling and runing using qemu, remember to put qemu on path if you 
are using wsl.