CXX = g++
CXXFLAGS = -Wall -Wextra -O2
LIBS = -lcrypto

all: hmac_demo

hmac_demo: hmac_demo.cpp
	$(CXX) $(CXXFLAGS) hmac_demo.cpp -o hmac_demo $(LIBS)

run: hmac_demo
	./hmac_demo create message.txt
	./hmac_demo verify message.txt

clean:
	rm -f hmac_demo message.tag