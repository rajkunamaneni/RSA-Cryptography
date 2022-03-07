C = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic `pkg-config --cflags gmp`  
LDFLAGS = `pkg-config --libs gmp`
OBJS = numtheory.o randstate.o rsa.o 

all: decrypt encrypt keygen 

encrypt: encrypt.o $(OBJS) 
	$(CC) -o encrypt encrypt.o $(OBJS) $(LDFLAGS)

decrypt: decrypt.o $(OBJS) 
	$(CC) -o decrypt decrypt.o $(OBJS) $(LDFLAGS)

keygen: keygen.o $(OBJS) 
	$(CC) -o keygen keygen.o $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f encrypt encrypt.o decrypt decrypt.o keygen keygen.o $(OBJS)

debug: CFLAGS += -g

debug: clean all
