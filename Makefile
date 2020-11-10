# ---------------------------------
PROGRAM=perfectmono

HEADERS=text_render.h utf8.h
SOURCES=perfect_mono.c text_render.c

CC=gcc
CFLAGS=-I/usr/include/freetype2 -lGL -lGLU -lglut -lfreetype -lm
# ---------------------------------

OBJDIR=build
OBJ_FILES = $(patsubst %.c,$(OBJDIR)/%.o,$(SOURCES))

# Building
$(PROGRAM): $(OBJ_FILES)
	$(CC) -o $@ $^  $(CFLAGS)

# Create object directory present file-marker
$(OBJDIR)/.present:
	mkdir -p $(OBJDIR)
	touch $@

# Build rule per object file
$(OBJDIR)/%.o: %.c $(HEADERS) $(OBJDIR)/.present
	$(CC) -c -o $@ $< $(CFLAGS)

# Cleaning
.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o $(PROGRAM)

# Running
run: $(PROGRAM)
	./$(PROGRAM)
