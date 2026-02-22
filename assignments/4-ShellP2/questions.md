1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  We use fork/execvp instead of just execvp because fork creates a new child process that is an exact copy of the parent. This allows the shell to continue running after executing the external command. If we called execvp directly, it would replace the current shell process with the new program, effectively terminating our shell. The fork provides process isolation - the child process can execute the command (and potentially fail) without affecting the parent shell process, which can then wait for the child to complete and continue accepting new commands.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If fork() fails, it returns -1 to the parent process and no child process is created. This can happen if the system has reached its maximum number of processes, or if there are insufficient resources available. In my implementation, I check if pid < 0 and return ERR_EXEC_CMD, which then prints an error message to the user. The shell continues running and can accept new commands.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  execvp() searches for the executable in the directories specified by the PATH environment variable. The 'p' in execvp stands for "path". It searches each directory in PATH (separated by colons) until it finds a matching executable file with the given name. If the command contains a slash (/), execvp() treats it as a path and doesn't search PATH.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  The purpose of calling wait() (or waitpid()) is to make the parent process wait for the child process to complete execution. This allows the parent to retrieve the child's exit status and prevents the child from becoming a "zombie" process (a terminated process that still occupies a process table entry). If we didn't call wait(), the child would become a zombie after termination, and the parent wouldn't know when the command finished or what its exit status was. The shell would also return to the prompt immediately, potentially overlapping command outputs.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  WEXITSTATUS() extracts the exit status code from a terminated child process. When a child process calls exit() with a status code, this information is stored in the status variable returned by wait(). WEXITSTATUS() masks and shifts the status to get the actual 8-bit exit code. This is important because it allows the shell to determine whether the command succeeded (typically exit code 0) or failed (non-zero exit codes), and to implement features like the 'rc' command that shows the last return code.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  My implementation handles quoted arguments by tracking an 'in_quotes' boolean flag. When encountering an opening quote (") that's not already in quotes, it sets in_quotes to true and skips the quote character. While in_quotes, spaces are treated as regular characters and copied to the argument. When encountering a closing quote, in_quotes is set to false. This is necessary because users need to pass arguments containing spaces (like filenames with spaces or message strings) that should be treated as single arguments rather than being split into multiple arguments.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  The main changes were: 1) Simplified from handling multiple commands with pipes to a single cmd_buff structure, 2) Added proper quoted string handling with in_quotes tracking, 3) Improved space handling to eliminate duplicates while preserving quoted spaces, 4) Changed from returning a command list to populating a single cmd_buff. The main challenge was ensuring the quoted string logic worked correctly with the space trimming and argument boundary detection. Getting the pointer arithmetic right for copying arguments to the internal buffer while maintaining proper null termination required careful debugging.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals are software interrupts that provide a mechanism for asynchronous communication between processes or from the kernel to processes. They differ from other IPC forms because they are event-driven rather than stream-based or message-based. Signals don't carry data payloads (except for real-time signals) and are used for notifications, control, and error handling rather than data exchange. Other IPC methods like pipes, sockets, and shared memory are designed for structured data transfer, while signals are for process lifecycle management and exceptional conditions.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGKILL: Cannot be caught or ignored, forces immediate process termination. Used when a process is unresponsive and needs to be killed immediately. SIGTERM: Can be caught, allows graceful shutdown. Used for normal process termination, giving the process a chance to clean up resources. SIGINT: Generated by Ctrl+C, can be caught. Used to interrupt running processes, typically allowing them to clean up before terminating.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  When a process receives SIGSTOP, it is suspended (stopped) and cannot continue execution until it receives SIGCONT. The process remains in memory but is not scheduled for CPU time. SIGSTOP cannot be caught or ignored, unlike SIGINT. This is because SIGSTOP is a non-catchable signal designed to provide reliable process suspension for system management and debugging purposes. If it could be ignored, a malicious or misbehaving process could avoid being stopped by system administrators or debuggers, compromising system control.
