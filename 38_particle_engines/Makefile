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

.PHONY: dir

$(APP) : $(OBJSUBDIRS) $(ALLCPPSOBJ) $(ALLCPPSOBJ)
	$(CC) -o $(APP) $(patsubst $(SRC)%,$(OBJ)%,$(ALLCPPSOBJ) $(ALLCSOBJ)) $(LIBS)

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

$(OBJSUBDIRS):
	$(MKDIR) $(OBJSUBDIRS)

#OBJS especifican que archivos se compilarán como parte del proyecto
#OBJS = $(SOURCES)/main.cpp 

#CC especifica que compilador se usará
#CC = g++

#SOURCES		:= source
#DATA		:= data
#INCLUDES	:= include

#COMPILER_FLAGS especifica las opciones adicionales de compilación que se usarán
#-w suprime todos los warning
#COMPILER_FLAGS = -w

#LINKER_LAGS especifica las librerías que se enlazaran
#LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf

#OBJ_NAME especifica el nombre del ejecutable
#OBJ_NAME = bin 

#Esto es el target que compilará el ejecutable
#all : $(OBJS)
#	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
#clean: $(OBJS)
#	rm $(OBJ_NAME)
