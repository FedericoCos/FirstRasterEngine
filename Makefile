CXX = g++
CFLAGS = -std=c++17 -O2 -Iheaders
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

# List of source files
SRCS = Scene.cpp GameObject.cpp Player.cpp Camera.cpp Flag.cpp

# Automatically generate a list of object files
OBJS = $(SRCS:.cpp=.o)

# Name of the executable
TARGET = VulkanTest

# Default target
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJS)
	$(CXX) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Compile each .cpp file into a .o file
%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

# Phony targets
.PHONY: all clean test

# Run the program
test: $(TARGET)
	glslc shaders/shader.vert -o shaders/vert.spv
	glslc shaders/CarShader.frag -o shaders/CarFrag.spv
	glslc shaders/TreeShader.frag -o shaders/TreeFrag.spv
	glslc shaders/LavaShader.frag -o shaders/LavaFrag.spv
	glslc shaders/StreetShader.frag -o shaders/StreetFrag.spv
	glslc shaders/LampShader.frag -o shaders/LampFrag.spv
	glslc shaders/BoxShader.frag -o shaders/BoxFrag.spv
	glslc shaders/HouseShader.frag -o shaders/HouseFrag.spv
	glslc shaders/FieldShader.frag -o shaders/FieldFrag.spv
	glslc shaders/CarShaderMenu.frag -o shaders/CarMenuFrag.spv
	glslc shaders/HouseShaderMenu.frag -o shaders/HouseMenuFrag.spv
	glslc shaders/LampShaderMenu.frag -o shaders/LampMenuFrag.spv
	glslc shaders/StreetShaderMenu.frag -o shaders/StreetMenuFrag.spv
	glslc shaders/FieldShaderMenu.frag -o shaders/FieldMenuFrag.spv
	./$(TARGET)

# Clean up the build files
clean:
	rm -f $(TARGET) $(OBJS)
