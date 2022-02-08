@echo -off
mode 80 25

cls
if exist fs0:\kernel.efi then
 fs0:
 kernel.efi
 goto END
endif

 echo "Unable to find bootloader".
 
:END