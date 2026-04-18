# Makefile for the Tiger-Compiler hobby project.
#
# The project is organized as a single translation unit: main.c
# transitively #includes every other .c file.  This keeps the build
# step simple and matches the original Turbo-C setup.
#
# Targets:
#     make              -- build both `tigerc` (compiler) and `tm` (VM)
#     make debug        -- build with verbose DEBUG tracing enabled
#     make clean        -- delete binaries and generated artifacts
#     make run-sum      -- compile sum.tig and run on the simulator
#     make run-max      -- compile max.tig and run on the simulator
#     make run-fact     -- compile fact.tig and run on the simulator

CC       ?= cc
CFLAGS   ?= -O2 -std=gnu99 -Wno-implicit-function-declaration \
            -Wno-format -Wno-int-conversion -Wno-deprecated-declarations
LDFLAGS  ?=

TIGERC_SRCS := main.c
# tm.c is a standalone translation unit -- its own main()
TM_SRCS     := tm.c

.PHONY: all debug clean run-sum run-max run-fact

all: tigerc tm

tigerc: $(TIGERC_SRCS) $(wildcard *.c) $(wildcard *.h)
	$(CC) $(CFLAGS) -o $@ $(TIGERC_SRCS) $(LDFLAGS)

tm: $(TM_SRCS)
	$(CC) $(CFLAGS) -o $@ $(TM_SRCS) $(LDFLAGS)

debug: CFLAGS += -DDEBUG -g -O0
debug: clean all

clean:
	rm -f tigerc tm tcode.tm code.txt scanop.txt parserop.txt

# Convenience targets that pipe sample input to the simulator.
run-sum: all
	./tigerc sum.tig
	@echo "3 4" | ./tm tcode.tm <<< "g$$'\n'q" || true

run-max: all
	./tigerc max.tig
	@printf "g\nq\n" | ./tm tcode.tm || true

run-fact: all
	./tigerc fact.tig
	@printf "g\nq\n" | ./tm tcode.tm || true
