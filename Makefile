CC := clang++
CXXFLAGS := -std=c++17 $(OPTIMIZE) -Wall
INCLUDES := -Iinclude
SOURCE_DIR := src
OBJECTS_DIR := objs
SOURCES := $(wildcard $(SOURCE_DIR)/*.cpp) 
HEADERS := $(wildcard $(SOURCE_DIR)/*.h) 
OBJECTS := $(addprefix $(OBJECTS_DIR)/, $(notdir $(patsubst %.cpp, %.o, $(SOURCES))))

all: alien jsongen

alien: $(filter-out $(OBJECTS_DIR)/jsongen.o, $(OBJECTS))
	$(CC) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

jsongen: $(filter-out $(OBJECTS_DIR)/main.o, $(OBJECTS))
	$(CC) $(CXXFLAGS) $(LDFLAGS) $^ -o $@
	
$(OBJECTS_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	$(CC) $(CXXFLAGS) $(INCLUDES) -c $^ -o $@

.PYONY clean:
	rm $(OBJECTS_DIR)/*.o alien jsongen
