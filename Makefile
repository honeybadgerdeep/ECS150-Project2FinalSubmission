# Target library
lib := libuthread.a

# Saving relevant directory names
SRC_DIR = .
OBJ_DIR = ./obj
LIB_DIR = .

# GCC compile flags as per assignment specs
CFLAGS = -Wall -Wextra -Werror

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Command used to build the library
LIB_BUILD = ar rcs

.PHONY: all
all: $(lib)

# Build the library file
$(lib): $(OBJS) | $(LIB_DIR)
	$(LIB_BUILD) $(lib) $@ $^

# Compile object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -o $@ -c $<

# Create object files directory
$(OBJ_DIR):
	mkdir -p $@

.PHONY: clean
clean:
	$(RM) -rv $(OBJ_DIR)
	$(RM) -v $(lib)