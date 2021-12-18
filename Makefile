FILES = ./build/kernel.asm.o ./build/kernel.o ./build/idt/idt.asm.o  ./build/idt/idt.o ./build/keyboard/keyboard.o ./build/keyboard/classic.o ./build/loader/formats/elf.o ./build/loader/formats/elfloader.o ./build/task/tss.asm.o ./build/isr80h/isr80h.o ./build/isr80h/io.o ./build/isr80h/misc.o ./build/task/task.asm.o ./build/task/process.o ./build/task/task.o ./build/gdt/gdt.asm.o ./build/gdt/gdt.o ./build/memory/memory.o ./build/io/io.asm.o ./build/memory/heap/heap.o ./build/memory/heap/kernelHeap.o ./build/memory/paging/paging.o ./build/memory/paging/paging.asm.o ./build/disk/disk.o ./build/string/string.o ./build/fileSystem/pathParser.o ./build/fileSystem/file.o ./build/disk/streamer.o ./build/fileSystem/fat/fat16.o
INCLUDES = -I./src
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: ./bin/boot.bin ./bin/kernel.bin user_programs
	rm -rf ./bin/os.bin
	dd if=./bin/boot.bin > ./bin/os.bin
	dd if=./bin/kernel.bin >> ./bin/os.bin
	dd if=/dev/zero bs=1048576 count=16 >> ./bin/os.bin
	
	sudo mkdir /mnt/d
	sudo mount -t vfat ./bin/os.bin /mnt/d
	# Copy a file over
	sudo cp ./hello.txt /mnt/d
	sudo cp ./programs/blank/blank.elf /mnt/d
	
	sudo umount /mnt/d
	sudo rm -rf /mnt/d

./bin/kernel.bin: $(FILES)
	i686-elf-ld -g -relocatable $(FILES) -o ./build/kernelfull.o
	i686-elf-gcc $(FLAGS) -T ./src/linker.ld -o ./bin/kernel.bin ./build/kernelfull.o 

./bin/boot.bin: ./src/boot/boot.asm
	nasm -f bin ./src/boot/boot.asm -o ./bin/boot.bin

./build/%.o: ./src/%.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c $< -o $@

./build/%.asm.o: ./src/%.asm
	nasm -f elf -g $< -o $@

run:
	qemu-system-i386 -hda ./bin/os.bin

user_programs:
	cd ./programs/stdlib && $(MAKE) all
	cd ./programs/blank && $(MAKE) all

user_program_clean:
	cd ./programs/blank && $(MAKE) clean

.PHONY: clean
clean: user_program_clean
	rm -rf ./bin/*
	rm -rf $(FILES)
