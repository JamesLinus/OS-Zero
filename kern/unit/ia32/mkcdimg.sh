#! /bin/sh

#cp zero cdimg/kern
#cp zero cdimg/kern
#genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o cd.iso cdimg/
# use GRUB 2
cp zero cdimg2/kern
grub-mkrescue -o cd.iso cdimg2/
