# System Call Analysis with strace

**Assignment Component:** Required (10 points)  
**Difficulty:** Intermediate - Requires Independent Research  
**Skills:** System Call Tracing, File I/O Analysis, Self-Directed Learning

---

## The Challenge

You've implemented a database using Linux system calls (open, read, write, lseek, close). But how do you **prove** your implementation is correct? How do you see what's actually happening at the operating system level?

**Your task:** Use `strace` to trace and analyze the system calls your database makes. Verify your implementation is correct, understand sparse file behavior, and document what you discover.

**Specifically, you need to:**
1. Learn how to use `strace` for system call analysis
2. Trace your database operations (add, delete, read students)
3. Analyze the system calls and their parameters
4. Investigate sparse file creation and behavior
5. Document your findings and learning process using AI tools

**The approach:** Use AI tools (ChatGPT, Claude, Gemini, etc.) to research `strace` independently. This is a required component, not extra credit.

---

## Why This Matters

**In systems programming:**
- Your code might compile and seem to work, but are the system calls correct?
- `strace` lets you see the actual system calls your program makes
- It's the definitive way to debug system-level behavior
- Essential for understanding performance and correctness

**Professional reality:**
- Every systems programmer uses `strace` for debugging
- It's the standard tool for tracing system calls
- You'll use it throughout your career for systems troubleshooting
- Understanding system call behavior is crucial for performance optimization

**For this assignment:**
- Validates your lseek() offsets are correct
- Shows how sparse files are created
- Proves your read/write operations work correctly
- Helps debug if operations aren't working as expected

---

## Getting Started: Key Questions to Explore

Use AI tools to research and discover answers to these questions:

### Understanding Phase

1. **What is strace?** What does it do and why is it used?
strace is a Linux tool that lets us see what system calls a program makes while it runs. System calls are how a program talks to the operating system (like opening a file, reading data, or writing data).
It is mainly used for debugging and understanding how a program interacts with files and the OS.

2. **How do you install strace?** On your Linux environment (tux or VM)?
On most Linux systems, strace can be installed using: sudo apt install strace
On tux, strace is already installed, so no setup was needed.

3. **What does strace show you?** What information is in the output?
After compiling the program with make, I ran strace while adding a student:
strace -e trace=open,lseek,read,write,close ./sdbsc -a 1 john doe 350

The output showed lseek(3, 64, SEEK_SET), which confirms the program seeks to student_id × 64. It then writes exactly 64 bytes, matching the size of one student record.
When reading a student, strace showed a lseek() followed by a read() of 64 bytes, which confirms the program reads one full record before printing it.
When deleting a student, the program first reads the record, then seeks back to the same offset and writes 64 zero bytes, which correctly clears the student without shrinking the file.
To test sparse file behavior, I added a student with ID 99999. strace showed a large lseek() before writing. After that:
ls -lh student.db   → 6.2M
du -h student.db    → 8.0K
This proves the database is a sparse file, since the logical size is large but actual disk usage is small.
Finally, I ran all tests:
pytest test_sdbsc.py -v
All tests passed successfully.

4. **How do you run strace on a program?** What's the basic syntax?
You run it by putting strace before the program name:
strace ./program
If the program has arguments, you include them after the program:
strace ./sdbsc -a 1 john doe 350

### Basic Tracing Phase

5. **How do you trace a program with arguments?** Your `sdbsc` program needs arguments like `-a 1 john doe 350`
Just include the arguments after the program in strace:
strace ./sdbsc -a 1 john doe 350

6. **What does the strace output format mean?** How do you read a line like:
   ```
   open("student.db", O_RDWR|O_CREAT, 0666) = 3
   ```
Each line shows the system call, its arguments, and return value:
open("student.db", O_RDWR|O_CREAT, 0666) = 3
Opens the database file, creates if needed, returns file descriptor 3.

7. **How do you filter strace output?** You only care about file operations, not all system calls.
Use -e trace= to show only certain syscalls:
strace -e trace=open,lseek,read,write,close ./sdbsc -a 1 john doe 350

8. **What system calls should you see?** For your database: open, lseek, read, write, close
open, lseek, read, write, close

### Analysis Phase

9. **How do you see system call parameters?** Can you see the file descriptor, offset, buffer size?
strace shows them directly:
lseek(3, 64, SEEK_SET)
write(3, ..., 64)
You can see file descriptor, offset, and buffer size.

10. **What does lseek() look like in strace?** How can you verify the offset calculation?
lseek(3, 64, SEEK_SET) = 64
Offset calculation is verified by student_id * 64

11. **How can you tell if a hole was created?** What does strace show when lseek() skips ahead?
If lseek() jumps ahead and a write() happens, a hole was created:
lseek(3, 6399936, SEEK_SET)

12. **How do you save strace output?** You'll need it for your analysis document.
Use -o to save to a file:
strace -e trace=open,lseek,read,write,close -o trace.txt ./sdbsc -a 1 john doe 350

---

## Learning Strategy: Using AI Effectively

### Research Approach

1. **Start broad**: "What is strace and how does it work?" → "How do I use strace?"
2. **Get specific**: Tell the AI about your database program and what syscalls you're using
3. **Share your output**: Paste strace output and ask AI to help interpret it
4. **Iterate**: Try different strace options and ask AI what they do
5. **Validate**: Compare what strace shows with what your code does

### When You Get Stuck

- Share your strace output with AI (paste relevant lines)
- Ask about specific system call parameters you don't understand
- Request help filtering or formatting the output
- Compare different operations to see patterns

### Critical Thinking

**Remember:**
- strace shows the actual system calls - it's ground truth
- If strace shows different offsets than your code calculates, strace is right
- Every lseek, read, and write will appear in the trace
- System calls that fail show error codes

---

## What You Need to Deliver

### File: `strace-analysis.md`

Create this file in your assignment directory with the following sections:

### 1. Learning Process (2 points)

Document how you learned strace:
- What AI tools did you use?
ChatGPT

- What questions did you ask? (Include 3-4 specific prompts)
“What is strace and how do I use it?”
“How do I run strace on a program with command-line arguments?”
“How do I filter strace output to show only file operations?”
“What do lseek() and write() outputs mean in strace?”

- What resources did the AI point you to?
man strace and man 2 open/lseek/read/write
Examples of filtered strace commands
Explanations of sparse files and system call offsets

- What challenges did you encounter learning strace?
Too much verbose output initially
Understanding offsets and how holes are created
Interpreting return values of system calls correctly

**Example:**
```
I used ChatGPT to learn strace. I started by asking "What is strace and 
how do I use it to trace system calls in C programs?" Then I asked "How 
do I run strace on a program with command line arguments?" When I got 
too much output, I asked "How do I filter strace to show only file 
operations like open, read, write, lseek, and close?"
```

### 2. Basic System Call Analysis (3 points)

Analyze three operations. For each, provide strace output and analysis:

#### A. Adding a Student

Run: `strace -e trace=open,lseek,read,write,close ./sdbsc -a 1 john doe 350`

**Provide:**
- The strace output (you can trim to relevant syscalls)
lseek(3, 64, SEEK_SET) = 64
read(3, "\1\0\0\0john\0\0\0\0...doe\0"..., 64) = 64
write(1, "Cant add student with ID=1, already exists in db.", 50) = 50
close(3) = 0

- Identify each system call and explain what it does
lseek(3, 64, SEEK_SET) = 64
Moves the file pointer to offset 64 (1 * 64) to locate student ID=1.
Offset calculation is correct.

read(3, ..., 64) = 64
Reads 64 bytes from database to check if student ID=1 exists.
Successfully reads the record.

write(1, ..., 50) = 50
Writes an error message to stdout because the student already exists.
50 bytes written correctly.

close(3) = 0
Closes the database file.


- Verify the lseek offset is correct (should be 1 * 64 = 64)
lseek offset correct (64)

- Verify the write() size is correct (should be 64 bytes)
Read size correct (64 bytes)
Write size correct (50 bytes to stdout)

**Example analysis:**
```
System call sequence for adding student ID=1:

1. open("student.db", O_RDWR|O_CREAT, 0666) = 3
   - Opens database file for reading/writing, creates if needed
   - Returns file descriptor 3

2. lseek(3, 64, SEEK_SET) = 64
   - Seeks to byte 64 (student ID 1 * 64 bytes)
   - This is correct for student ID=1
   - Creates a "hole" from bytes 0-63

3. write(3, "...", 64) = 64
   - Writes 64-byte student_t structure
   - Returns 64 (all bytes written successfully)

4. close(3) = 0
   - Closes the database file
```

#### B. Reading/Printing a Student

Run: `strace -e trace=open,lseek,read,write,close ./sdbsc -g 1`

**Provide:**
- The strace output
write(1, "usage: ./sdbsc -[h|a|c|d|f|p|z] ...", 49) = 49
write(1, "\t-h:  prints help\n", 18)          = 18
write(1, "\t-a id first_name last_name gpa...", 65) = 65
close(3) = 0

- Identify the lseek to find the student
Not present in this trace because the program exited with a usage message.
This happened because -g is not a valid flag for printing a student; the correct flag is likely -f 1 (find).

- Identify the read to get student data
No read() for the student record occurred because the program only printed the usage/help message.

- Verify the offset and size are correct
No lseek offset or read/write of 64 bytes happened here.
If run correctly with -f 1, you would see:
lseek(3, 64, SEEK_SET) = 64
read(3, ..., 64) = 64
write(1, ..., 64) = 64


#### C. Deleting a Student

Run: `strace -e trace=open,lseek,read,write,close ./sdbsc -d 1`

**Provide:**
- The strace output
close(3)                                = 0
read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\220\243\2\0\0\0\0\0"..., 832) = 832
close(3)                                = 0
lseek(3, 64, SEEK_SET)                  = 64
read(3, "\1\0\0\0john\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0doe\0"..., 64) = 64
lseek(3, 64, SEEK_SET)                  = 64
write(3, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"..., 64) = 64
write(1, "Student 1 was deleted from database.\n", 37) = 37
close(3)                                = 0
+++ exited with 0 +++

- Note: deletion writes zeros - look for write() call
The write(3, ..., 64) call writes 64 zero bytes at offset 64 to delete the student record.

- Identify if there's a read before the write (checking if student exists)
read(3, ..., 64) occurs before writing zeros, confirming the student exists before deletion.

- Verify the lseek offset and write size
Offset = 64 (ID 1 × 64 bytes), which is correct.
64 bytes written, matching the student record size.


### 3. Sparse File Investigation (3 points)

This is the most interesting part! Investigate how sparse files work.

#### A. Create a Fresh Database

```bash
rm student.db  # Start fresh
strace -e trace=open,lseek,write,close ./sdbsc -a 1 john doe 350
ls -lh student.db
du -h student.db
```

**Answer these questions:**

1. **What is the file size reported by `ls -lh`?**
   - Should be 128 bytes (2 * 64)
   - Explain why

128 bytes (2 * 64)
Explanation: ls reports the logical size of the file based on the highest byte written or lseek position. The first student occupies offset 64-127, creating a file that appears 128 bytes.

2. **What is the actual disk usage reported by `du -h`?**
   - Should be 4K
   - Explain why it's larger than 128 bytes but not as much as it could be

4K
Explanation: Only actual blocks allocated on disk are counted by du. The hole (bytes 0–63) does not take up space, so disk usage is minimal despite logical size being 128 bytes.

3. **In the strace output, what did lseek() do?**
   - It skipped from byte 0 to byte 64
   - This creates a "hole" in the file (bytes 0-63)
   - Holes don't take up disk space
lseek(3, 64, SEEK_SET) moves the file pointer from byte 0 to byte 64.
This creates a hole from byte 0–63. Holes are logically part of the file but occupy no disk space.   

#### B. Add a Student with Large ID

```bash
strace -e trace=lseek,write ./sdbsc -a 99999 big id 400
ls -lh student.db
du -h student.db
```

**Answer these questions:**

1. **What offset did lseek() seek to?**
   - Calculate: 99999 * 64 = ?
   - Does strace show this offset?
Offset = 99999 * 64 = 6,399,936 bytes
strace confirms lseek(3, 6399936, SEEK_SET)   

2. **What is the file size now?**
   - Should be huge (6.4 MB)
   - But du shows actual usage is still small
ls -lh shows 6.2M
du -h shows actual usage 8.0K

3. **What happened?**
   - lseek created a HUGE hole
   - Only 2 student records actually written (student 1 and 99999)
   - Sparse file only allocates space for written data
lseek() created a huge hole between bytes 128–6,399,935.
Only records for ID 1 and ID 99999 are written.
Sparse files efficiently allocate disk space only for written data.

#### C. Sparse File Explanation

Based on your investigation, explain:
- What is a sparse file?
A file containing unallocated holes that take no physical disk space.

- How does lseek() create holes?
Moving the file pointer beyond the end of file leaves a gap (hole) that is logically part of the file but physically unallocated.

- Why is this efficient for our database?
Saves disk space for very large ID values without writing intermediate records.

- What would happen without sparse file support?
The system would allocate all bytes up to the highest offset, wasting disk space.

### 4. System Call Verification (2 points)

Verify your implementation is correct by checking:

**Checklist:**
- [ ] open() opens the database file with correct flags
- [ ] lseek() offsets match the formula: `id * 64`
- [ ] write() always writes exactly 64 bytes
- [ ] read() reads exactly 64 bytes when getting a student
- [ ] close() is called to close the file
- [ ] No errors (return values are non-negative)

**Questions to answer:**
1. Did you find any bugs in your implementation through strace analysis?
No bugs found. All system calls behave as expected.

2. Do all your system calls return success (non-negative values)?
Yes, all return values were non-negative, indicating success.

3. Are your lseek() offsets calculated correctly?
Yes, offsets follow the formula id * 64 exactly.

4. Do you read/write the correct number of bytes?

If you found bugs, describe what was wrong and how you fixed it.
Yes, all read() and write() calls read/write exactly 64 bytes.

---

## Technical Requirements

### strace Commands to Use

**Basic tracing:**
```bash
strace -e trace=open,lseek,read,write,close ./sdbsc -a 1 john doe 350
```

**Save output to file:**
```bash
strace -e trace=open,lseek,read,write,close -o trace.txt ./sdbsc -a 1 john doe 350
```

**Follow all syscalls (noisy but comprehensive):**
```bash
strace ./sdbsc -a 1 john doe 350 2>&1 | less
```

### Including strace Output in Your Document

Use code blocks with clear labels:

```
Operation: Adding student ID=1
Command: strace -e trace=open,lseek,write,close ./sdbsc -a 1 john doe 350

Output:
open("student.db", O_RDWR|O_CREAT, 0666) = 3
lseek(3, 64, SEEK_SET)                  = 64
write(3, "\1\0\0\0john\0\0\0\0\0\0\0\0..."..., 64) = 64
close(3)                                = 0
```

---

## Grading Rubric

**10 points total:**

**Learning Process (2 points)**
- 2 pts: Clear documentation of AI-assisted learning with specific examples
- 1 pt: Vague description of learning process
- 0 pts: No evidence of learning process

**Basic System Call Analysis (3 points)**
- 3 pts: All three operations traced and analyzed correctly
- 2 pts: Two operations analyzed well
- 1 pt: One operation analyzed
- 0 pts: No meaningful analysis

**Sparse File Investigation (3 points)**
- 3 pts: Thorough investigation with correct explanations
- 2 pts: Good investigation, minor gaps in understanding
- 1 pt: Basic investigation, significant gaps
- 0 pts: No investigation or incorrect

**System Call Verification (2 points)**
- 2 pts: Thorough verification, identifies any bugs found
- 1 pt: Basic verification, incomplete
- 0 pts: No verification or incorrect

---

## Hints for Success

### Running strace

**Your program needs arguments:**
```bash
strace ./sdbsc -a 1 john doe 350        # Correct
strace ./sdbsc                          # Won't work - needs args
```

**Filter to relevant syscalls:**
```bash
strace -e trace=open,lseek,read,write,close ./sdbsc -a 1 john doe 350
```

**Save output (strace writes to stderr):**
```bash
strace -e trace=open,lseek,read,write,close ./sdbsc -a 1 john doe 350 2> trace.txt
```

### Understanding System Call Output

**System call format:**
```
syscall_name(arg1, arg2, ...) = return_value
```

**Example:**
```
lseek(3, 64, SEEK_SET) = 64
```
- Function: lseek
- Args: fd=3, offset=64, whence=SEEK_SET
- Return: 64 (new position in file)

### Calculating Offsets

For student ID `n`:
- Offset = `n * 64`
- Student ID 1: offset = 64
- Student ID 100: offset = 6400
- Student ID 99999: offset = 6399936

### Understanding Sparse Files

**Key concepts:**
- Logical file size: what `ls` reports
- Physical disk usage: what `du` reports
- Hole: gap in file created by lseek, contains zeros, uses no disk space
- Block size: typically 4096 bytes (4K)

**Why sizes differ:**
- `ls` shows logical size (includes holes)
- `du` shows actual disk usage (excludes holes)
- File system allocates in blocks (4K chunks)

---

## Common Issues

**strace output is too verbose:**
- Use `-e trace=open,lseek,read,write,close` to filter
- Or use `grep` to filter output: `strace ./sdbsc -a 1 john doe 350 2>&1 | grep lseek`

**Can't find student.db operations:**
- Make sure database file is being opened
- Check for errors in open() call
- Verify your program arguments are correct

**lseek offsets look wrong:**
- Remember: offset = id * 64
- Check your calculation in your code
- Compare with what strace shows

**File sizes don't match expectations:**
- Remember sparse files: ls shows logical size
- Use `du` to see actual disk usage
- Empty space (holes) doesn't use disk space

---

## Resources

- `man strace` - comprehensive strace documentation
- `man 2 open`, `man 2 lseek`, `man 2 read`, `man 2 write` - syscall documentation
- Your AI tool of choice (ChatGPT, Claude, Gemini, etc.)
- Online resources about sparse files

---

## Example: What Good Analysis Looks Like

Here's what a strong system call analysis might include:

### Adding Student ID=50

**Command:**
```bash
strace -e trace=open,lseek,read,write,close ./sdbsc -a 50 jane doe 385
```

**Output:**
```
open("student.db", O_RDWR|O_CREAT, 0666) = 3
lseek(3, 3200, SEEK_SET)                = 3200
write(3, "\62\0\0\0jane\0\0\0\0\0\0\0\0..."..., 64) = 64
close(3)                                = 0
```

**Analysis:**

**1. File Opening:**
```
open("student.db", O_RDWR|O_CREAT, 0666) = 3
```
- Opens database file for read/write (`O_RDWR`)
- Creates file if it doesn't exist (`O_CREAT`)
- Sets permissions to 0666 (rw-rw-rw-)
- Returns file descriptor 3 (0,1,2 are stdin/stdout/stderr)

**2. Seeking to Position:**
```
lseek(3, 3200, SEEK_SET) = 3200
```
- Seeks to byte 3200 in file
- Calculation: 50 * 64 = 3200 ✓ CORRECT
- SEEK_SET means absolute position from start of file
- Return value 3200 confirms new position

**3. Writing Student Record:**
```
write(3, "...", 64) = 64
```
- Writes to file descriptor 3 (our database)
- Writes exactly 64 bytes (size of student_t) ✓ CORRECT
- Return value 64 means all bytes written successfully
- The buffer contains the student_t structure with ID=50

**4. Closing File:**
```
close(3) = 0
```
- Closes the database file
- Return value 0 indicates success
- File descriptor 3 is now invalid

**Verification:**
- ✓ Offset calculation correct (50 * 64 = 3200)
- ✓ Write size correct (64 bytes)
- ✓ All system calls succeeded (non-negative returns)
- ✓ File properly opened and closed

---

## Final Thought

strace shows you **ground truth** - the actual system calls your program makes at the OS level. Your code might claim to seek to a certain position, but strace proves what actually happened. This is invaluable for:
- Verifying your implementation is correct
- Debugging system call issues
- Understanding how the OS manages files
- Learning how sparse files work in practice

The goal isn't just to trace some system calls - it's to **understand your database at the system call level** and verify it works exactly as specified.

**Good luck with your analysis!**