1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

My implementation ensures that all child processes complete before the shell continues accepting user input by saving each childs PID and then calling waitpid() on each child. Without waiting on all children, some might end up being zombie processes and the shell could start executing new commands while earler ones are still running.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

It is necessary to close unused pipe ends after calling dup2() because leaving them open can cause file descriptor leaks and prevent EOF from being signaled. This can cause processes to hang while waiting for input that would never arrive.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

CD is implemented as a built-in rather than an external command because it changes the working directory of the shell itself. If cd were external, it would run in a child process whose directory change wouldn't affect the parent shell.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

To handle an arbitrary number of piped commands, I would have to dynamically allocate the command list and pipe array instead of using static array lengths. The trade-offs to consider are having the program and code be more complex as well as needing to ensure proper cleanup on errors.