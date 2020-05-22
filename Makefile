CFLAGS+=-g# -fsanitize=address

all: Ezo_i2c.o BUILD_LIB
	$(CXX) $(CFLAGS) main.cpp -o ezo.out -L./ -lezoi2c

Ezo_i2c.o: Ezo_I2c_lib/Ezo_i2c.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

BUILD_LIB: Ezo_i2c.o
	$(LD) -r Ezo_i2c.o -o libezoi2c.a

clean:
	rm -f *.o *.out *.a