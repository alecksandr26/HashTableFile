C = cc
C_FLAGS = -pedantic -Wall -ggdb
OBJDIR = obj
BINDIR = bin
SRCDIR = src
INCDIR = include
TESTDIR = test

# Create the list of objects
OBJS = $(addprefix $(OBJDIR)/, main.o dat.o map.o sat.o)

# Remove the main program
OBJS_TEST = $(filter-out $(OBJDIR)/main.o, $(OBJS))

# Create the list of bins
BIN = $(addprefix $(BINDIR)/, main.out)

# Create the lists of tests
TESTS = $(addprefix $(TESTDIR)/, test_sat.out test_dat.out test_map.out)

all: $(OBJDIR) $(BINDIR) $(TESTS) $(BIN)

$(OBJDIR):
	mkdir -p $@

$(BINDIR):
	mkdir -p $@

# Compile all the objects files
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(wildcard $(INCDIR)/*.h)
	@echo [Compiling: $< -o $@]
	@$(C) $(C_FLAGS) -c $< -o $@

# Compile the binary
$(BIN): $(OBJS)
	@echo [Linking: $(OBJS) -o $@]
	@$(C) $(C_FLAGS) $(OBJS) -o $@
	@echo [Finished]

run: $(BIN)
	@echo [Running: $(BIN)]
	@./$(BIN)

$(TESTDIR)/%.out: $(TESTDIR)/%.c $(OBJS_TEST)
	@echo [Linking: $< $(OBJS_TEST) -o $@]
	@$(C) $(C_FLAGS) $< $(OBJS_TEST) -o $@
	@echo [Finished]

test: $(TESTS)
	@echo [Running tests: $(TESTS)]
	@$(foreach t, $(TESTS), ./$(t);)
	@echo [Passed]

clean:
	@echo [Deleting: $(OBJS) $(TESTS) $(BIN)]
	@rm $(OBJS) $(TESTS) $(BIN)



