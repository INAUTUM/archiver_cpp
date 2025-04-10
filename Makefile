s: build run

.PHONY: build
build:
	clang++ src/main.cpp src/archiver.cpp src/logger.cpp -o myapp -std=c++23 -lz

.PHONY: run
run:
	./myapp