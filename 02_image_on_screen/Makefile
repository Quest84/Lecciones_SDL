#OBJS especifican que archivos se compilarán como parte del proyecto
OBJS = 02_image_on_screen.cpp

#CC especifica que compilador se usará
CC = g++

#COMPILER_FLAGS especifica las opciones adicionales de compilación que se usarán
#-w suprime todos los warning
COMPILER_FLAGS = -w

#LINKER_LAGS especifica las librerías que se enlazaran
LINKER_FLAGS = -lSDL2

#OBJ_NAME especifica el nombre del ejecutable
OBJ_NAME = 02_image_on_screen

#Esto es el target que compilará el ejecutable
all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
clean: $(OBJS)
	rm $(OBJ_NAME)
