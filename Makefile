CFLAGS= -g
CXXFLAGS= -g -std=gnu++23 -Wall -Wextra -pedantic
LDFLAGS= -lsqlite3 -lcrypto

%: %.c
	$(CC) -o $@ $^ $(LIBS) $(CFLAGS) $(LDFLAGS)

%: %.cc
	$(CXX) -o $@ $^ $(LIBS) $(CXXFLAGS) $(LDFLAGS)

all: rainbowPIN sha256

rainbowPIN:

sha256:

.PHONY: clean
clean:
	rm -rf rainbowPIN sha256
