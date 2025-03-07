#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

### Local Mode Tests

@test "Local: 'dragon' built-in command prints ASCII art (at least doesn't fail)" {
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

