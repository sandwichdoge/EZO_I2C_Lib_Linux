CFLAGS+=-g -fsanitize=address

all: Ezo_i2c.o
	$(CXX) $(CFLAGS) main.cpp Ezo_i2c.o -o ezo.out

Ezo_i2c.o: Ezo_I2c_lib/Ezo_i2c.cpp
	$(CXX) $(CFLAGS) -c Ezo_I2c_lib/Ezo_i2c.cpp -o Ezo_i2c.o

clean:
	rm -f *.o *.out