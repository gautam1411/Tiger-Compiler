# Tiger-Compiler — Code Review & Fixes

This document summarizes the correctness issues and portability hazards found in the
Tiger-Compiler project, and the fixes that were applied. Severity is labelled as
**[Critical]** (showstopper, wrong behaviour or crash), **[High]** (latent crash,
undefined behaviour, or silent miscompile), **[Medium]** (code quality, warnings, or
behaviour that only matters in edge cases), and **[Low]** (cosmetic).

End-to-end verification after the fixes: `sum.tig`, `max.tig`, and `fact.tig` all
compile with `./tigerc` and produce correct output on `./tm` (sum `3 4` → `11`,
max `7 12` → `12`, fact `5` → `120`, fact `6` → `720`).

---

## 1. Scanner — EOF handling is broken on platforms where `char` is unsigned  [Critical]

**File**: `scan.c` (in `getToken()` and `lookahead()`)

**Before**:

```c
char c = getNextChar();
```

`getNextChar()` returns `int` (because it needs to carry `EOF == -1` out of band).
Storing that into a plain `char` truncates the high bits, and on any platform where
`char` is unsigned (ARM, PowerPC, many embedded toolchains) `EOF` becomes `0xFF` — a
valid character. The scanner never sees end-of-file and spins.

**After**:

```c
/* c must be int so EOF (-1) is preserved regardless of whether the
 * platform's `char` is signed or unsigned. */
int c = getNextChar();
```

The same change was applied in `lookahead()`.

---

## 2. Parser — argument list chaining drops all but the first two arguments  [Critical]

**File**: `parse.c`, function `Exp_list()`

**Before**: the loop wrote each new argument into `t->sibling`, unconditionally,
so the sibling chain never grew beyond length 2 regardless of how many arguments a
call had.

**After**: a `tail` pointer is maintained and each new argument is appended:

```c
TreeNode * Exp_list(void) {
  TreeNode *q = NULL, *tail = NULL;
  TreeNode *t = expr();
  tail = t;
  while (token != RPAREN && token != END && token != ENDFILE) {
    match(COMMA);
    q = expr();
    if (q != NULL && tail != NULL) { tail->sibling = q; tail = q; }
    else if (q != NULL)            { t = q; tail = q; }
  }
  return t;
}
```

---

## 3. Parser — NULL dereference on empty `let … in end`  [High]

**File**: `parse.c`, function `Let_exp()`

A `let ... in end` with no expressions in the body leaves `t->child[1] == NULL`,
but the old code walked `child[1]->sibling` to find the last expression and copy
its type, crashing.

**Fix**: guard the walk.

```c
temp = t->child[1];
if (temp != NULL) {
  while (temp->sibling != NULL) temp = temp->sibling;
  t->type = temp->type;
} else {
  t->type = ETYPE_NIL;
}
```

---

## 4. Parser — `match()` exits with status 0 on syntax errors  [Medium]

**File**: `parse.c`, function `match()`

On an unexpected token the parser called `exit(0)`, so shell callers (CI, Makefile
targets, `&&` pipelines) could not detect that compilation had failed. Changed to
`exit(1)`.

---

## 5. Semantic analysis — `break` check is tautologically true  [High]

**File**: `semant.c`, `BreakK` case of `transExp()`

**Before**:

```c
if (t->attr.loopaddr == NULL ||
    t->attr.loopaddr->expkind != ForK ||
    t->attr.loopaddr->expkind != WhileK)
  semantError("Break statement not within while or for statement", ...);
```

`(x != ForK) || (x != WhileK)` is *always* true (a value can't equal two distinct
enumerators at once), so a legitimate `break` inside `for`/`while` would still be
reported as an error — or, more commonly, the error was silently masked by other
control-flow bugs.

**After**:

```c
if (t->attr.loopaddr == NULL ||
    (t->attr.loopaddr->expkind != ForK &&
     t->attr.loopaddr->expkind != WhileK))
  semantError(...);
```

---

## 6. Semantic analysis — `for` loop variable goes into the wrong environment  [High]

**File**: `semant.c`, `ForK` case of `transDecs()`

The loop variable `i` in `for i := lo to hi do ...` was being entered into the type
environment (`tenv`) instead of the value environment (`venv`). Any reference to
`i` in the loop body then had to fall back to `tenv` to resolve, which only worked
accidentally in some code paths.

**Fix**: `S_enter(venv, s, enventry2)`.

---

## 7. Semantic analysis — NULL deref when resolving undeclared identifiers  [High]

**File**: `semant.c`, function `getNodeType()`

When a name wasn't found in `venv`, the function was called with the raw pointer
from `S_look(venv, ...)`, which is `NULL`, and then dereferenced.

**Fix**: the function is now null-safe and falls back to `tenv` before returning
the node's own `type` field:

```c
ExpType getNodeType(TreeNode *t, S_table venv, S_table tenv) {
  EnvEntry e;
  if (t == NULL) return ETYPE_NIL;
  if (t->expkind != IdK && t->expkind != LvalueK) return t->type;
  e = S_look(venv, S_Symbol(t->attr.id.name));
  if (e == NULL) e = S_look(tenv, S_Symbol(t->attr.id.name));
  if (e == NULL) return t->type;
  return e->type;
}
```

---

## 8. Semantic analysis — function parameter list is mutated in place  [Medium]

**File**: `semant.c`, `FUNCTIONCALL` type-check branch

Old code called `reverseList(enventry->u.params)` every time the function was used,
which mutated the canonical parameter list stored in the symbol table. After the
first call, subsequent calls saw the list in the wrong order and every type check
was bogus.

**Fix**: walk `enventry->u.params` directly (it's already in the intended order) and
pair it with the argument sibling chain without modification.

---

## 9. Semantic analysis — `:=` type check ignores the symbol table  [Medium]

**File**: `semant.c`, `AssignK`

Assignment was comparing `t->child[0]->type` to `t->child[1]->type`, but an `IdK`
node stores its environment-derived type in the symbol table, not on the node. The
comparison succeeded for the wrong reasons on well-typed programs and rejected some
well-typed ones.

**Fix**: route both sides through `getNodeType(..., venv, tenv)`.

---

## 10. TM simulator — off-by-one on address-bounds checks  [High]

**File**: `tm.c`

Two spots used `>` where they should have used `>=`, leaving the last cell of
instruction / data memory accessible out of bounds:

```c
/* before */                     /* after */
if (pc < 0 || pc > IADDR_SIZE)   if (pc < 0 || pc >= IADDR_SIZE)
if (m  < 0 || m  > DADDR_SIZE)   if (m  < 0 || m  >= DADDR_SIZE)
if (loc > IADDR_SIZE)            if (loc >= IADDR_SIZE)
```

---

## 11. TM simulator — `gets()` and `fflush(stdin)` are undefined behaviour  [High]

**File**: `tm.c`, `doCommand()` and `readInstructions()`

`gets()` was removed from the C standard in C11 (no bounds check), and
`fflush()` on an input stream is undefined behaviour. Both have been replaced with
`fgets(in_Line, LINESIZE, stdin)` plus newline trimming, and an EOF case that
cleanly quits the simulator rather than looping forever.

The `fgets()` in `readInstructions()` also had its return value silently ignored,
which causes a hard crash on a short/empty line — that's now handled too.

---

## 12. TM simulator — implicit-`int` on `main()`  [Medium]

**File**: `tm.c`

`main()` was declared without a return type, which is a hard error under C99+
(and a warning under gnu99). Now `int main(int argc, char *argv[])`.

---

## 13. `DEBUG` macro semantics contradicted themselves  [High, caused unusable output]

**Files**: `config.h`, `util.c`

`config.h` had `#define DEBUG FALSE` and `util.c` had a *second* redefinition
`# define DEBUG TRUE`. Every `#ifdef DEBUG` block in the codebase checks only
*presence* (`#ifdef`, not `#if`), so both definitions make DEBUG "on" at compile
time — every trace printf fired on every run, swamping the real output and making
`code.txt`/`tcode.tm` impossible to read.

**Fix**: removed both unconditional `#define DEBUG` lines. `DEBUG` is now a pure
presence macro, enabled only by `-DDEBUG` on the compiler command line (via
`make debug`).

---

## 14. Un-gated trace `printf`s throughout `parse.c`, `scan.c`, `icodegen.c`  [Medium]

Dozens of lines like `printf("File : %s Line : %d\n", __FILE__, __LINE__);` were
emitted unconditionally. They wrote to `stdout` — the same stream where the
compiler's user-facing messages live — making normal compilation output unreadable.

Every one of them has been wrapped in `#ifdef DEBUG` / `#endif`. They still fire
when you run `make debug`, but stay silent in a normal build.

---

## 15. Implicit-`int` declarations (C99+ rejects these)  [Low/Medium]

- `parse.c`: `extern TraceScan;` → `extern int TraceScan;`
- `util.c`: `indentno = 0;` at file scope → `static int indentno = 0;`
- `semant.c`: `static struct EnvEntry_ { ... };` had a useless `static` storage
  class on a struct *definition* (no variable); the `static` was removed.

---

## 16. `table.c` pointer-to-int casts are too narrow on 64-bit  [Medium]

Before: `((unsigned)key) % TABSIZE` — casting a 64-bit pointer to 32-bit
`unsigned` truncates and produces warnings.

After: `#include <stdint.h>` and `((unsigned)(uintptr_t)key) % TABSIZE`. This keeps
the hash stable and silences the warning under `-Wpointer-to-int-cast`.

---

## 17. `icodegen.c` `fprintf` with a stray argument  [Low]

`fprintf(code,"in:\n", hello);` passed an argument that had no matching format
specifier. Removed the stray argument.

---

## 18. `icodegen.c` IfK used the wrong temp index  [High]

The `IfK` code was emitting `if_false t<tloc> goto L<lloc>` where `tloc` was the
*next-available* temporary at the *start* of code generation for the condition —
not the temporary that actually held the condition result. Branches were comparing
against an uninitialized slot.

**Fix**: capture the return value of `genCode(tree->child[0])` (which is the temp
index where the condition ended up) and use that.

```c
temp2 = lloc;
temp1 = genCode(tree->child[0]);
fprintf(code, "if_false t%d goto L%d\n", temp1, lloc++);
```

---

## 19. Build system — no `Makefile`  [Medium]

The original project had no Makefile; every source file had to be compiled by
hand. Added `Makefile` with:

- `make` / `make all` — builds `tigerc` (the compiler) and `tm` (the simulator).
- `make debug` — `-DDEBUG -g -O0` for a chatty trace build.
- `make clean` — removes build artifacts and intermediate files.
- `make run-sum` / `make run-max` / `make run-fact` — convenience targets that
  compile the corresponding `.tig`, feed canned input to `tm`, and print the result.

CFLAGS suppress legacy-code warnings (`-Wno-implicit-function-declaration`,
`-Wno-format`, `-Wno-int-conversion`, `-Wno-deprecated-declarations`) so the build
is clean on modern GCC/Clang without requiring a wholesale rewrite.

---

## Summary of verification

```
$ make clean && make all
$ ./tigerc sum.tig && printf "g\n3\n4\nq\n"  | ./tm tcode.tm   # → 11
$ ./tigerc max.tig && printf "g\n7\n12\nq\n" | ./tm tcode.tm   # → 12
$ ./tigerc max.tig && printf "g\n12\n7\nq\n" | ./tm tcode.tm   # → 12
$ ./tigerc fact.tig && printf "g\n5\nq\n"    | ./tm tcode.tm   # → 120
$ ./tigerc fact.tig && printf "g\n6\nq\n"    | ./tm tcode.tm   # → 720
```

All four fixes-under-test produce the expected arithmetic output.

---

## Suggested follow-ups (not yet applied)

- Replace the single-translation-unit `#include "util.c"` / `#include "scan.c"` style
  with a real multi-file build (one `.o` per `.c`, linked together). It's the only
  thing preventing a standard `make -j` from working.
- Add a handful of regression tests under a `tests/` directory that run each sample
  `.tig`, pipe deterministic input to `tm`, and `grep` the output — easily wired
  into `make test`.
- The TM simulator conflates "the user's program talking to the user" with "the
  simulator talking to the user". Separate them (e.g. a `--batch` mode that auto-runs
  `g`, forwards stdin straight to the `IN` instruction, and then exits) so programs
  like `sum.tig` and `fact.tig` can be driven by a shell pipeline without the
  `g\n...\nq\n` dance.
- The symbol-table hash (`table.c`) truncates pointers to 32 bits and uses a tiny
  fixed 127-bucket table. Fine for toy programs, but worth noting.
- Error-recovery in the parser is effectively "exit on first error" — the
  commented-out `while (lineno == old_line) getToken()` block hints at a real
  recovery strategy that was abandoned. Worth finishing if you plan to type-check
  non-trivial programs.
