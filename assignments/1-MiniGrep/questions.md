# Questions

Answer the following questions about your minigrep implementation:

## 1. Pointer Arithmetic in Pattern Matching

In your `str_match()` function, you need to check if the pattern appears anywhere in the line, not just at the beginning. Explain your approach for checking all possible positions in the line. How do you use pointer arithmetic to advance through the line?

> I use two pointers: one for the line and one for the pattern. For each line position, I compare characters using pointer increments. If a mismatch occurs, I reset the pattern pointer and advance the line pointer by one, checking all possible positions.


## 2. Case-Insensitive Comparison

When implementing case-insensitive matching (the `-i` flag), you need to compare characters without worrying about case. Explain how you handle the case where the pattern is "error" but the line contains "ERROR" or "Error". What functions did you use and why?

> I use tolower() on both the line and pattern characters before comparing. This ensures "ERROR" or "Error" matches "error" regardless of case.

## 3. Memory Management

Your program allocates a line buffer using `malloc()`. Explain what would happen if you forgot to call `free()` before your program exits. Would this cause a problem for:
   - A program that runs once and exits?
   - A program that runs in a loop processing thousands of files?

> If free() is omitted:
For a short-lived program, the OS reclaims memory, so no real issue.
For a long-running loop over many files, memory leaks accumulate and can cause crashes.

## 4. Buffer Size Choice

The starter code defines `LINE_BUFFER_SZ` as 256 bytes. What happens if a line in the input file is longer than 256 characters? How does `fgets()` handle this situation? (You may need to look up the documentation for `fgets()` to answer this.)

> fgets() reads up to 255 characters for a 256-byte buffer. Longer lines are split across multiple reads, which could affect pattern matching if the pattern crosses the split.

## 5. Return Codes

The program uses different exit codes (0, 1, 2, 3, 4) for different situations. Why is it useful for command-line utilities to return different codes instead of always returning 0 or 1? Give a practical example of how you might use these return codes in a shell script.

> Different exit codes let scripts detect specific outcomes. For example, 0 = matches found, 1 = no matches, 3 = file error. A script can respond differently based on the code.
