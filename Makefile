CXX := clang++
CXXFLAGS := -std=c++20 -Wall -Wextra -O2
LDFLAGS := -larchive

HOMEBREW_PREFIX := /opt/homebrew
LIBARCHIVE_PATH := $(HOMEBREW_PREFIX)/opt/libarchive

# Пути для включения и библиотек
INCLUDE_DIRS := -I$(LIBARCHIVE_PATH)/include
LIBRARY_DIRS := -L$(LIBARCHIVE_PATH)/lib

SRC_DIR := src
SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(SOURCES:.cpp=.o)

TARGET := myapp

.PHONY: all build clean run

all: build

build: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(LIBRARY_DIRS) $^ -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

run: build
	@./$(TARGET) $(ARGS)

clean:
	rm -f $(TARGET) $(OBJECTS) myapp.log
	rm -rf *.dSYM

gen-data:
	@echo "Генерация тестовых данных..."
	@cd generator_csv && python main.py

install-deps:
	@echo "Установка зависимостей..."
	@brew install libarchive

help:
	@echo "Использование:"
	@echo "  make build      - Собрать проект"
	@echo "  make run ARGS=  - Запустить с аргументами (пример: make run ARGS=input.csv)"
	@echo "  make clean      - Очистить артефакты сборки"
	@echo "  make gen-data   - Сгенерировать тестовые данные"
	@echo "  make install-deps - Установить зависимости"