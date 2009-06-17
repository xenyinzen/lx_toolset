OBJ= winbmp.o bitmap.o  fb.o showbitmap.o 
TARGET=showbitmap 

all: $(TARGET) 

$(TARGET): $(OBJ) 
	$(CC) $(OBJ) -o $(TARGET) 

.c.o: 
	$(CC) -Wall -g -c -o $@ $< 

clean: 
	rm $(TARGET) -f 
	rm *.o -f 
	rm *~ -f 
