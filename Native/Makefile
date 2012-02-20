####################################################
###### Copyright(c) GHI Electronics, LLC ######
####################################################

OUTFILE=RLP_test
LINKERSCRIPT = RLP_LinkScript.lds


INCL=./include

CC		=arm-none-eabi-gcc
LD		=arm-none-eabi-gcc

CCFLAGS=  -g -mlittle-endian -mcpu=arm7tdmi  -Wall -I. -I$(INCL)
CCFLAGS+= -mapcs-frame -fno-builtin -g0 -save-temps 
CCFLAGS+= -Os

LDFLAGS =-nostartfiles -Wl,--Map -Wl,./Output/$(OUTFILE).map
LDFLAGS+=-lc -lgcc -Wl,--omagic
LDFLAGS+=-T $(LINKERSCRIPT)

OBJS+= Debug.o \
		main.o \
		Bluetooth.o \
		AnalogRead.o \
		fourier.o \
		sin.o\
		time.o \
		pwm.o \
		helpers.o \
		gainControl.o
		
rebuild: clean all del_o

all: $(OBJS)
	$(LD) $(LDFLAGS) -o ./Output/$(OUTFILE).elf $(OBJS)
	

Debug.o: Debug.c Debug.h
	$(CC) -c $(CCFLAGS) Debug.c -o Debug.o
	
main.o: main.c Debug.h
	$(CC) -c $(CCFLAGS) main.c -o main.o
	
Bluetooth.o: Bluetooth.c Debug.h
	$(CC) -c $(CCFLAGS) Bluetooth.c -o Bluetooth.o
	
AnalogRead.o: AnalogRead.c AnalogRead.h
	$(CC) -c $(CCFLAGS) AnalogRead.c -o AnalogRead.o
	
time.o: time.c time.h
	$(CC) -c $(CCFLAGS) time.c -o time.o
	
fourier.o: fourier.c fourier.h
	$(CC) -c $(CCFLAGS) fourier.c -o fourier.o
	
sin.o: sin.c sin.h
	$(CC) -c $(CCFLAGS) sin.c -o sin.o
	
pwm.o: pwm.c pwm.h
	$(CC) -c $(CCFLAGS) pwm.c -o pwm.o
	
helpers.o: helpers.c helpers.h
	$(CC) -c $(CCFLAGS) helpers.c -o helpers.o

gainControl.o: gainControl.c gainControl.h
	$(CC) -c $(CCFLAGS) gainControl.c -o gainControl.o
	
clean:
	-rm *.o ./Output/*.elf ./Output/*.map

del_o:
	-rm *.o

del_map:
	-rm ./Output/*.map