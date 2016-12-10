make all:
	gcc -o ezview ezview.c -lglfw -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo

make clean:
	rm ezview
