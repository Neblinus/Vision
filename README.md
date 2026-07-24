# Vision
Simple BrainFuck to C transpiler using only standard C code.

Completely portable and cross-platform.

The transpiled C code uses `stdint.h`'s `uint8_t` type for the array on which BrainFuck programs operate to
avoid `unsigned char` sizing issues accross platforms.

Some run length encoding (e.g. `++++` --> `*pointer += 4`) is done for optimization purposes, but nothing too
complex i.e. doesn't optimize loops (at least not yet).

You can manage the size of the array on which the program operates through a command-line argument.

## Usage

```bash
git clone https://github.com/Neblinus/Vision.git # Clone this repo
gcc -o vision Vision/vision.c -O3                # Compile the transpiler with optimizations
vision path/to/brainfuck/source.bf array_size    # Run with the input source and the desired array size in bytes
```

## Example

BrainFuck source (prints "Hello World!\n"):

```brainfuck
++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.
```

Pasting the code on a `sample.bf` file, and passing it to the transpiler with
`./vision sample.bf 32`, using an array/tape size of 32 bytes, the output is:

```c
#include <stdio.h>
#include <stdint.h>
typedef uint8_t byte;
int main(void)
{
	byte memblock[32] = {0};
	byte *pointer = memblock;
	*pointer += 8;
	while (*pointer != 0)
	{
	
	pointer += 1;
	*pointer += 4;
	while (*pointer != 0)
	{
	
	pointer += 1;
	*pointer += 2;
	pointer += 1;
	*pointer += 3;
	pointer += 1;
	*pointer += 3;
	pointer += 1;
	*pointer += 1;
	pointer -= 4;
	*pointer -= 1;
	
	}
	pointer += 1;
	*pointer += 1;
	pointer += 1;
	*pointer += 1;
	pointer += 1;
	*pointer -= 1;
	pointer += 2;
	*pointer += 1;
	while (*pointer != 0)
	{
	
	pointer -= 1;
	
	}
	pointer -= 1;
	*pointer -= 1;
	
	}
	pointer += 2;
	printf("%c", *pointer);
	pointer += 1;
	*pointer -= 3;
	printf("%c", *pointer);
	*pointer += 7;
	printf("%c", *pointer);
	*pointer += 3;
	printf("%c", *pointer);
	pointer += 2;
	printf("%c", *pointer);
	pointer -= 1;
	*pointer -= 1;
	printf("%c", *pointer);
	pointer -= 1;
	printf("%c", *pointer);
	*pointer += 3;
	printf("%c", *pointer);
	*pointer -= 6;
	printf("%c", *pointer);
	*pointer -= 8;
	printf("%c", *pointer);
	pointer += 2;
	*pointer += 1;
	printf("%c", *pointer);
	pointer += 1;
	*pointer += 2;
	printf("%c", *pointer);
	
}
```

Which you can then compile and run:

```
gcc -o main c-code-output.c -O3
./main
Hello World!
```
