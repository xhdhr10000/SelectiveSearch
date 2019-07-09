INCLUDES=*.h */*.h
SRCS=*.cpp segmentation/*.cpp

all: $(SRCS) $(INCLUDES)
	g++ $(SRCS) -o ssearch