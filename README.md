# Mapple-os
An OS using the `UEFI` bootloading system as well as many other mordern feature to try and create an OS
with as little bloat as possible while supporting features that allow any `Portable Executable`(mordern
Microsoft .exe format) and `ELF` format files to run natively on it.

## Build
Requirements:
  * [ ] WSL or LINUX device with GNU make installed
  * [ ] QEMU installed( you need to add QEMU to path if you are on WSL)


First run `make install` to install dependeces of Mapple os, these being:
  * binutils
  * gcc
  * ccd2iso

then run `make` to build and finally `make iso` to make an .img file in dist/iso/Mapple.img. then run it
by `make run` or `make runwsl` if you are on WSL.

## TODO
* [ ] Make an iso file from .elf
* [ ] Make a Simple UI
  * [ ] Make a simple desktop for debuging
  * [ ] Make a Simple Base UI driver
  * [ ] Port it to be compatable with linux.elf and  windows.dll which will be called if any elf programs or .exe files are run respectively
