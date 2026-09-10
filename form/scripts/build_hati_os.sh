#!/bin/sh
# OS compilation/linking carrier; Form emits leaves and packs the disk image.
set -eu
if [ "$#" -ne 4 ]; then
    echo 'usage: build_hati_os.sh CLANG LLD LLVM_OBJCOPY OUTPUT_DIRECTORY' >&2
    exit 2
fi
hati_cc=$1
hati_ld=$2
hati_objcopy=$3
hati_out=$4
hati_tools=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
hati_root=$(CDPATH= cd -- "$hati_tools/../.." && pwd)
hati_here="$hati_root/os/hati-os"
mkdir -p "$hati_out"
hati_out=$(CDPATH= cd -- "$hati_out" && pwd)
cd "$hati_root"
rm -f "$hati_out/guest-native.S" "$hati_out/hati.img"
printf '%s\n' "$hati_out" | ./fkwu os/hati-os/emit-run.fk
test -s "$hati_out/guest-native.S"
"$hati_cc" --target=i386-none-elf -march=i386 -mno-sse -mno-sse2 -mno-mmx \
    -ffreestanding -fno-pic -fno-stack-protector -Os -Wall -Wextra \
    -c "$hati_here/kernel.c" -o "$hati_out/kernel.o"
"$hati_cc" --target=i386-none-elf -c "$hati_here/entry.S" -o "$hati_out/entry.o"
"$hati_cc" --target=i386-none-elf -c "$hati_out/guest-native.S" -o "$hati_out/guest-native.o"
"$hati_ld" -flavor gnu -m elf_i386 -T "$hati_here/linker.ld" -o "$hati_out/kernel.elf" \
    "$hati_out/entry.o" "$hati_out/kernel.o" "$hati_out/guest-native.o"
"$hati_objcopy" -O binary "$hati_out/kernel.elf" "$hati_out/kernel.bin"
"$hati_cc" --target=i386-none-elf -c "$hati_here/boot.S" -o "$hati_out/boot.o"
"$hati_ld" -flavor gnu -m elf_i386 --image-base=0 -Ttext 0x7C00 --oformat binary \
    -o "$hati_out/boot.bin" "$hati_out/boot.o"
printf '%s\n' "$hati_out" | ./fkwu os/hati-os/pack-run.fk
test -s "$hati_out/hati.img"
