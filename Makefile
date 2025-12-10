CFLAGS = -std=c2x -D _DEBUG -ggdb3 -O0 -Wall -Wextra -Waggressive-loop-optimizations -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wopenmp-simd -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing -Wno-varargs -Wstack-protector -fcheck-new -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
linker_flags = -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr 
CC = gcc

# linker_flags =
# CFLAGS =

all: main

main: TreeDump.o main.o Input.o Derivative.o Fcomp.o Calculator.o Tree.o Optimizer.o Taylor.o Start.o Plot.o
	$(CC) $(linker_flags) TreeDump.o main.o Input.o Derivative.o Fcomp.o Calculator.o Tree.o Optimizer.o Taylor.o Start.o Plot.o -lm -o wolfram

TreeDump.o: TreeDump.c

main.o: main.c

Input.o: Input.c

Calculator.o: Calculator.c

Tree.o: Tree.c

Optimizer.o: Optimizer.c

Fcomp.o: Fcomp.c

Derivative.o: Derivative.c

Taylor.o: Taylor.c

Start.o: Start.c

Plot.o: Plot.c

clean:
	rm *.o
