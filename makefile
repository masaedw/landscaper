OUTPUT = landscaper
OBJS = main.o gamesetting.o gamemain.o gamemainobject.o programsystemmain.o systemstate.o common.o gamenetwork.o basesetting.o effectstate.o fieldsetting.o result.o title.o gamedemo.o textinput.o

CPP = g++
CFLAGS = $(shell sdl-config --cflags) -O3 -c
LDFLAGS =  $(shell sdl-config --libs) -lSDL_mixer -lSDL_net -lSDL_image -lSDL_ttf -lSDL_gfx -lGL -lm

all:	$(OUTPUT)

$(OUTPUT): $(OBJS)
	$(CPP) $(LDFLAGS) $^ -o $@

.cpp.o:
	$(CPP) $(CFLAGS) $< -o $@

clean:
	-rm -f $(OUTPUT) $(OBJS) .nfs* *~ core.*



