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
| 2 | Design on paper: state kept between calls, what each call does, when a line is complete | [ ] |
| 3 | Helpers (own string functions, only those the design needs), tested one by one | [ ] |
| 4 | Main function in stages, tested with several BUFFER_SIZE values and input types | [ ] |
| 5 | Hardening: edge cases, leaks, Norm, README with algorithm justification | [ ] |
| 6 | Bonus (only after the mandatory part is perfect) | [ ] |

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

- (none yet)

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
