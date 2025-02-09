1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  fgets() is a good choice for this application because we are building a shell. When using a shell, commands are typically entered line by line, and fgets() reads input one line at a time.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  malloc() is used because it allows for dynamic memory allocation. This means we can determine the size of the buffer at runtime, which helps us avoid using too much or too little memory.


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  Trimming leading and trailing spaces is necessary because the whitespaces could interfere with the tokenization of the input.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  One example of redirection that we should implement is output redirection, for example command > file.txt. The challenge would be making it so that the shell opens or creates the file for writing and redirects the command's output to that file. Another example is input redirection, for example command < file.txt. The challenge here would be having the shell open the file for reading and feeding its contents to the command. Lastly, we could implement error redirection, for example command 2> file.txt. The challenge here would be having the shell distinguish between stdout and stderr.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  The key difference between the two is that with redirection you are sending a command's input or output to/from a file. With pipes, you are connecting the output of one command to the input of another. Piping does not involve any file I/O.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**: Keeping these seperate allows users and programs to handle errors differently from regular output. For example, a user might want to redirect only the standard output to a file, while still wanting to see error messages directly on the terminal.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**: The shell should display error messages from STDERR immediately so that the user is aware of any problems right away. In cases where a command outputs both stdout and stderr, the shell could offer an option to merge them (perhaps using redirection), but in my opinion it should not be the default. Keeping them separate would avoid any potential confusion.