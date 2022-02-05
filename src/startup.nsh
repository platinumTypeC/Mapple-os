@echo -off
mode 80 25

cls
if exist .\main.efi then
 main.efi
 goto END
endif

if exist fs0:\main.efi then
 fs0:
 echo Found bootloader on fs0:
 main.efi
 goto END
endif

if exist fs1:\main.efi then
 fs1:
 echo Found bootloader on fs1:
 efi\boot\main.efi
 goto END
endif

if exist fs2:\main.efi then
 fs2:
 echo Found bootloader on fs2:
 efi\boot\main.efi
 goto END
endif

if exist fs3:\main.efi then
 fs3:
 echo Found bootloader on fs3:
 efi\boot\main.efi
 goto END
endif

if exist fs4:\main.efi then
 fs4:
 echo Found bootloader on fs4:
 efi\boot\main.efi
 goto END
endif

if exist fs5:\main.efi then
 fs5:
 echo Found bootloader on fs5:
 efi\boot\main.efi
 goto END
endif

if exist fs6:\main.efi then
 fs6:
 echo Found bootloader on fs6:
 efi\boot\main.efi
 goto END
endif

if exist fs7:\main.efi then
 fs7:
 echo Found bootloader on fs7:
 efi\boot\main.efi
 goto END
endif

 echo "Unable to find bootloader".
 
:END