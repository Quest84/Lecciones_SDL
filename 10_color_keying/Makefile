#OBJS especifican que archivos se compilarán como parte del proyecto
OBJS = $(SOURCES)/main.cpp 

#CC especifica que compilador se usará
CC = g++

SOURCES		:= source
DATA		:= data
INCLUDES	:= include

#COMPILER_FLAGS especifica las opciones adicionales de compilación que se usarán
#-w suprime todos los warning
COMPILER_FLAGS = -w

#LINKER_LAGS especifica las librerías que se enlazaran
LINKER_FLAGS = -lSDL2 -lSDL2_image

#OBJ_NAME especifica el nombre del ejecutable
OBJ_NAME = bin 

#Esto es el target que compilará el ejecutable
all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
clean: $(OBJS)
	rm $(OBJ_NAME)
