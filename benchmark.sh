#!/bin/bash

# 1. Check if the input file was provided
if [ -z "$1" ]; then
    echo "Usage: $0 <problem_list.txt>"
    exit 1
fi

INPUT_FILE="$1"
OUTPUT_FILE="benchmark_results.csv"

# 2. Define your two program versions here
PROG_A="./PROVER/eprover"
PROG_B="./INTMAP_16/eprover"

# Define the arguments for the theorem prover
ARGS="--auto --detsort-new --detsort-rw"

# 3. Create/Clear the output file and add a header
echo "Problem,Time_Version_A(s),Time_Version_B(s)" > "$OUTPUT_FILE"

# Tell the Bash built-in 'time' to only output the "real" time in seconds (%R)
TIMEFORMAT="%R"

# 4. Read the input file line by line
while IFS= read -r problem || [ -n "$problem" ]; do
    
    # Skip empty lines
    if [ -z "$problem" ]; then
        continue
    fi

    # Extract the first 3 characters for the directory name (e.g., "ABC")
    dir_name="${problem:0:3}"
    
    # Construct the relative path to the problem
    problem_path="../../TPTP/TPTP-v9.2.1/Problems/${dir_name}/${problem}"

    # Verify the problem file exists before running
    if [ ! -f "$problem_path" ]; then
        echo "Warning: $problem_path not found. Skipping..."
        continue
    fi

    echo "Benchmarking $problem..."

    # Run Version A
    # - The inner "> /dev/null 2>&1" destroys all output/errors from eprover.
    # - The outer "{ ...; } 2>&1" captures the time output (which normally prints to stderr) 
    #   and routes it into our time_a variable.
    time_a=$( { time $PROG_A $ARGS "$problem_path" > /dev/null 2>&1; } 2>&1 )

    # Run Version B
    time_b=$( { time $PROG_B $ARGS "$problem_path" > /dev/null 2>&1; } 2>&1 )

    # Append the results to the CSV output file
    echo "$problem,$time_a,$time_b" >> "$OUTPUT_FILE"

done < "$INPUT_FILE"

echo "Done! Results are saved in $OUTPUT_FILE"