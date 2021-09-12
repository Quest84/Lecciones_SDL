APP 		:= bin

CCFLAGS 	:= -Wall -pedantic
CFLAGS		:= $(CCFLAGS)
CC			:= g++
C			:= gcc
LIBS		:= -lSDL2 -lSDL2_image -lSDL2_ttf
MKDIR		:= mkdir -p
SRC			:= source
OBJ			:= obj

ALLCS		:= $(shell find $(SRC) -type f -iname *.c)
ALLCSOBJ	:= $(patsubst %.c,%.o,$(ALLCS))
ALLCPPS		:= $(shell find $(SRC) -type f -iname *.cpp)
ALLCPPSOBJ	:= $(patsubst %.cpp,%.o,$(ALLCPPS))
SUBDIRS		:= $(shell find $(SRC) -type d)
OBJSUBDIRS 	:= $(patsubst $(SRC)%,$(OBJ)%,$(SUBDIRS))

OBJSTRUEDIR	:= $(patsubst $(SRC)%,$(OBJ)%,$(ALLCPPSOBJ) $(ALLCSOBJ))

.PHONY: dir

$(APP) : $(OBJSUBDIRS) $(ALLCPPSOBJ) $(ALLCPPSOBJ)
	$(CC) -o $(APP) $(OBJSTRUEDIR) $(LIBS)

%.o : %.c
	$(C) -o $(patsubst $(SRC)%,$(OBJ)%,$@) -c $^ $(CFLAGS)

%.o : %.cpp
	$(CC) -o $(patsubst $(SRC)%,$(OBJ)%,$@) -c $^ $(CCFLAGS)
#dir no es un fichero de salida
#dir es un objetivo .PHONY - esto se lanza solo cuando está desactualizado
dir:
	$(info $(SUBDIRS))
	$(info $(OBJSUBDIRS))
	$(info $(ALLCS))
	$(info $(ALLCSOBJ))
	$(info $(ALLCPPS))
	$(info $(ALLCPPSOBJ))

clean:
	rm $(APP)
	rm -r $(OBJSTRUEDIR)

$(OBJSUBDIRS):
	$(MKDIR) $(OBJSUBDIRS)
