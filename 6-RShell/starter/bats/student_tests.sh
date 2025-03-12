#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

### Local Mode Tests

@test "Local: check ls runs without errors" {
    run ./dsh <<EOF
ls
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "dsh4>" ]]
}

@test "Local: Pipeline echo hello | cat" {
    run ./dsh <<EOF
echo hello | cat
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "hello" ]]
    [[ "$output" =~ "dsh4>" ]]
}

@test "Local: Pipeline echo hello world | tr a-z A-Z" {
    run ./dsh <<EOF
echo hello world | tr a-z A-Z
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "HELLO WORLD" ]]
    [[ "$output" =~ "dsh4>" ]]
}

@test "Local: Pipeline echo 'c b a' | tr \" \" \"\\n\" | sort" {
    run ./dsh <<EOF
echo "c b a" | tr " " "\n" | sort
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "a" ]]
    [[ "$output" =~ "b" ]]
    [[ "$output" =~ "c" ]]
    [[ "$output" =~ "dsh4>" ]]
}

@test "Local: Pipeline extra whitespace between pipes" {
    run ./dsh <<EOF
echo hello |    | tr a-z A-Z
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "HELLO" ]]
    [[ "$output" =~ "dsh4>" ]]
}

@test "Local: Pipeline error in middle command" {
    run ./dsh <<EOF
echo hello | nonexistcmd | tr a-z A-Z
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "execvp" ]] || [[ "$output" =~ "not found" ]]
    [[ "$output" =~ "dsh4>" ]]
}

@test "Local: Pipeline word count using wc" {
    run ./dsh <<EOF
echo hello world | wc -w
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "2" ]]
    [[ "$output" =~ "dsh4>" ]]
}


@test "Local: 'dragon' built-in command prints ASCII art" {
  run ./dsh <<EOF
dragon
exit
EOF
  [ "$status" -eq 0 ]
  [[ "$output" =~ "@" ]]
}

@test "Local: 'cd' to parent directory and run 'pwd'" {
  run ./dsh <<EOF
cd ..
pwd
exit
EOF
  [ "$status" -eq 0 ]
  [[ "$output" =~ "dsh4>" ]] 
}

@test "Local: built-in 'exit' terminates the shell" {
  run ./dsh <<EOF
exit
EOF
  [ "$status" -eq 0 ]
}


@test "Local: echo pipeline through grep" {
  run ./dsh <<EOF
echo "this is a test" | grep test
exit
EOF
  [ "$status" -eq 0 ]
  [[ "$output" =~ "this is a test" ]]
}

@test "Local: multiple pipes (echo -> grep -> cut)" {
  run ./dsh <<EOF
echo "hello:world" | grep hello | cut -d : -f2
exit
EOF
  [ "$status" -eq 0 ]
  [[ "$output" =~ "world" ]]
}


### Remote Mode Tests

setup_server() {
  export TEST_PORT=7899
  ./dsh -s -p "$TEST_PORT" &
  export SERVER_PID=$!
  sleep 1
}

cleanup_server() {
  kill $SERVER_PID 2>/dev/null || true
  wait $SERVER_PID 2>/dev/null || true
}


@test "Remote: connect client, run 'ls', then 'exit' (server stays alive)" {
  setup_server

  run ./dsh -c -p "$TEST_PORT" <<EOF
ls
exit
EOF

  [ "$status" -eq 0 ]

  kill -0 $SERVER_PID 2>/dev/null
  alive_status=$?
  [ "$alive_status" -eq 0 ]

  cleanup_server
}


@test "Remote: run pipeline (echo -> grep) via client-server" {
  setup_server

  run ./dsh -c -p "$TEST_PORT" <<EOF
echo "hello server" | grep hello
exit
EOF

  [ "$status" -eq 0 ]
  [[ "$output" =~ "hello server" ]]

  cleanup_server
}


@test "Remote: built-in 'cd' then run 'pwd' (basic check for no crash)" {
  setup_server

  run ./dsh -c -p "$TEST_PORT" <<EOF
cd ..
pwd
exit
EOF

  [ "$status" -eq 0 ]
  [[ "$output" =~ "/" ]]

  cleanup_server
}

@test "Remote: multiple commands in one session" {
  setup_server

  run ./dsh -c -p "$TEST_PORT" <<EOF
pwd
ls
echo "done testing multiple commands"
exit
EOF

  [ "$status" -eq 0 ]
  [[ "$output" =~ "done testing multiple commands" ]]

  cleanup_server
}

@test "Remote: run a 3-command pipeline (grep -> cut -> tr) via client-server" {
  setup_server

  run ./dsh -c -p "$TEST_PORT" <<EOF
echo "color:red:blue:green" | grep color | cut -d : -f3 | tr a-z A-Z
exit
EOF

  [ "$status" -eq 0 ]
  [[ "$output" =~ "BLUE" ]]

  cleanup_server
}
