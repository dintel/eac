CC = gcc

CFLAGS = -Wall			 	\
	 -O3			 	\
	 -std=c99			\
	 -pedantic-errors		\
	 -g				\
	 -DDEBUG			\
         -pthread                       \
	 -lm

COFLAGS = -c				\
	  -Wall			 	\
	  -O3			 	\
	  -std=c99			\
	  -pedantic-errors		\
	  -g				\
          -pthread                      \
	  -DDEBUG

ENCODER = eac_encode
DECODER = eac_decode
ENCODER_OBJECTS = encoder.o lz77.o cfc.o bit_string.o bit_string_writer.o log.o queue.o block.o
DECODER_OBJECTS = decoder.o lz77.o cfc.o bit_string.o bit_string_writer.o log.o

.PHONY: all

all: tags $(ENCODER) $(DECODER)

$(ENCODER): $(ENCODER_OBJECTS)
	$(CC) $(ENCODER_OBJECTS) -o $@ $(CFLAGS)

$(DECODER): $(DECODER_OBJECTS)
	$(CC) $(DECODER_OBJECTS) -o $@ $(CFLAGS)

%.o: %.c
	$(CC) $(COFLAGS) -c -o $@ $<

clean:
	rm -f $(ENCODER) $(DECODER) *.o

tags: *.c *.h
	etags *.c *.h

doc: doc/Doxyfile *.c *.h
	cd doc; doxygen; cd ..
