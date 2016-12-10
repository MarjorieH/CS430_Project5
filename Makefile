all:
	gcc -o ezview ezview.c -lglfw -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo

clean:
	rm ezview

test:
	./ezview testimage.ppm
