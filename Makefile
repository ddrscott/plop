# OSX Specific for now
all: dev

dev:
	g++ plop.cpp `./build/wx-config --cxxflags --libs all` -mmacosx-version-min=10.5 -o plop

dist:
	g++ plop.cpp `./build/wx-config --cxxflags --libs all` -mmacosx-version-min=10.5 -02 -o plop
