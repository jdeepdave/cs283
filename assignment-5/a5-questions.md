1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

After I start each command with fork(), I save the IDs of all those child processes. At the end, I loop through and use waitpid() to wait for each one to finish. So the shell knows all the work is done before it lets me type the next command. If I forget waitpid(), the shell would just keep going without waiting, and those commands would still be running in the background. This can cause a mess, like zombie processes hanging around using up system resources.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

Pipes are like tubes between commands. After you connect the part you need with dup2(), you close the extra ends so nobody accidentally keeps the tube open. If you don't close them the system thinks the pipe is still being used, so commands waiting for the end of the data will just keep waiting forever. You could also use up too many file slots (file descriptors), which might crash your shell.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

cd changes the shell’s own current folder. If I made it an external command, it would change the folder for that temporary process, not for the shell itself. So when the shell comes back, I'm still in the same old folder. If external, the folder would change in the child process and disappear as soon as that child finishes. The shell wouldn’t actually move anywhere.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

Instead of making a fixed-size array for commands, I’d use something like malloc() to make space as I go. Every time a | is found, the array grows. However the tradeoffs are more flexibility, but slower because of all the memory work (allocating, copying, freeing). Also you have to be careful to free the memory later, or you get memory leaks.
