*This project has been created as part of the 42 curriculum by amoutia.*

# get_next_line

## Description

`get_next_line` is a C function that reads a file descriptor **one line at a time**.

```c
char	*get_next_line(int fd);
```

Each call returns the next line of `fd`, including its terminating `\n`. The last line of a file has no `\n` if the file does not end with one. When there is nothing left to read, or when an error happens, it returns `NULL`. It works on regular files and on standard input, and it is allowed to use only `read`, `malloc` and `free`.

The project teaches three things: how `read(2)` really behaves (it can return fewer bytes than asked, it never adds a `\0`, and consumed bytes cannot be read again), how a `static` variable keeps state between calls, and how to own every `malloc`'d block cleanly on every exit path.

The behaviour must not depend on `BUFFER_SIZE`, the number of bytes asked from `read` at a time. The code is correct from `BUFFER_SIZE=1` to `BUFFER_SIZE=10000000`.

### Files

| File | Content |
|---|---|
| `get_next_line.c` | `get_next_line` and its three static helpers (`read_file`, `read_helper`, `make_line`) |
| `get_next_line_utils.c` | `ft_strlen`, `ft_strchr`, `ft_substr`, `ft_strjoin` |
| `get_next_line.h` | include guard, includes, the `BUFFER_SIZE` default, prototypes |
| `*_bonus.c`, `*_bonus.h` | the same project with **several file descriptors at once** (see [Bonus](#bonus-several-file-descriptors)) |

## Instructions

Compile with the flags of the subject. `BUFFER_SIZE` can be set on the command line, and the header gives it a default (5) when it is not:

```sh
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c main.c
```

Bonus:

```sh
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line_bonus.c get_next_line_utils_bonus.c main.c
```

A minimal `main.c` that prints every line of a file:

```c
#include <fcntl.h>
#include <stdio.h>
#include "get_next_line.h"

int	main(void)
{
	int		fd;
	int		counter;
	char	*line;

	counter = 0;
	fd = open("file.txt", O_RDONLY);
	line = get_next_line(fd);
	while (line)
	{
		printf("Line N%d : %s", counter, line);
		free(line);
		line = get_next_line(fd);
	}
	close(fd);
	return (0);
}
```

The caller owns every returned line and must `free` it. For the bonus, include `get_next_line_bonus.h` instead.

## Algorithm

### The problem

`read` hands out bytes and forgets them: once read, they cannot be read again. A chunk of `BUFFER_SIZE` bytes rarely ends exactly where a line ends, so one call will often read **past the end of the line it must return**. Those extra bytes belong to the next call. They have to be kept somewhere that survives the return of the function.

### The design

A `static char *left` keeps the bytes that were read but not yet returned. The pointer is static (it lives for the whole program and starts as `NULL`); the string it points to lives on the heap. Each call does four things:

1. **Read until there is a line.** While `left` holds no `\n` and `read` has not reported the end of the file, read `BUFFER_SIZE` bytes into a heap buffer, terminate it with `\0`, and append it to `left` (`read_helper`, which builds a new string with `ft_strjoin` and frees the old `left`).
2. **Stop on `\n` or on EOF.** `read` returning `0` is the end of the file. `-1` is an error. A chunk smaller than `BUFFER_SIZE` is **not** the end of the file (pipes and terminals do this), so the loop is driven by the return value of `read`, never by the size.
3. **Cut.** `make_line` copies everything up to and including the first `\n` (or everything, if there is none) into a new string: that is the line to return.
4. **Keep the rest.** The remaining bytes become the new `left` for the next call.

When `left` is empty after reading, there is nothing left in the file: it is freed, set to `NULL`, and `NULL` is returned. Later calls keep returning `NULL`.

### Worked example

File `ab\ncdefgh\nij` (the last line has no `\n`), `BUFFER_SIZE=5`:

| Call | `read` returns | `left` after the cut | Returned |
|---|---|---|---|
| 1 | `ab\ncd` (5 bytes) | `cd` | `ab\n` |
| 2 | `efgh\n` (5 bytes) | `` (empty string) | `cdefgh\n` |
| 3 | `ij` (2 bytes), then `0` (EOF) | `` (empty string) | `ij` |
| 4 | `0` (EOF) | freed, `NULL` | `NULL` |

In call 3 the chunk `ij` is smaller than `BUFFER_SIZE` but the loop does not stop: only the following `read` returning `0` ends the file. Call 2 shows a line that ends exactly at the end of a chunk: the cut leaves an empty `left` and nothing extra was read. When a chunk contains several lines, the leftover already holds a `\n`, so the next call skips the read loop entirely.

### Why this design

- **A static leftover is forced by the problem.** A local variable would die at the `return`, and `read` cannot rewind (`lseek` is forbidden).
- **A heap buffer, not a stack array.** `BUFFER_SIZE` can be ten million, and the stack is about 8 MB. A `malloc` is limited by memory, not by the stack.
- **Reading chunks, not single characters.** One `read` per character is correct but needlessly slow. The chunk size is the knob the subject gives us.
- **One accumulating string.** Keeping a single `left` instead of a list of chunks makes the cut and the leftover simple and keeps the ownership rule easy: one pointer, one owner.

### Memory and errors

Every block has exactly one owner, and every exit path frees what it holds:

- `read_helper` **consumes** `left`: it frees it whether the join worked or not, and returns the new string (`NULL` on failure). The caller assigns the result and stops if it is `NULL`. A failed `malloc` therefore cannot leak the old text or lose a chunk silently.
- On a `read` error (`-1`) or a failed `malloc`, the buffer and `left` are freed, `left` is `NULL`, and `NULL` is returned. The next call starts clean.
- `ft_substr` returns an empty string, not `NULL`, when asked for a range that starts at or after the end: an empty string is a valid value (what is left after the last line); `NULL` always means failure.
- A negative `fd` or `BUFFER_SIZE < 1` returns `NULL` at once.

### Known limits

- **Speed on very long lines with a tiny buffer.** Each append copies the whole accumulated string and `ft_strchr` rescans it, so a line of *n* bytes read one byte at a time costs about *n*² operations. A 20 000-character line with `BUFFER_SIZE=1` still finishes in about a second. A fix would be to search for the `\n` only in the newest chunk.
- **Binary files and files modified while being read** are undefined behaviour, as the subject allows.

## Bonus: several file descriptors

The bonus keeps **one static variable**: an array of pointers, `static char *left[ARR_SIZE]`. The element `left[fd]` is the leftover of that file descriptor, and the fd number is the index. The algorithm above is unchanged: it simply works on `left[fd]` instead of `left`. Reading `fd 3`, then `fd 4`, then `fd 3` again therefore resumes each file where it stopped.

`ARR_SIZE` is defined once in `get_next_line_bonus.h` (default 1024, which is the usual default of `ulimit -n`). An fd that is negative or `>= ARR_SIZE` returns `NULL`, because indexing past the array would write outside of it.

Two behaviours to be aware of: an fd closed before its end and reopened under the same number inherits the old leftover, and an fd that is abandoned halfway keeps its leftover allocated (still reachable from the static array, so it is not a leak).

## Resources

- `man 2 read`, `man 3 malloc`, `man 3 free`: [read(2)](https://man7.org/linux/man-pages/man2/read.2.html)
- The 42 `get_next_line` subject
- [AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html) and Valgrind, for leaks and invalid memory access

### Use of AI

I used Claude as a tutor and reviewer, not as the author of the code. It helped me understand how `read` and `static` variables behave, pointed out the bugs in my drafts without fixing them for me, suggested the rule that the join helper consumes the old string. I wrote the six source files myself, tested them, and can explain every line.
