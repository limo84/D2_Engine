
run:
	make test && ./test

test: test.c libd2/libd2.a
	gcc test.c -o test libd2/libd2.a -I src/ -I vendor/ -lm -lSDL2 -lSDL2_mixer


#################################################################
#                     SHARED LIBS                               #
#################################################################
# ----- LIBD2 -------------------------------------------------

INCS := -I src/ -I vendor/
OBJS_D2 := libd2/d2.o libd2/glad.o libd2/stb_image.o libd2/d2_sdl.o libd2/d2_sprite.o

libd2.so: $(OBJS_D2)
	gcc -shared -o libd2/libd2.so $(OBJS_D2) -lSDL2 -lSDL2_mixer
	rm libd2/*.o

libd2/%.o: src/%.c src/*.h
	mkdir -p libd2
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

HEADERS := src/*.h vendor/*.h

install: libd2 $(OBJS_D2)
	ar rcs libd2/libd2.a $(OBJS_D2)
	sudo cp libd2/libd2.a /usr/local/lib/libd2.a
	sudo mkdir -p /usr/local/include/d2
	sudo cp $(HEADERS) /usr/local/include/d2/

uninstall:
	sudo rm -rf /usr/local/lib/libd2.a
	sudo rm -rf /usr/local/include/d2/

