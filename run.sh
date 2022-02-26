if [[ $(grep Microsoft /proc/version) ]];
then 
    qemu-system-x86_64.exe -drive file=Mapple.img,format=raw -m 200M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="OVMFbin/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="OVMFbin/OVMF_VARS-pure-efi.fd";
else 
    qemu-system-x86_64 -drive file=Mapple.img,format=raw -m 200M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="OVMFbin/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="OVMFbin/OVMF_VARS-pure-efi.fd";
fi