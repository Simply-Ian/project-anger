CXX=g++
C_FLAGS= -std=c++2a -fdiagnostics-color=always -g -pthread -Ofast
LDFLAGS=-lsfml-graphics -lsfml-system -lsfml-window -lpthread
SRCS = anger_engine/conversions/rads_degs.cpp anger_engine/ResourceManager.cpp anger_engine/WinManager.cpp main.cpp os/exec.cpp os/get_default_font_filename.cpp Player.cpp anger_engine/GameObject.cpp anger_engine/Camera.cpp miniMap.cpp MyRMan.cpp Game.cpp
OBJS = $(SRCS:.cpp=.o)

.PHONY: clean

run: $(OBJS)
	$(CXX) $^ $(LDFLAGS) -o run

# Собирает из doxygen-файлов документацию в формате html
#docs: Doxyfile doxyfiles/AlignmentGroup.dox doxyfiles/Fragment.dox doxyfiles/Logger.dox doxyfiles/tocElem.dox doxyfiles/bookmarx_file.dox doxyfiles/Groups.dox doxyfiles/mainpage.dox doxyfiles/todos.dox doxyfiles/Controller.dox doxyfiles/Lifecycle.dox doxyfiles/Model.dox doxyfiles/View.dox
#	doxygen Doxyfile

# Общее правило для создания объектных файлов
%.o: %.cpp
	$(CXX) $(C_FLAGS) $< -c -o $@

clean:
	rm *.o
	cd anger_engine
	rm Camera.o GameObject.o ResourceManager.o WinManager.o
	cd ..