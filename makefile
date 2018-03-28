CXX = g++
CC_OPTS = -Wall -ggdb3 -std=c++11
LIBPATH = -L./lib
LIB = -lcurl -lpthread
LIB += -lcjson
TARGET = bin/epolic_upload

all : $(TARGET)

bin/epolic_upload : src/test_epolice_upload_api.cpp
	$(CXX) $(CC_OPTS) $^ -o $@  $(LIBPATH) $(LIB)

clean :
	rm -rf $(TARGET)