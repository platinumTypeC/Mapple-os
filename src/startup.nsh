@echo -off
mode 80 25

cls
if exist fs0:\efi\Boot\boot.efi then
 fs0:
 .\efi\Boot\boot.efi
 goto END
endif

 echo "Unable to find bootloader".
 
:END