CC := gcc
SRCD := src
TSTD := tests
BLDD := build
BIND := bin
INCD := include

ALL_SRCF := $(shell find $(SRCD) -type f -name *.c)
ALL_OBJF := $(patsubst $(SRCD)/%,$(BLDD)/%,$(ALL_SRCF:.c=.o))
FUNC_FILES := $(filter-out build/main.o, $(ALL_OBJF))
INC := -I $(INCD)

EXEC := project1

CFLAGS := -Wall -Werror
DFLAGS := -g -DDEBUG
INFOFLAGS := -DINFO
STD := -std=gnu11
TEST_LIB := -lcriterion

LIBS := -lreadline -pthread -ljansson

CFLAGS += $(STD)

.PHONY: clean all info

info: CFLAGS += $(INFOFLAGS)
info: all

debug: CFLAGS += $(DFLAGS)
debug: all

all: setup $(EXEC) $(TEST_EXEC)

setup:
	mkdir -p bin build out

$(EXEC): $(ALL_OBJF)
	$(CC) $^ -o $(BIND)/$@


$(TEST_EXEC): $(ALL_OBJF)
	$(CC) $(CFLAGS) $(INC) $(FUNC_FILES) $(TEST_SRC) $(TEST_LIB) -o $(BIND)/$@

$(BLDD)/%.o: $(SRCD)/%.c
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	$(RM) -r $(BLDD) $(BIND) out