FILES=./build/blank.o 
INCLUDES= -I../stdlib/src

all: ${FILES}
	i686-elf-gcc -g -T ./linker.ld -o ./blank.elf -ffreestanding -O0 -nostdlib -fpic -g ${FILES} ../stdlib/stdlib.elf

./build/%.o: ./%.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c $< -o $@

clean:
	rm -rf ${FILES}
	rm ./blank.elf
