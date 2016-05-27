IDIR=../include
CC=g++
CFLAGS=-I$(IDIR) -fpermissive -g

ODIR=../obj
LDIR=../lib

#Libs are included without the lib prefix because -l does that for you
LIBS=-lm -lSDL2 -lSDL2_mixer -lSDL2_image -lchipmunk

#H files go here
_DEPS = main.h ai_interpret.h audio.h controller.h dumb_physics.h dumb_ui.h entity.h graphics.h jsmn.h main.h mystrings.h parseentity.h parselevel.h parseobject.h player.h 
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

#Target object files go here (There basically the c files)
_OBJ = ai_interpret.o audio.o dumb_physics.o dumb_ui.o entity.o game.o graphics.o jsmn.o main.o mymath.o mystrings.o parseentity.o parselevel.o parseobject.o player.o controller.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o : %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

#The end program results name
../program_name : $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 

