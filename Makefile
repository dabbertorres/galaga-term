CXX      := clang
CXX_ARGS := --std=c11 -O0 -g

BIN := galaga
SRC := main.c util.c term.c log.c
OBJ := $(SRC:%.c=%.o)
DEP := $(SRC:%.c=%.d)

$(BIN): $(OBJ)
	$(CXX) -o $@ $(CXX_ARGS) -MJ compile_commands.json $^ $(LDFLAGS)

include: $(DEP)

$(OBJ): %.o: %.c
	$(CXX) -c -o $@ $(CXX_ARGS) -MMD -MF $*.d -MJ compile_commands.json $<

.PHONY: clean
clean:
	@-rm $(BIN)
	@-rm $(OBJ)

