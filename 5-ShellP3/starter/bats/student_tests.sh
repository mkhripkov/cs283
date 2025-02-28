#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

# A simple pipeline using two commands.
@test "Pipeline: echo hello | cat" {
    run ./dsh <<EOF
echo hello | cat
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"hello"* ]]
}

# Pipeline that transforms text to uppercase.
@test "Pipeline: echo hello world | tr a-z A-Z" {
    run ./dsh <<EOF
echo hello world | tr a-z A-Z
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"HELLO WORLD"* ]]
}

# Pipeline with three commands to sort tokens.
@test "Pipeline: echo 'c b a' | tr \" \" \"\n\" | sort" {
    run ./dsh <<EOF
echo "c b a" | tr " " "\n" | sort
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"a"* ]]
    [[ "$output" == *"b"* ]]
    [[ "$output" == *"c"* ]]
}


# Pipeline with extra whitespace between pipes.
@test "Pipeline: extra whitespace between pipes" {
    run ./dsh <<EOF
echo hello |    | tr a-z A-Z
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"HELLO"* ]]
}

# Pipeline with a non-existent command in the chain.
@test "Pipeline: error in middle command" {
    run ./dsh <<EOF
echo hello | nonexistcmd | tr a-z A-Z
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"execvp"* ]] || [[ "$output" == *"not found"* ]]
}

# Pipeline that uses a word count utility.
@test "Pipeline: word count using wc" {
    run ./dsh <<EOF
echo hello world | wc -w
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"2"* ]]
}
