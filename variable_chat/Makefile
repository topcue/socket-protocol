CC		= gcc
CFlAGS	= -W -Wall -o2

N=variable_chat
S=$N_server
C=$N_client
c=c

all: $S $C

$S: $S.c
	@echo "##### build server"
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$C: $C.$c
	@echo "##### build client"
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	@rm -rf *.o

# EOF

