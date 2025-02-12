1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  fgets() is a solid choice for reading user input in our shell because it's safe, line-oriented, and prevents overflows. Unlike scanf() or gets(), fgets() ensures that we don’t accidentally read beyond the allocated buffer, which could cause memory corruption. It stops reading after a newline (\n) or when it reaches the buffer limit, making it ideal for processing commands line by line. This is important in a shell because we want to process one command at a time before prompting the user again. Additionally, fgets() gracefully handles EOF (End of File), which allows the shell to run in both interactive and script-based environments without issues.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  Using malloc() we can allocate memory for cmd_buff instead of using a fixed-size array because it allows dynamic memory allocation, which is more flexible. A fixed-size array might either waste memory (if it’s too large) or run out of space (if it’s too small for longer commands). By using malloc(), we allocate just the right amount of memory needed, and we can later realloc() if needed for even longer inputs. This approach makes our shell more scalable and adaptable to different input sizes without unnecessary memory usage.


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  Trimming spaces before and after a command is important because spaces don’t affect the meaning of a command but can cause unexpected issues when executing commands. If we don't remove these spaces, we might store commands incorrectly, causing errors like trying to execute an empty string or incorrect argument parsing. For example, if a user types " ls ", without trimming, the shell might treat it as " ls" (with spaces in the command name), which could fail to execute properly. Trimming ensures that commands are stored cleanly and consistently, avoiding unnecessary errors.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  Output redirection lile >/>>, redirection of stdout/stderr, and redirection of input via <. These examples present challenges such as misplacement of the rdirection of stdout, ensuring that file permissions allow read/write when redirecting something like ls. Another challenge is existence of the file being redicrected 

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Redirection and piping both deal with controlling the flow of data, but they serve different purposes. Redirection sends the output of a command to a file or takes input from a file. It is typically used for saving data or using pre-existing files as input. For example, ls > file.txt saves the directory listing into a file instead of displaying it on the screen. On the other hand, piping (|) connects the output of one command directly into another command's input. It allows commands to work together dynamically without using files. For example, ls | grep ".txt" lists files and filters only those containing ".txt".

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  For example, if we run ls non_existent_folder > output.txt, without separating STDERR, the error message would end up in output.txt, making it hard to determine if the command was successful. By default, errors appear in the terminal while the output can be redirected elsewhere. This separation helps maintain clarity in command execution.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  Our shell should handle errors by displaying clear error messages to STDERR and returning an appropriate exit code. If a command fails, we should print an informative message (e.g., "command not found" or "permission denied"), so the user understands what went wrong.We should also allow users to merge STDOUT and STDERR using redirection (2>&1). This is useful when a user wants to capture everything into one file or pipe both error and normal output into another command. For example,