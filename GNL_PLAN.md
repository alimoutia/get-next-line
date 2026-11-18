# get_next_line — working plan and progress

Subject version: 14.3 · Student: amoutia · Started: 2026-11-03

> Keep this file OUTSIDE the repository. Only the files the subject lists may be in the repo root.

## How we work

- Theory first. No project code until the phase before it is explained and understood.
- I write every line. Claude asks questions, reviews, and says bluntly what is wrong and where to look. No ready-made solutions.
- Every phase ends with a checkpoint: I explain it in my own words, Claude says what is missing.
- For every line I write I must be able to say why it is there (the evaluation can ask for a live modification).
- No direct answers from AI (subject, chapter III). The README must describe how AI was used.

## What the subject requires

### Function
- `char *get_next_line(int fd);`
- Returns the next line each call. Repeated calls read the file one line at a time.
- Returns `NULL` when there is nothing left to read, or on error.
- The line includes the final `\n`. Exception: the last line of a file that does not end with `\n` is returned without one.
- A line that is only `"\n"` is a valid line (not NULL).
- Must work on files AND on standard input. A fd is not always a regular file (stdin, pipes, ...).

### Files, functions, flags
- Turn in, at the repo root: `get_next_line.c`, `get_next_line_utils.c`, `get_next_line.h`.
- Header must at least contain the prototype. Helpers go in `get_next_line_utils.c`.
- Allowed external functions: `read`, `malloc`, `free`. Nothing else (no string functions: write my own helpers).
- Forbidden: libft, `lseek()`, global variables.
- Compile: `cc -Wall -Wextra -Werror -D BUFFER_SIZE=n <files>.c` (flag written without spaces).
- Must also compile and work WITHOUT `-D BUFFER_SIZE` (default value defined by me in the header).
- Must work for any BUFFER_SIZE: 1, 9999, 10000000, ... (the subject asks "do you know why?").
- Read as little as possible per call. Do not read the whole file and then split it.
- Undefined behavior (allowed): file modified between calls before EOF; binary files.

### General rules
- Norm applies. Check against the Norm document and `norminette` (functions max 25 lines, max 5 functions per file, limits on variables and parameters: verify).
- No crashes (segfault, double free, ...) except undefined behavior. No memory leaks.
- Check `read` errors and `malloc` failures.

### README (must be at repo root)
- First line, italic: *This project has been created as part of the 42 curriculum by amoutia.*
- Sections: Description, Instructions, Resources (references + how AI was used, for which tasks and parts).
- Extra requirement for this project: a detailed explanation and justification of the algorithm chosen.
- Not the same as libft's README.

### Bonus (only assessed if the mandatory part is perfect)
- Only ONE static variable.
- Manage several file descriptors at the same time, switching between them without losing the reading state of each.
- Files: `get_next_line_bonus.c`, `get_next_line_bonus.h`, `get_next_line_utils_bonus.c`.

### Evaluation
- Tests use very different buffer sizes and very different line lengths.
- A short live modification of the code may be requested.
- After validation, the function can be added to libft.

## Corrections from my first reading (2026-11-04)

1. The last line may come without `\n`. `NULL` also means read error. An empty line `"\n"` is not NULL.
2. A fd is not only a regular file. File extension is irrelevant: `read` sees bytes. `read` does not promise to fill the buffer.
3. The flag is `-D BUFFER_SIZE=42`, no spaces. I still need the "why" of 1, 9999, 10000000 (hint: where can a huge buffer live?).
4. Leftover data between calls is the core design, not an edge case: it happens with any BUFFER_SIZE above 1.
5. A static local keeps its value between calls and lives for the whole program. Visibility stays inside the function. Different from `static` on a function in libft.
6. Allowed functions are only read, malloc, free. Norm limits shape how the code is split in two `.c` files.
7. Bonus: how can ONE static variable remember several fds? (to think about later)
8. README requires an algorithm explanation: keep design notes as I go.

## Plan and progress

Status: [ ] not started · [~] in progress · [x] done and checkpoint passed

| Phase | What | Status |
| --- | --- | --- |
| 0 | Read the subject, checkpoint on understanding | [x] 2026-11-04 |
| 1 | Tools: `read()` behavior, static variables, where memory lives | [x] checkpoint passed 2026-11-07 |
| 2 | Design by prototyping in a scratch file, stage by stage (see "Prototype stages"). Short written expectation before each stage, explanation after | [~] started 2026-11-09 (paper-only mapping stalled: my counter-and-skip idea dropped the leftover) |
| 3 | Helpers (own string functions, only those the design needs), tested one by one | [ ] |
| 4 | Main function in stages, tested with several BUFFER_SIZE values and input types | [ ] |
| 5 | Hardening: edge cases, leaks, Norm, README with algorithm justification | [ ] |
| 6 | Bonus (only after the mandatory part is perfect) | [ ] |

### Prototype stages (phase 2 and onward, scratch file outside the repo)

Rule: before each stage write 2 or 3 lines of what I expect; after it, explain why it behaved that way.

Change on 2026-11-12: I chose to write a full draft in a practice file first, then refine. The stages below become the TEST ORDER for that draft (BUFFER_SIZE 1 first, then 5, 42, 10000000, then the edge cases). Conditions: a short written design goes with the draft (what is static, what is stored between calls, what each call does), and I explain each part before moving on. One function = one job, aiming at 25 lines or fewer, so the Norm split later is easy.

1. [x] (covered by the full draft, tested with BUFFER_SIZE 1) BUFFER_SIZE fixed at 1: read byte by byte until `\n` or EOF, return the line. No leftover needed yet. Questions first: what accumulates the line and where does it live? How do I append with only `malloc`/`free` (no `realloc`)? What ends the loop and what is returned for each ending? Tests: `hello\nworld\n`, a file without a final `\n`, an empty file.
2. [x] BUFFER_SIZE above 1: read overshoots the `\n` and data is lost. See it fail, then add the leftover (static pointer to heap data).
3. [~] Edge cases (done: empty file, no final `\n`, `\n\n`, one byte, long line, bad fd, directory, stdin; still open: `malloc` failure in the join path): empty file, last line without `\n`, only `\n`, `read` returning -1, memory freed on every path.
4. [ ] Split into helpers under the Norm (25 lines per function, 5 functions per file, only `read`/`malloc`/`free`).
5. [ ] Move to the repo files; test with several BUFFER_SIZE values, stdin, and `-fsanitize=address`.

### Phase 1 experiments

**A. `read` in chunks.** Throwaway program: open a short file, `read(fd, buf, 5)` in a loop, print each return value and the bytes.
- DONE 2026-11-07. Test file "hello world!!" (13 bytes), chunks of 5: returns were 5, 5, 3, then 0.
- Surprise: the third print showed `d!!rl`. The `rl` was stale data left in MY buffer from the previous chunk; `read` only wrote 3 bytes and never terminates the buffer.
- Answers: end of file -> returns 0 (one extra call is always needed to learn it). Error -> returns -1. After 0 or -1 the buffer must not be used. After data, only the first `ret` bytes are valid. The file position advances by the bytes returned, so the next call continues where the last stopped.
- Final program: buffer from `malloc(BUFFER_SIZE + 1)`, terminator placed at index `ret`, loop stops on `ret <= 0`, every exit path closes the fd and frees the buffer. Valgrind could not run on my machine (AVX-512 in the system loader), so I used `-fsanitize=address`.

**B. Call counter.** A function that returns how many times it was called, no globals. First with a plain local, then with `static`.
- DONE 2026-11-07. Plain local: prints 1, 1, 1 because a new variable is created and set to 0 at every call, and dies at return.
- `static`: counts 1, 2, 3. One variable with one fixed address for the whole program, initialized before `main` (zero -> `.bss`, nonzero -> `.data`). No assignment instruction runs at each call (checked in the assembly: the local version has `movl $0, -4(%rbp)`, the static version has none).

**Checkpoint 1:** explain `read` (return values, partial reads, position) and static variables (lifetime vs visibility, initialization) in my own words. PASSED 2026-11-07, with these notes: `read` can return fewer bytes than asked even when the file is not finished (pipes, terminals); on error it returns exactly -1 and the buffer contents are not guaranteed; the position moves forward with each successful read.

## Facts established in phase 1 (rely on these)

- `read` returns the number of bytes (1 to count), 0 at end of file, -1 on error. It does not add `\0`. The count is the truth; the rest of the buffer is garbage.
- Bytes already taken from the fd by `read` cannot be read again (no `lseek`), so anything read but not yet returned must be kept somewhere.
- A stack buffer fails for a huge BUFFER_SIZE (stack is about 8 MB: `ulimit -s`). A `malloc`ed buffer works.
- Heap data survives a return only if some pointer to it survives. An ordinary local pointer dies at return and the block is lost.
- A static local has one fixed address, is initialized once before `main`, keeps its value between calls, and is visible only inside its function. The initializer must be a constant.
- Whatever is acquired is released on every exit path (close the fd, free the buffer), in reverse order of acquisition.
- Norm reminders: no assignment inside a condition; declarations at the top of the function.

## Design decisions (fill in as they are made)

- One static pointer `left` to a heap string holding what was read but not yet returned (NULL at start).
- A helper that appends one chunk **consumes** its first argument: it frees the old `left` whether or not the join worked and returns the new string (NULL on failure). The caller assigns the result, frees `buf`, and returns NULL if the result is NULL. Reason: a failed join must not leak the old text, and must not let the loop carry on with a hole in the data.
- `ft_substr(s, 0, ft_strlen(s))` serves as a copy, so `ft_strdup` and `ft_memcpy` are not needed (keeps utils at 5 functions).

## Open questions

- What happens to the leftover when it contains no `\n`?
- (answered in phase 1: on the heap, because the stack is about 8 MB) Where must the buffer live for BUFFER_SIZE = 10000000?
- How does one static variable serve several fds (bonus)?
- What is freed, and when, so there is no leak at end of file or on error?

## Test list (grow it as I go)

- Empty file · one line with `\n` · one line without `\n` · only `"\n"` · many empty lines
- Last line without `\n` · very long line (longer than BUFFER_SIZE) · many short lines in one read
- BUFFER_SIZE 1, 2, 42, 9999, 10000000 · no flag
- stdin (terminal, pipe) · invalid fd · closed fd
- valgrind / ASan for leaks and invalid reads

## Progress log

- 2026-11-03: subject received, read by me and by Claude.
- 2026-11-04: answered the eight reading questions; corrections noted above. Plan agreed. Phase 1 started.
- 2026-11-05 to 2026-11-07: experiment A (read in chunks) done after several patches: stale bytes, off-by-one terminator, loop not stopping on -1, 10 MB stack crash, leaks on error paths. Experiment B (counter) done. Checkpoint 1 passed on 2026-11-07. Next: phase 2 (design on paper).
- 2026-11-09: paper mapping stalled (counter-and-skip idea cannot work: `read` consumes data and `lseek` is forbidden). Switched to prototyping.
- 2026-11-14: first full draft (array of chunks) crashed. Bugs found: write one past the end of the buffer, `arr` never allocated, `=` instead of `==`, chunks not terminated, `read` 0/-1 not handled, all chunks alias one buffer, wrong size arithmetic, leftover never freed. Decision: do not repair it; restart with a single accumulating string. Keep: static pointer to a heap leftover, own `ft_strlen`, the idea of cutting after the first `\n`.

### New design to write down before coding (five steps, own words)

1. Start from the stored leftover (static pointer, NULL on the first call).
2. While what I hold has no `\n`: `read` into a buffer; if the return is 0 or less, stop; terminate the chunk at the returned count; append it to what I hold (allocate new, copy, free old).
3. If I hold nothing: free, set the static to NULL, return NULL. If `read` returned -1: free everything and return NULL.
4. Line = up to and including the first `\n` (or everything if there is none). Leftover = everything after it in a new allocation, or NULL if empty. Free what I held and the buffer.
5. Test order: BUFFER_SIZE 1, 5, 42, 10000000, then the edge files, always with `-fsanitize=address`.

- 2026-11-17: the draft (static `left`, `read_file`, `make_line`, NULL-safe helpers, `temp` freed after each join) is correct and leak-free: 7 files x 6 buffer sizes (1, 2, 5, 42, 9999, 10000000) all identical to the input under AddressSanitizer; bad fds, a directory and stdin also fine. Known weakness: a very long single line with a small buffer is quadratic (200 KB line: 91 s at BUFFER_SIZE 1, 2.3 s at 42).
- Remaining before the repo: `malloc` failure inside the join path, Norm (max 5 functions per file: utils has 6; 25 lines; check commas with norminette), header file with guards and prototypes, `static` for internal functions, rename `gnl` to `get_next_line`, remove `main`, README with the algorithm explained, test with the subject's compile command (with and without `-D BUFFER_SIZE`).

- 2026-11-17 (evening): malloc-failure path fixed with the consuming helper. Kit test (`failtest.sh`: the Nth `malloc` fails, N = 1..80): old code 6/80 bad runs, first attempt at the helper (freed `left` only after a successful join, no NULL check in `read_file`) 12/80 at BUFFER_SIZE 1 and 3/80 at 5, final version 0/80 at BUFFER_SIZE 1, 5, 42 and 10000000. Lessons: a helper's contract (who frees what) must hold on the failure path too; an unused variable is a hard error under `-Werror`; `chmod +x` for downloaded scripts.
- Remaining before the repo: Norm (25 lines, 5 functions per file, check commas with norminette), header file with guards and prototypes, `static` for internal functions, rename `gnl` to `get_next_line`, remove `main`, README with the algorithm explained, test with the subject's compile command (with and without `-D BUFFER_SIZE`).
