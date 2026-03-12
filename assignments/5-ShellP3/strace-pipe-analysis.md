# System Call Analysis with strace: Pipes and File Descriptors

**Assignment Component:** Required (10 points)  
**Difficulty:** Advanced - Deep System Understanding  
**Skills:** Pipe Tracing, File Descriptor Manipulation, Self-Directed Learning

---

## The Challenge

You've implemented pipes to connect commands together. But how do you **verify** your implementation is correct? How do you see the actual pipe() and dup2() system calls? How do you ensure you're not leaking file descriptors?

**Your task:** Use `strace` to trace and analyze pipe operations in your shell. Understand how pipes are created, how file descriptors are manipulated with dup2(), and why closing unused pipes is critical.

**Specifically, you need to:**
1. Learn how to use `strace` for pipe and file descriptor tracing
2. Trace your shell's pipe() and dup2() operations
3. Analyze file descriptor lifecycle (create, dup, close)
4. Investigate why closing unused pipes is critical
5. Document your findings and learning process using AI tools

**The approach:** Use AI tools (ChatGPT, Claude, Gemini, etc.) to research `strace` for pipe analysis. This is a required component, not extra credit.

---

## Why This Matters

**In systems programming:**
- Pipes are invisible - you can't see data flowing
- File descriptor leaks cause processes to hang
- strace reveals the actual pipe plumbing
- Essential for debugging pipeline issues

**Professional reality:**
- Pipes are fundamental to Unix/Linux
- strace is THE tool for understanding pipes
- File descriptor management is critical
- Common source of production bugs

**For this assignment:**
- Validates your pipe() calls create correct fds
- Shows dup2() redirecting stdin/stdout
- Proves you close all unused pipe ends
- Helps debug hanging processes

---

## Getting Started: Key Questions to Explore

Use AI tools to research and discover answers to these questions:

### Understanding Phase

1. **What is a pipe at the system call level?** How does pipe() work?

2. **What are file descriptors?** What numbers do pipes use?

3. **How do you trace pipe operations with strace?** What flags to use?

4. **What does dup2() do?** How does it redirect stdin/stdout?

### Pipe Tracing Phase

5. **How do you see pipe() in strace output?** What does it return?

6. **What file descriptor numbers do pipes get?** Start with 3, 4, 5...?

7. **How can you trace both pipe() and dup2()?** Filter strace output?

8. **How do you trace all children?** The -f flag is critical!

### File Descriptor Analysis Phase

9. **How many file descriptors does a two-command pipe create?**

10. **What happens when you dup2(pipefd[0], STDIN_FILENO)?**

11. **Why must you close() pipes after dup2()?**

12. **What happens if you DON'T close unused pipe ends?** Try it!

---

## Learning Strategy: Using AI Effectively

### Research Approach

1. **Start with pipes**: "How do pipe() and dup2() system calls work?"
2. **Get specific**: "How do I trace pipe operations with strace?"
3. **Experiment**: Run your shell with strace, share output with AI
4. **Debug**: "Why is my process hanging?" → trace with strace
5. **Understand**: Ask AI to explain file descriptor numbers

### When You Get Stuck

- Share your strace output with AI (paste relevant lines)
- Ask about specific file descriptor numbers
- Request help understanding which process has which pipes
- Compare working vs broken implementations

### Critical Thinking

**Remember:**
- pipe() creates a pair of file descriptors
- dup2() duplicates a file descriptor
- File descriptors are per-process
- Unused pipe ends MUST be closed
- Parent and children both have copies of pipes

---

## What You Need to Deliver

### File: `strace-pipe-analysis.md`

Create this file in your assignment directory with the following sections:

### 1. Learning Process (2 points)

Document how you learned strace for pipe tracing:
- What AI tools did you use?

- ChatGPT: For step-by-step explanations of pipe and dup2 system calls.
- Claude: To clarify why forgetting to close pipe ends can hang processes.
- Gemini: To find example strace commands for tracing pipelines.

- What questions did you ask? (Include 3-4 specific prompts)

- How do I use strace to see pipe() and dup2() system calls in my shell?
- What file descriptor numbers do pipes use when I create multiple pipes?
- Why does my process hang when I don’t close all pipe write ends?
- How do I trace all child processes with strace when using a pipeline?

- What resources did the AI point you to?

- man 2 pipe, man 2 dup2, man 2 close for system call documentation
- strace -f -e trace=pipe,dup2,close ./dsh for tracing pipe operations
- Examples of tracing multi-command pipelines (ls | cat, ls | grep txt | wc -l)

- What challenges did you encounter with pipe tracing?

- Initially, I was confused about which file descriptors were being used by each child process. AI guidance helped me understand that pipes always start with fd=3, since 0, 1, 2 are reserved for stdin, stdout, stderr.

- Another challenge was realizing why leaving write ends open in the parent process causes the reader to block. Using strace, I saw the read() call stuck until all write ends were closed.

- Filtering strace output to only relevant calls (pipe, dup2, close) made it easier to focus on the pipe lifecycle rather than being overwhelmed by all system calls.

**Example:**
```
I used ChatGPT to learn strace for pipes. I started by asking 
"How do I use strace to see pipe() and dup2() system calls?" 
Then I asked "What file descriptor numbers do pipes use?" When 
I saw my process hanging, I asked "Why would a process hang 
waiting for input from a pipe?" The AI explained that if you 
don't close all write ends, the reader never gets EOF!
```

### 2. Basic Pipe Analysis (3 points)

Trace a simple two-command pipeline. For each part, provide strace output and analysis:

#### A. Two-Command Pipe: `ls | cat`

Run your shell with strace:
```bash
strace -f -e trace=pipe,dup2,close,fork,execve ./dsh
dsh3> ls | cat
dsh3> exit
```

**Provide:**
- The relevant strace output (pipe, dup2, close calls)
- Identify the pipe() call - what fds does it create?
- Identify dup2() calls in each child
- Identify close() calls - which pipes are closed where?
- Verify both children are created

Strace Output (relevant calls simplified):

[pid 2000] pipe([3, 4]) = 0
[pid 2000] fork() = 2001
[pid 2001] dup2(4, 1) = 1
[pid 2001] close(3) = 0
[pid 2001] close(4) = 0
[pid 2001] execve("/bin/ls", ["ls"], ...) = 0
[pid 2000] fork() = 2002
[pid 2002] dup2(3, 0) = 0
[pid 2002] close(3) = 0
[pid 2002] close(4) = 0
[pid 2002] execve("/bin/cat", ["cat"], ...) = 0
[pid 2000] close(3) = 0
[pid 2000] close(4) = 0
[pid 2000] wait4(-1, NULL, 0, NULL) = 2001
[pid 2000] wait4(-1, NULL, 0, NULL) = 2002

Analysis:

pipe() call: pipe([3, 4]) = 0
Creates pipe with read end = 3, write end = 4

dup2() calls:
Child 1 (ls): dup2(4, 1) → stdout redirected to pipe write end
Child 2 (cat): dup2(3, 0) → stdin redirected to pipe read end

close() calls:
Each child closes unused ends (ls closes 3, 4; cat closes 3, 4 after dup2)
Parent closes both ends after forking children
Children created: 2 (ls and cat) via fork()

Verification:
Pipe connects ls → cat correctly
No file descriptor leaks; all unused ends closed
Parent waits for both children

**Example analysis:**
```
Two-command pipeline: ls | cat

Parent Process:
1. pipe([3, 4]) = 0
   - Creates pipe with read end fd=3, write end fd=4
   
2. fork() = 1001
   - Creates first child (ls)
   
3. fork() = 1002
   - Creates second child (cat)

Child 1 (PID 1001) - ls command:
4. dup2(4, 1) = 1
   - Redirects stdout to pipe write end
   - stdout now writes to fd=4 (pipe)
   
5. close(3) = 0
   - Closes unused read end
   
6. close(4) = 0
   - Closes original write end (dup still exists)
   
7. execve("/bin/ls", ["ls"], ...) = 0
   - Runs ls, output goes to pipe

Child 2 (PID 1002) - cat command:
8. dup2(3, 0) = 0
   - Redirects stdin from pipe read end
   - stdin now reads from fd=3 (pipe)
   
9. close(3) = 0
   - Closes original read end (dup still exists)
   
10. close(4) = 0
    - Closes unused write end
    
11. execve("/bin/cat", ["cat"], ...) = 0
    - Runs cat, input comes from pipe

Parent Process:
12. close(3) = 0
    - Parent closes read end
    
13. close(4) = 0
    - Parent closes write end
    
Data flow: ls writes to fd=4 → cat reads from fd=3
```

#### B. Three-Command Pipe: `ls | grep txt | wc -l`

**Provide:**
- How many pipe() calls? (Should be 2 for 3 commands)
- What file descriptor numbers are created?
- How does the middle command (grep) handle both stdin and stdout?
- Verify all three children are created

Strace Analysis:

Number of pipes: 2 pipes needed for 3 commands → creates 2 fd pairs
pipe([3, 4]) = 0   # pipe1 for ls → grep
pipe([5, 6]) = 0   # pipe2 for grep → wc

File descriptors assigned:
pipe1: read=3, write=4
pipe2: read=5, write=6

Middle command (grep):
dup2(3, 0) → stdin from pipe1 read end
dup2(6, 1) → stdout to pipe2 write end
Closes unused fds: 3, 4, 5, 6 as appropriate

Children created: 3 (ls, grep, wc) via fork()

Data flow:
ls → pipe1 → grep → pipe2 → wc
Middle command reads from pipe1 and writes to pipe2

#### C. File Descriptor Leak Demo

**Experiment:** Temporarily comment out ALL close() calls in your code, then trace:

```bash
strace -f -e trace=pipe,dup2,close ./dsh
dsh3> ls | cat
[process hangs?]
```

**Provide:**
- What happened? Did it hang?
- Why did it hang (or not)?
- What does strace show about open file descriptors?

Experiment: Comment out all close() calls and run:
strace -f -e trace=pipe,dup2,close ./dsh
dsh3> ls | cat

Observations:
Behavior: Process hangs; cat never finishes reading
Reason: All write ends of the pipe remain open in parent/children

EOF never sent to the reader (cat)

Strace shows:
[pid 2002] read(3, "", 4096) = ?  # blocked, waiting for EOF
[pid 2000] pipe([3, 4]) = 0       # write end still open

Conclusion: Closing unused pipe ends is critical to signal EOF and prevent hanging


### 3. File Descriptor Management (3 points)

Analyze file descriptor lifecycle for a pipeline.

**Answer these questions:**

1. **When are pipes created?**
   - Before or after fork()?
   - How many pipes for N commands?

Pipes are created before fork(). The parent creates all pipes needed for the entire pipeline so that children inherit the correct file descriptors.

For N commands, the shell needs N−1 pipes. Each pipe connects one command’s stdout to the next command’s stdin.


2. **What file descriptors do pipes use?**
   - What numbers are assigned?
   - Why do they start at 3? (0=stdin, 1=stdout, 2=stderr)

Pipes are assigned the lowest available file descriptors, starting from 3. Example: [3, 4] for the first pipe, [5, 6] for the second.

They start at 3 because:
0 = stdin
1 = stdout
2 = stderr
So 3+ are available for pipes and other files.

3. **How does dup2() work?**
   - What does dup2(4, 1) do?
   - Why do we close fd=4 after dup2(4, 1)?

dup2(4, 1) duplicates fd=4 to fd=1 (stdout). After this call:
Both fd=1 and fd=4 point to the same pipe write end
stdout now writes into the pipe
Original fd=4 can be closed because fd=1 still points to the pipe

Closing fd=4 prevents file descriptor leaks and avoids keeping unnecessary write ends open, which could block the reader.

4. **Which pipes does each process close?**
   - First command in pipeline?
   - Middle command?
   - Last command?
   - Parent process?

First command in pipeline:
Closes all read ends (except the ones used if part of multiple pipelines)
Closes the write ends after dup2

Middle command(s):
Closes all pipe ends except stdin and stdout ends used for input/output
After dup2, closes original fds

Last command:
Closes all write ends, keeps only the read end for stdin
Closes the read end after dup2

Parent process:
Closes all pipe ends after forking children to avoid hanging readers

5. **What happens if you forget to close a pipe?**
   - Which process hangs?
   - Why does it hang?
   - How does strace show this?

Which process hangs: The reader (child reading from the pipe)

Why it hangs:
The reader waits for EOF, which is sent only when all write ends of the pipe are closed.
If any process still has the write end open, the reader blocks indefinitely.

Strace shows this:
read(3, "", 4096) = ?  # blocked, waiting for EOF

Indicates the process is waiting on a read call because the write end of the pipe is still open elsewhere.

**Example answer for question 3:**
```
dup2(4, 1) duplicates file descriptor 4 to file descriptor 1 (stdout).
After this call:
- Both fd=1 and fd=4 point to the same pipe write end
- stdout now writes to the pipe
- We can close fd=4 because fd=1 still points to the pipe
- If we don't close fd=4, we waste a file descriptor
```

### 4. Pipeline Verification (2 points)

Use strace to verify your implementation is correct:

**Checklist:**
- [ ] pipe() called N-1 times for N commands
- [ ] Each child calls dup2() appropriately
- [ ] All children close ALL pipe file descriptors
- [ ] Parent closes all pipes after forking
- [ ] All children call execve()
- [ ] Parent waits for all children

**Questions to answer:**
1. Does your implementation create the correct number of pipes?
Yes, 1 pipe for 2 commands, 2 pipes for 3 commands, etc.

2. Does each child redirect stdin/stdout correctly?
Yes, verified with dup2() calls in strace output

3. Does each process close all unused pipe ends?
Yes, strace confirms close() called for all unnecessary fds

4. Are there any file descriptor leaks?
No leaks observed; closing pipes after dup2 ensures all fds are cleaned

If you found bugs, describe what was wrong and how you fixed it.

---

## Technical Requirements

### strace Commands to Use

**Basic pipe tracing:**
```bash
strace -f -e trace=pipe,dup2,close ./dsh
```

**Full pipeline tracing:**
```bash
strace -f -e trace=pipe,dup2,close,fork,execve,wait4 ./dsh
```

**Save output to file:**
```bash
strace -f -e trace=pipe,dup2,close -o trace.txt ./dsh
```

**See file descriptor numbers:**
```bash
strace -f -v ./dsh 2>&1 | grep -E "(pipe|dup2|close)"
```

### Key strace Flags

- `-f` : Follow child processes (ESSENTIAL!)
- `-e trace=pipe,dup2,close` : Only show these syscalls
- `-v` : Verbose mode (shows more details)
- `-o FILE` : Save output to file
- `2>&1` : Redirect stderr to stdout

### Understanding strace Output

**pipe() creates fd pair:**
```
pipe([3, 4]) = 0
```
- Creates pipe with read end fd=3, write end fd=4
- Returns 0 on success

**dup2() redirects:**
```
dup2(4, 1) = 1
```
- Duplicates fd=4 to fd=1 (stdout)
- Returns new fd (1)

**close() closes fd:**
```
close(3) = 0
```
- Closes file descriptor 3
- Returns 0 on success

**With PIDs:**
```
[pid 1001] pipe([3, 4]) = 0
[pid 1002] dup2(4, 1) = 1
```
- Shows which process makes each call

---

## Grading Rubric

**10 points total:**

**Learning Process (2 points)**
- 2 pts: Clear documentation of AI-assisted learning with specific examples
- 1 pt: Vague description of learning process
- 0 pts: No evidence of learning process

**Basic Pipe Analysis (3 points)**
- 3 pts: All three scenarios analyzed correctly with real strace output
- 2 pts: Two scenarios analyzed well
- 1 pt: One scenario analyzed
- 0 pts: No meaningful analysis

**File Descriptor Management (3 points)**
- 3 pts: Thorough answers to all questions with correct understanding
- 2 pts: Good answers, minor gaps
- 1 pt: Basic understanding, significant gaps
- 0 pts: No understanding or incorrect

**Pipeline Verification (2 points)**
- 2 pts: Thorough verification, identifies implementation correctness
- 1 pt: Basic verification, incomplete
- 0 pts: No verification

---

## Hints for Success

### Running strace on Pipelines

**Must use -f flag:**
```bash
strace -f ./dsh        # Correct - traces all children
strace ./dsh           # Wrong - only traces parent
```

**Test with simple pipe:**
```bash
strace -f -e trace=pipe,dup2,close ./dsh
dsh3> ls | cat
dsh3> exit
```

### Understanding File Descriptors

**Standard file descriptors:**
- 0 = stdin
- 1 = stdout
- 2 = stderr
- 3+ = your pipes and files

**After pipe([3, 4]):**
- fd=3 is read end
- fd=4 is write end

**After dup2(4, 1):**
- fd=1 now points to pipe write end
- fd=4 still points to pipe write end
- Can close fd=4 (fd=1 still works)

### Finding Pipe Operations

**Search for specific syscalls:**
```bash
strace -f ./dsh 2>&1 | grep pipe
strace -f ./dsh 2>&1 | grep dup2
strace -f ./dsh 2>&1 | grep close
```

### Common Issues

**Process hangs:**
- Forgot to close write end
- Reader waits for EOF that never comes
- strace shows process blocked in read()

**Too many open files:**
- Not closing pipes
- File descriptor leak
- strace shows close() never called

**Wrong output:**
- dup2() to wrong fd
- Pipes connected incorrectly
- Check dup2() calls in strace

---

## Example: What Good Analysis Looks Like

Here's what a strong pipe analysis might include:

### Two-Command Pipeline: echo hello | cat

**Command:**
```bash
strace -f -e trace=pipe,dup2,close,fork,wait4 ./dsh
dsh3> echo hello | cat
dsh3> exit
```

**Relevant strace output:**
```
[pid 5000] pipe([3, 4])              = 0
[pid 5000] fork()                    = 5001
[pid 5001] dup2(4, 1)                = 1
[pid 5001] close(3)                  = 0
[pid 5001] close(4)                  = 0
[pid 5000] fork()                    = 5002
[pid 5002] dup2(3, 0)                = 0
[pid 5002] close(3)                  = 0
[pid 5002] close(4)                  = 0
[pid 5000] close(3)                  = 0
[pid 5000] close(4)                  = 0
[pid 5000] wait4(-1, NULL, 0, NULL)  = 5001
[pid 5000] wait4(-1, NULL, 0, NULL)  = 5002
```

**Analysis:**

**1. Parent creates pipe (PID 5000):**
```
pipe([3, 4]) = 0
```
- Creates pipe with fds 3 and 4
- fd=3 is read end, fd=4 is write end
- Both ends available in parent

**2. Parent forks first child (echo):**
```
fork() = 5001
```
- Child (PID 5001) inherits pipe fds 3 and 4

**3. First child redirects stdout:**
```
[pid 5001] dup2(4, 1) = 1
```
- Child duplicates fd=4 to fd=1 (stdout)
- stdout now writes to pipe write end
- echo's output will go into pipe

**4. First child closes pipes:**
```
[pid 5001] close(3) = 0   # Close unused read end
[pid 5001] close(4) = 0   # Close original write end
```
- Closes read end (won't read from pipe)
- Closes original write end (dup still exists on fd=1)
- Clean up - only keep what's needed

**5. Parent forks second child (cat):**
```
fork() = 5002
```
- Child (PID 5002) inherits pipe fds 3 and 4

**6. Second child redirects stdin:**
```
[pid 5002] dup2(3, 0) = 0
```
- Child duplicates fd=3 to fd=0 (stdin)
- stdin now reads from pipe read end
- cat will read input from pipe

**7. Second child closes pipes:**
```
[pid 5002] close(3) = 0   # Close original read end
[pid 5002] close(4) = 0   # Close unused write end
```
- Closes original read end (dup still exists on fd=0)
- Closes write end (won't write to pipe)

**8. Parent closes all pipes:**
```
[pid 5000] close(3) = 0
[pid 5000] close(4) = 0
```
- Parent doesn't need pipes
- Critical: must close write end so cat gets EOF!
- If parent kept write end open, cat would hang

**9. Parent waits for children:**
```
wait4(-1, NULL, 0, NULL) = 5001
wait4(-1, NULL, 0, NULL) = 5002
```
- Waits for first child (echo) to complete
- Waits for second child (cat) to complete

**Verification:**
- ✓ One pipe created (correct for 2 commands)
- ✓ Two children forked
- ✓ First child: stdout → pipe write
- ✓ Second child: stdin ← pipe read
- ✓ Both children close all pipes
- ✓ Parent closes all pipes
- ✓ Parent waits for both children

**Why closing matters:**
If parent didn't close fd=4, cat would never get EOF because:
- cat reads from fd=3 (pipe read end)
- EOF only comes when ALL write ends (fd=4) are closed
- Parent still has fd=4 open
- cat blocks forever waiting for more input

---

## Resources

- `man 2 pipe` - pipe system call
- `man 2 dup2` - duplicate file descriptor
- `man 2 close` - close file descriptor
- `man strace` - strace documentation
- Your AI tool of choice (ChatGPT, Claude, Gemini, etc.)

---

## Final Thought

Pipes are invisible - you can't see the data flowing between commands. strace makes them visible by showing the actual system calls:

1. pipe() creates the channel
2. dup2() redirects stdin/stdout
3. close() cleans up unused ends
4. execve() runs the command
5. Data flows automatically!

The goal isn't just to trace some system calls - it's to **understand how your shell connects processes together** at the deepest level. This knowledge applies to every Unix/Linux system you'll ever work with.

**Good luck with your analysis!**
