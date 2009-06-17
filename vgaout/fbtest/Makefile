OBJ = fb.o fbtest.o 
TARGET = fbtest 

all: $(TARGET) 

$(TARGET): $(OBJ) 
	$(CC) $(OBJ) -o $(TARGET) 

.c.o: 
	$(CC) -Wall -g -c -o $@ $< 

clean: 
	rm $(TARGET) -f 
	rm *.o -f 
	rm *~ -f 
