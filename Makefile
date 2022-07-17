CFLAGS= -g
CXXFLAGS= -g -std=c++2a -Wall -Wextra -pedantic
LDFLAGS= -lsqlite3 -lssl -lcrypto -lpthread

%: %.c
	$(CC) -o $@ $^ $(LIBS) $(CFLAGS) $(LDFLAGS)

%: %.cc
	$(CXX) -o $@ $^ $(LIBS) $(CXXFLAGS) $(LDFLAGS)

all: rainbowPIN

rainbowPIN:

sha256:

.PHONY: clean lint
lint: rainbowPIN
	clang-tidy -checks=cert-* --warnings-as-errors=* $(^).cc

clean:
	rm -rf rainbowPIN sha256
