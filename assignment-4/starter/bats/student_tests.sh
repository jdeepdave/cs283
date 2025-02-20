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

@test "Verify RC command" {
    run ./dsh <<EOF
not_a_command
rc
exit
EOF
    [[ "$output" == *"2"* ]] 
}

@test "multiple spaces" {
    run ./dsh <<EOF
echo    hello     world
exit
EOF
    expected_output="hello world"

    echo "Captured stdout:"
    echo "Output: '$output'"
    echo "Exit Status: $status"

    [[ "$output" == *"$expected_output"* ]]
}

@test "Verify exit command" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Invalid permissions test" {
    touch no_exec_file
    chmod -x no_exec_file
    run ./dsh <<EOF
./no_exec_file
rc
exit
EOF
    [[ "$output" == *"Error: Permission denied"* ]]
    rm no_exec_file
}

@test "cd behaviour test" {
    mkdir -p test_dir
    run ./dsh <<EOF
cd test_dir
pwd
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"test_dir"* ]]
    rmdir test_dir
}

@test "pwd behaviour test" {
    run ./dsh <<EOF
pwd
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"/"* ]]
}

@test "Invalid Command recognized and handled" {
    run ./dsh <<EOF
not_a_command
exit
EOF
    [[ "$output" == *"Error: Command not found in PATH"* ]]
}

@test "Test Dragon" {
    run ./dsh <<EOF
dragon
exit
EOF
    [[ "$output" == *"@"* ]] 
}

