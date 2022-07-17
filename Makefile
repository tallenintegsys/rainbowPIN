CFLAGS= -g
CXXFLAGS= -g -std=c++2a -Wall -pedantic
LDFLAGS= -lsqlite3 -lssl -lcrypto -lpthread

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
