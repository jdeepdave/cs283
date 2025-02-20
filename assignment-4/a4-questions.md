1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  We need to use fork() before execvp() because of how these system calls work together. When execvp() runs, it completely replaces the current process's code with the new program. If the shell directly called execvp(), it would be replaced by whatever command the user wanted to run (like ls or echo), effectively terminating the shell. Instead, by calling fork() first, we create a copy of the shell process. This child process can then safely call execvp() to run the desired command, while the original shell process (the parent) continues to exist and wait for the next command. This approach allows the shell to persist and maintain its functionality throughout multiple command executions.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  When fork() returns an error, it indicates that the operating system was unable to create a new process, typically because either there isn't enough available memory or the system has reached its process limit. In a shell implementation, it's important to handle this failure gracefully by informing the user through an error message (using perror("fork failed")) and returning an appropriate error code, rather than silently failing or crashing.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  When you tell the computer to do something like "ls", it doesn't know where that tool is at first. So, it looks at the "PATH" list and goes to each place on the list, one by one, until it finds the "ls" tool. Then, it uses said tool and if it goes through the whole list and still can't find the tool, it spits our the error

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  wait() makes sure the shell waits for the child process to finish before continuing. Without wait(), the shell would immediately go back to waiting for new input and might print the next prompt before the command finishes running. This could make things look messy, with outputs from different commands mixed together.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  WEXITSTATUS() gets the exit code of the finished command. Every program returns an exit code when it finishes. 0 means success, while other numbers like 2 can represent a command not found. Our shell uses WEXITSTATUS() to store the last exit code, so users can check it later with rc

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  build_cmd_buff() keeps spaces inside quotes as part of the same argument instead of treating them as separate words. This is needed so that commands like echo "hello world" print exactly as expected.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  I updated the parser to handle quotes properly, remove extra spaces, and store arguments correctly. Making sure multiple spaces inside quotes were not removed while still removing extra spaces outside quotes was the most difficult part.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals in Linux are asynchronous notifications used to communicate with processes. They tell a process to perform an action, like stopping, continuing, or terminating. Unlike other Interprocess Communication (IPC) "methods like pipes, message queues, or shared memory, which involve data transfer, signals only send notifications (like interrupts)".

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGKILL ->  terminates process by force. An unresponsive or dangerously slow process can be terminated
                   SIGTERM -> requests program to terminate. Seen many times when exiting certain prorgrams via prompt to ensure a safe termination
                   SIGINT -> interruption of a process. This is commonly seen with Control + C in terminal. It can interrupt an endless loop or other faulty programs

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  It cannot be caught, blocked, or ignored (unlike SIGINT or SIGTERM). The process stays in a stopped state until it gets a SIGCONT signal to resume.
