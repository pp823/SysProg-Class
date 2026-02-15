1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  fgets() is a good choice because it safely reads a whole line from input, including spaces, until a newline or a maximum number of characters is reached. Unlike scanf, it does not stop at whitespace, which is important for shell commands like ls -la /tmp. It also prevents buffer overflows by allowing us to specify the maximum number of characters.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**: We use malloc() so that the memory for storing commands is dynamically allocated at runtime. A fixed-size array could waste memory if the command is short or be insufficient for long commands. Dynamic allocation allows flexibility and ensures that multiple command buffers can coexist without overwriting each other.

3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**: Trimming spaces is necessary because extra spaces can create empty or incorrect arguments. For example, ls   -l with extra spaces could generate empty strings in argv or confuse parsing. Without trimming, commands may fail, produce unexpected behavior, or cause errors when passed to execvp.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**: 
    1. command > file.txt – Redirect STDOUT to a file. Challenge: We need to open the file, duplicate the file descriptor to STDOUT, and ensure it is closed after execution. 
    2. command >> file.txt – Append STDOUT to a file. Challenge: Similar to above but must use O_APPEND to avoid overwriting.  
    3. command < input.txt – Redirect STDIN from a file. Challenge: Must open the input file and duplicate its descriptor to STDIN for the command.  
    Implementing these requires careful use of open(), dup2(), and error handling.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**: Redirection connects a command to a file, either for input or output, while piping connects the output of one command directly to the input of another. Redirection changes the source or destination of data; pipes connect commands in a workflow, enabling multiple commands to process data sequentially without intermediate files.


- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**: Keeping STDOUT and STDERR separate allows users to distinguish between normal output and error messages. This separation is useful for debugging, logging, or redirecting output to different files. For example, a user can capture normal output in one file and errors in another, avoiding confusion.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**: The shell should allow users to see error messages even when output is redirected. We can merge STDOUT and STDERR using 2>&1 syntax, which duplicates STDERR to STDOUT. This is useful when we want to capture all output in one stream. Our shell should support both separate and merged streams for flexibility. 