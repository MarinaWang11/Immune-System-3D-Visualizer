EXE = 3dvis
CXX = g++
IMGUI_DIR = lib/imgui

# Fontes do Projeto
SOURCES = src/main.cpp \
          src/glad.cpp \
          include/Sphere.cpp \
          $(IMGUI_DIR)/imgui.cpp \
          $(IMGUI_DIR)/imgui_demo.cpp \
          $(IMGUI_DIR)/imgui_draw.cpp \
          $(IMGUI_DIR)/imgui_tables.cpp \
          $(IMGUI_DIR)/imgui_widgets.cpp \
          $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp \
          $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp

# Nomes dos Arquivos Objeto
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))

# Flags de Compilação e Bibliotecas
CXXFLAGS = -std=c++11 -g -Wall -Wformat
CXXFLAGS += -I./include -I./lib -I./$(IMGUI_DIR) -I./$(IMGUI_DIR)/backends
CXXFLAGS += `pkg-config --cflags glfw3`

LIBS = -lglfw -lGL -lX11 -lXxf86vm -lXrandr -lpthread -lassimp -lXi -ldl -lXinerama -lXcursor
LIBS += `pkg-config --static --libs glfw3`

# Regras de Compilação (Build Rules)

# Regra principal: criar o executável final
all: $(EXE)

# Regra para linkar todos os arquivos .o no executável
$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(LIBS)


# Regra para arquivos na pasta src/ (main.cpp, glad.cpp)
main.o glad.o: %.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Regra para arquivos na pasta include/ (Sphere.cpp)
Sphere.o: %.o: include/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Regra para arquivos do ImGui
imgui.o imgui_demo.o imgui_draw.o imgui_tables.o imgui_widgets.o: %.o: $(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Regra para arquivos de backend do ImGui
imgui_impl_glfw.o imgui_impl_opengl3.o: %.o: $(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Regra para limpar o projeto
clean:
	rm -f $(EXE) $(OBJS)