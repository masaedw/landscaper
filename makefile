OUTPUT = landscaper
OBJS = main.o gamesetting.o gamemain.o gamemainobject.o programsystemmain.o systemstate.o common.o gamenetwork.o basesetting.o effectstate.o fieldsetting.o result.o title.o gamedemo.o textinput.o

CPP = g++
LINK =  -lSDL -lSDL_mixer -lSDL_net -lSDL_image -lSDL_ttf -lSDL_gfx -lGL  -lglut -lm
LINKDIR = -L/usr/lib -L/usr/X11R6/lib

all:	$(OUTPUT)

$(OUTPUT): $(OBJS)
	$(CPP) -O3 $(LINKDIR) -o $@ $(OBJS) $(LINK)

.cpp.o:
	$(CPP) -O3 -c $< -o $@

clean:
	-rm -f $(OUTPUT) $(OBJS) .nfs* *~ core.*



