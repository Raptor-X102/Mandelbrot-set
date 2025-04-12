# Параллелизация
PARALLEL ?= no

ifeq ($(PARALLEL),yes)
    PARALLEL_FLAGS := -DPARALLEL_ENABLED -fopenmp
else
    PARALLEL_FLAGS :=
endif
# Компилятор
CXX = x86_64-w64-mingw32-g++

# Флаги компиляции
CXXFLAGS = -mavx2 -O3 $(PARALLEL_FLAGS) -Dmain=SDL_main -Wall -Wextra -std=c++17 -MMD -MP

# Флаги линковки
LDFLAGS = $(PARALLEL_FLAGS) -lm -lpthread -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -mwindows

# Пути к заголовочным файлам
HEADERS = headers
HEADERS_SDL2 = /mingw64/include/SDL2
HEADERS_FFMPEG = /mingw64/include

# Пути к библиотекам
LIBS_DIR = /mingw64/lib

# Директории
SRCDIR = src
OUTDIR = build

# Файлы
SRC = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OUTDIR)/%.o, $(SRC))
DEPS = $(OBJS:.o=.d)

# Выходной файл
OUT = $(OUTDIR)/main.exe

# Цель по умолчанию
all: $(OUT)

# Компиляция .cpp → .o
$(OUTDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(OUTDIR)
	$(CXX) $(CXXFLAGS) -I$(HEADERS_SDL2) -I$(HEADERS_FFMPEG) -I$(HEADERS) -c $< -o $@

# Компоновка .o в .exe
$(OUT): $(OBJS)
	$(CXX) -g -o $(OUT) $(OBJS) -L$(LIBS_DIR) $(LDFLAGS)

# Включение зависимостей
-include $(DEPS)

# Очистка
clean:
	rm -rf $(OUTDIR)/*.exe $(OUTDIR)/*.o $(OUTDIR)/*.d

run:
	./build/main.exe -D MASTER_TEST
	./build/main.exe -D MASTER_TEST
	./build/main.exe -D MASTER_TEST
	./build/main.exe -D MASTER_TEST
	./build/main.exe -D MASTER_TEST
	./build/main.exe -D MASTER_TEST
	./build/main.exe -D MASTER_TEST
	./build/main.exe -D MASTER_TEST



