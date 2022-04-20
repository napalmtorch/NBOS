mkdir "bin/iso"
mkdir "bin/iso/boot"
mkdir "bin/iso/boot/grub"

./ramfsmgr build/disks/a/build

cp 'bin/kernel.bin' 'bin/iso/boot/kernel.bin'
cp 'ramdisk.img' 'bin/iso/boot/ramdisk.img'
cp 'build/grub.cfg' 'bin/iso/boot/grub/grub.cfg'
grub-mkrescue -o 'NBOS.iso' 'bin/iso'
cp 'NBOS.iso' 'NBOS_FLP.img'
