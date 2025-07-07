
test: test.c
	gcc test.c -o test libd2/libd2.so -I src/ -I vendor/


#################################################################
#                     SHARED LIBS                               #
#################################################################
# ----- LIBD2 -------------------------------------------------

INCS := -I src/ -I vendor/
OBJS_D2 := libd2/d2.o libd2/glad.o libd2/stb_image.o libd2/d2_sdl.o

libd2.so: $(OBJS_D2)
	gcc -shared -o libd2/libd2.so $(OBJS_D2) -lSDL2 -lSDL2_mixer
	rm libd2/*.o

libd2/%.o: src/%.c src/*.h
	gcc -g -c -fPIC $< -o $@ $(INCS)

libd2/%.o: vendor/%.c src/*.h
	gcc -g -c -fPIC $< -o $@ $(INCS)

libd2:
	mkdir -p libd2


# ----------------------------------------------------------------

hotwindow: d2_hotwindow.c libd2.so
	gcc -g d2_hotwindow.c -o window -I src/ -I vendor/ $(CFLAGS) libd2/libd2.so


#################################################################
#                     STATIC LIBS                               #
#################################################################

install: lib lib/d2.o lib/glad.o lib/stb_image.o
	ar rcs lib/libd2.a lib/d2.o lib/glad.o lib/stb_image.o
	sudo cp lib/libd2.a /usr/local/lib/libd2.a
	sudo cp -r include /usr/local/include/d2/
	rm -rf lib

uninstall:
	sudo rm -rf /usr/local/lib/libd2.a
	sudo rm -rf /usr/local/include/d2/

lib:
	mkdir -p lib
