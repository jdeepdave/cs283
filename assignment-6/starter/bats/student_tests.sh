#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

#!/usr/bin/env bats

setup() {
    ./dsh -s &  # Start the server in the background
    SERVER_PID=$!
    sleep 1  # Allow time for the server to start
}

teardown() {
    if kill -0 $SERVER_PID 2>/dev/null; then
        kill $SERVER_PID
        sleep 0.5  # Allow time for shutdown
    fi
    wait $SERVER_PID 2>/dev/null || true
}

normalize_output() {
    echo "$1" | sed 's/dsh4>//g' | xargs
}

@test "Single Command: ls" {
    run ./dsh -c <<EOF
ls
EOF
    echo "Raw Output: $output"

    cleaned_output=$(normalize_output "$output")
    echo "Cleaned Output: $cleaned_output"

    [ "$status" -eq 0 ]
    [[ "$cleaned_output" =~ "dshlib.c" ]]
}

@test "Single Command: cat test.txt" {
    run ./dsh -c <<EOF
cat test.txt
EOF
    echo "Raw Output: $output"

    cleaned_output=$(normalize_output "$output")
    echo "Cleaned Output: $cleaned_output"

    [ "$status" -eq 0 ]
    [[ "$cleaned_output" =~ "test file" ]]
}

@test "Single Command: echo Hello" {
    run ./dsh -c <<EOF
echo Hello
EOF
    echo "Raw Output: $output"

    cleaned_output=$(normalize_output "$output")
    echo "Cleaned Output: $cleaned_output"

    [ "$status" -eq 0 ]
    [[ "$cleaned_output" =~ "Hello" ]]
}

@test "Single Command: grep test test.txt" {
    run ./dsh -c <<EOF
grep test test.txt
EOF
    echo "Raw Output: $output"

    cleaned_output=$(normalize_output "$output")
    echo "Cleaned Output: $cleaned_output"

    [ "$status" -eq 0 ]
    [[ "$cleaned_output" =~ "test file" ]]
}

@test "Single Command: whoami" {
    run ./dsh -c <<EOF
whoami
EOF
    echo "Raw Output: $output"

    cleaned_output=$(normalize_output "$output")
    echo "Cleaned Output: $cleaned_output"

    [ "$status" -eq 0 ]
}

@test "Single Command: pwd" {
    run ./dsh -c <<EOF
pwd
EOF
    echo "Raw Output: $output"

    cleaned_output=$(normalize_output "$output")
    echo "Cleaned Output: $cleaned_output"

    [ "$status" -eq 0 ]
    [[ "$cleaned_output" =~ "/Users/jdave/CS283-main/cs283/assignment-6/starter" ]]
}

@test "Single Command: uname -a" {
    run ./dsh -c <<EOF
uname -a
EOF
    echo "Raw Output: $output"

    cleaned_output=$(normalize_output "$output")
    echo "Cleaned Output: $cleaned_output"

    [ "$status" -eq 0 ]
}

@test "Single Command: date" {
    run ./dsh -c <<EOF
date
EOF
    echo "Raw Output: $output"

    cleaned_output=$(normalize_output "$output")
    echo "Cleaned Output: $cleaned_output"

    [ "$status" -eq 0 ]
}

@test "Double Command: echo Hello ; echo World" {
    run ./dsh -c <<EOF
echo Hello ; echo World
EOF
    echo "Raw Output: $output"

    cleaned_output=$(normalize_output "$output")
    echo "Cleaned Output: $cleaned_output"

    [ "$status" -eq 0 ]
    [[ "$cleaned_output" =~ "Hello" ]]
    [[ "$cleaned_output" =~ "World" ]]
}

@test "Double Command: echo First && echo Second" {
    run ./dsh -c <<EOF
echo First && echo Second
EOF
    echo "Raw Output: $output"

    cleaned_output=$(normalize_output "$output")
    echo "Cleaned Output: $cleaned_output"

    [ "$status" -eq 0 ]
    [[ "$cleaned_output" =~ "First" ]]
    [[ "$cleaned_output" =~ "Second" ]]
}

@test "Double Command: cat test.txt | grep test" {
    run ./dsh -c <<EOF
cat test.txt | grep test
EOF
    echo "Raw Output: $output"

    cleaned_output=$(normalize_output "$output")
    echo "Cleaned Output: $cleaned_output"

    [ "$status" -eq 0 ]
    [[ "$cleaned_output" =~ "test file" ]]
}
