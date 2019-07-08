INCLUDES=*.h */*.h
SRCS=selective_search.cpp segmentation/*.cpp

all: $(SRCS) $(INCLUDES)
	g++ $(SRCS) -o ssearch