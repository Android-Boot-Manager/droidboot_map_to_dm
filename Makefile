CC = aarch64-linux-gnu-gcc
CFLAGS = -Wall -static

TARGET = droidboot_map_to_dm

SRC = main.c

OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJ) $(TARGET)

$(OBJ): $(SRC)
