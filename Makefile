#OBJS specifies which files to compile as part of the project
OBJS = subtask2.cpp
OBJS2 = subtask2_client.cpp
#CC specifies which compiler we're using
CC = g++

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -w

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2	-lSDL2_image	-lSDL2_mixer	 -lSDL2_ttf

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = server
OBJ_NAME2 = client

#This is the target that compiles our executable
all : 

s : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME) -pthread
	./$(OBJ_NAME)	127.0.0.1
c : $(OBJS2)
	$(CC) $(OBJS2) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME2) -pthread
	./$(OBJ_NAME2)	127.0.0.1