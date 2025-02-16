1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**: We use fork because it leaves the shell process intact. We need to use fork so that the shell can manage multple commands without replacing its own process.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If fork fails, not child proces is created. In my implementation, an error message is printed when this hapens (using perror) and an error return code is set.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  execvp() finds the command by searching through the PATH environment variable

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  wait() makes the parent pause until the child completes, which allows us to wait until we get an exit status. If we did not use wait() the parent would not know when the command finished.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  WEXITSTATUS() gets the exit code of a child process from the status returned by wait(). It is important to know the exit code because it lets us know if the process failed or not.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  My implementation uses an inQuotes flag whenever the program encounters double quotes, so that spaces inside quotes are not treated as delimiters. This is necessary to ensure that arguments with embedded spaces are kept together as a single token.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  I removed the pipe-splitting logic and refactored the parser to handle quoted strings and eliminate duplicate spaces. The main challenge was preserving spaces within quotes while at the same time trimming unnecessary whitespace.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals provide asynchronous notifications to processes about events (such as a termination request). Unlike other forms of IPC, signals are lightweight and interrupt a process's execution.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGKILL: Immediately and forcefully terminate a process. SIGTERM: Requests a graceful termination, allows the process to be cleaned up before exiting. SIGINT: Generated (usually using Ctrl+C) to interrupt a process, giving it a chance to handle or exit gracefully.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  When a process recieves SIGSTOP, it is suspended immediately. Unlike SIGINT, SIGSTOP cannot be caught, blocked, or ignored.