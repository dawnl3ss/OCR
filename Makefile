# Nom du fichier exécutable
TARGET = main

# Compilateur
CC = gcc

# Flags pour pkg-config (GTK+ 3.0)
LDFLAGS = `pkg-config --libs gtk+-3.0` -lSDL2 -lSDL2_image -lm
CFLAGS = `pkg-config --cflags gtk+-3.0`

# Fichiers sources
SRC = src/menu/menu.c src/image_processing/image_processing.c src/grid_detection/grid_detection.c src/rotate/rotate.c src/solver/solver.c

# Règle par défaut
all: $(TARGET)

# Règle pour construire l'exécutable
$(TARGET):
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

# Nettoyer les fichiers générés
clean:
	rm -f $(TARGET)
	rm -rf output/Word output/Grid
	rm -rf output/result/*
	rm output/grid.txt output/words.txt

# Règle pour recompiler
rebuild: clean all