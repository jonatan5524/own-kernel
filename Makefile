FILES = ./build/kernel.asm.o ./build/kernel.o ./build/idt/idt.asm.o  ./build/idt/idt.o ./build/task/tss.asm.o  ./build/task/process.o ./build/task/task.o ./build/gdt/gdt.asm.o ./build/gdt/gdt.o ./build/memory/memory.o ./build/io/io.asm.o ./build/memory/heap/heap.o ./build/memory/heap/kernelHeap.o ./build/memory/paging/paging.o ./build/memory/paging/paging.asm.o ./build/disk/disk.o ./build/string/string.o ./build/fileSystem/pathParser.o ./build/fileSystem/file.o ./build/disk/streamer.o ./build/fileSystem/fat/fat16.o
INCLUDES = -I./src
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: ./bin/boot.bin ./bin/kernel.bin
	rm -rf ./bin/os.bin
	dd if=./bin/boot.bin > ./bin/os.bin
	dd if=./bin/kernel.bin >> ./bin/os.bin
	dd if=/dev/zero bs=1048576 count=16 >> ./bin/os.bin
	
	sudo mkdir /mnt/d
	sudo mount -t vfat ./bin/os.bin /mnt/d
	# Copy a file over
	sudo cp ./hello.txt /mnt/d
	
	sudo umount /mnt/d
	sudo rm -rf /mnt/d

./bin/kernel.bin: $(FILES)
	i686-elf-ld -g -relocatable $(FILES) -o ./build/kernelfull.o
	i686-elf-gcc $(FLAGS) -T ./src/linker.ld -o ./bin/kernel.bin ./build/kernelfull.o 

./bin/boot.bin: ./src/boot/boot.asm
	nasm -f bin ./src/boot/boot.asm -o ./bin/boot.bin

./build/kernel.asm.o: ./src/kernel.asm
	nasm -f elf -g ./src/kernel.asm -o ./build/kernel.asm.o

./build/kernel.o: ./src/kernel.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/kernel.c -o ./build/kernel.o

./build/gdt/gdt.asm.o: ./src/gdt/gdt.asm
	nasm -f elf -g ./src/gdt/gdt.asm -o ./build/gdt/gdt.asm.o

./build/gdt/gdt.o: ./src/gdt/gdt.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -I./src/gdt -std=gnu99 -c ./src/gdt/gdt.c -o ./build/gdt/gdt.o

./build/idt/idt.asm.o: ./src/idt/idt.asm
	nasm -f elf -g ./src/idt/idt.asm -o ./build/idt/idt.asm.o

./build/idt/idt.o: ./src/idt/idt.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -I./src/idt -std=gnu99 -c ./src/idt/idt.c -o ./build/idt/idt.o

./build/memory/memory.o: ./src/memory/memory.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -I./src/memory -std=gnu99 -c ./src/memory/memory.c -o ./build/memory/memory.o

./build/task/process.o: ./src/task/process.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -I./src/task -std=gnu99 -c ./src/task/process.c -o ./build/task/process.o

./build/task/task.o: ./src/task/task.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -I./src/task -std=gnu99 -c ./src/task/task.c -o ./build/task/task.o

./build/task/tss.asm.o: ./src/task/tss.asm
	nasm -f elf -g ./src/task/tss.asm -o ./build/task/tss.asm.o

./build/io/io.asm.o: ./src/io/io.asm
	nasm -f elf -g ./src/io/io.asm -o ./build/io/io.asm.o

./build/memory/heap/heap.o: ./src/memory/heap/heap.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -I./src/memory/heap -std=gnu99 -c ./src/memory/heap/heap.c -o ./build/memory/heap/heap.o

./build/memory/heap/kernelHeap.o: ./src/memory/heap/kernelHeap.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -I./src/memory/heap -std=gnu99 -c ./src/memory/heap/kernelHeap.c -o ./build/memory/heap/kernelHeap.o

./build/memory/paging/paging.o: ./src/memory/paging/paging.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -I./src/memory/paging -std=gnu99 -c ./src/memory/paging/paging.c -o ./build/memory/paging/paging.o

./build/memory/paging/paging.asm.o: ./src/memory/paging/paging.asm
	nasm -f elf -g ./src/memory/paging/paging.asm -o ./build/memory/paging/paging.asm.o

./build/disk/disk.o: ./src/disk/disk.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -I./src/disk -std=gnu99 -c ./src/disk/disk.c -o ./build/disk/disk.o

./build/string/string.o: ./src/string/string.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -I./src/string -std=gnu99 -c ./src/string/string.c -o ./build/string/string.o

./build/fileSystem/pathParser.o: ./src/fileSystem/pathParser.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -I./src/fileSystem -std=gnu99 -c ./src/fileSystem/pathParser.c -o ./build/fileSystem/pathParser.o

./build/fileSystem/file.o: ./src/fileSystem/file.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -I./src/fileSystem -std=gnu99 -c ./src/fileSystem/file.c -o ./build/fileSystem/file.o

./build/fileSystem/fat/fat16.o: ./src/fileSystem/fat/fat16.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -I./src/fileSystem -I./src/fileSystem/fat -std=gnu99 -c ./src/fileSystem/fat/fat16.c -o ./build/fileSystem/fat/fat16.o

./build/disk/streamer.o: ./src/disk/streamer.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -I./src/disk -std=gnu99 -c ./src/disk/streamer.c -o ./build/disk/streamer.o

clean:
	rm -rf ./bin/*
	rm -rf ./build/*.o
	rm -rf ./build/*/*.o
	rm -rf $(FILES)
