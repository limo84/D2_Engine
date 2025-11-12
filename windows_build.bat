:: gcc test.c src/*.c vendor/*.c -o test -I src/ -I vendor/ -L lib/ -lm -lSDL2 -lSDL2_mixer -lmingw32 -mwindows -lsetupapi -limm32 -lole32 -loleaut32 -lcomdlg32 ^
:: -luser32 -lgdi32 -lwinmm -lversion -luuid

mkdir libd2
gcc -g -c -fPIC src/d2.c -o libd2/d2.o -I src/ -I vendor/
gcc -g -c -fPIC vendor/glad.c -o libd2/glad.o -I src/ -I vendor/
gcc -g -c -fPIC vendor/stb_image.c -o libd2/stb_image.o -I src/ -I vendor/
gcc -g -c -fPIC src/d2_sdl.c -o libd2/d2_sdl.o -I src/ -I vendor/
gcc -g -c -fPIC src/d2_sprite.c -o libd2/d2_sprite.o -I src/ -I vendor/
ar rcs libd2/libd2.a libd2/d2.o libd2/glad.o libd2/stb_image.o libd2/d2_sdl.o libd2/d2_sprite.o