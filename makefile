default:
	g++ -o Pin8 -O2 SRC/App.cpp SRC/MainWindow.cpp SRC/Screen.cpp SRC/Core/Chip8Core.cpp -lbe -ltracker
	xres -o Pin8 SRC/Resources.rsrc
	mimeset -f "Pin8"