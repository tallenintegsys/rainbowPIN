CFLAGS= -g
CXXFLAGS= -O3 -Wall -pedantic
LDFLAGS= -lsqlite3 -lcrypto

%: %.c
	$(CC) -o $@ $^ $(LIBS) $(CFLAGS) $(LDFLAGS)

%: %.cc
	$(CXX) -o $@ $^ $(LIBS) $(CXXFLAGS) $(LDFLAGS)

all: rainbowPIN

rainbowPIN:

sha256:

.PHONY: clean
clean:
	rm -rf rainbowPIN sha256
