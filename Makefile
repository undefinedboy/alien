CC := g++
CXXFLAGS := -std=c++17 -O3 -Wall
SOURCE_DIR := .
OBJECTS_DIR := objs
SOURCES := $(wildcard $(SOURCE_DIR)/*.cpp) 
HEADERS := $(wildcard $(SOURCE_DIR)/*.h) 
OBJECTS := $(addprefix $(OBJECTS_DIR)/, $(notdir $(patsubst %.cpp, %.o, $(SOURCES))))

alien: $(OBJECTS)
	$(CC) $(CXXFLAGS) $^ -o $@
	
$(OBJECTS_DIR)/%.o: $(SOURCE_DIR)/%.cpp 
	$(CC) $(CXXFLAGS) -c $^ -o $@

.PYONY clean:
	rm $(OBJECTS_DIR)/*.o alien
