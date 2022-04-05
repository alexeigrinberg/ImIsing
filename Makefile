default: gui.cpp helper.cpp ising.cpp imgui/backends/imgui_impl_sdl.cpp imgui/backends/imgui_impl_opengl3.cpp imgui/imgui*.cpp
	c++ `sdl2-config --cflags` -I imgui -I imgui/backends gui.cpp helper.cpp ising.cpp imgui/backends/imgui_impl_sdl.cpp imgui/backends/imgui_impl_opengl3.cpp imgui/imgui*.cpp `sdl2-config --libs` -lGL -ldl
