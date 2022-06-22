CFLAGS= -g
LDFLAGS= -lsqlite3

%: %.c
	$(CC) -o $@ $^ $(LIBS) $(CFLAGS) $(LDFLAGS)

rainbowPIN:

.PHONY: clean
clean:
	rm -rf rainbowPIN
