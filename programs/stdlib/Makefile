FILES=./build/start.asm.o ./build/os.asm.o 
INCLUDES=

all: ${FILES}
	i686-elf-ld -m elf_i386 -relocatable ${FILES} -o stdlib.elf

./build/%.o: ./src/%.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c $< -o $@

./build/%.asm.o: ./src/%.asm
	nasm -f elf -g $< -o $@

clean:
	rm -rf ${FILES}
	rm ./stdlib.elf
