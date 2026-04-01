#!/bin/bash

# 1. Check if the input file was provided
if [ -z "$1" ]; then
    echo "Usage: $0 <problem_list.txt>"
    exit 1
fi

INPUT_FILE="$1"
OUTPUT_FILE="benchmark_results_2.csv"

# 2. Define your THREE program versions here
PROG_C="./PROVER/eprover"
PROG_A="./INTMAP_16/eprover"
PROG_B="./INTMAP_32/eprover" # Update this path to your third version

# Define the arguments for the theorem prover
ARGS="--auto --detsort-new --detsort-rw"

# 3. Create/Clear the output file and add a header
echo "Problem,Time_Version_16(s),Time_Version_32(s),Time_Version_normal(s)" > "$OUTPUT_FILE"

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
    problem_path="../../some_problems/TPTP-problems/$problem"

    # Verify the problem file exists before running
    if [ ! -f "$problem_path" ]; then
        echo "Warning: $problem_path not found. Skipping..."
        continue
    fi

    echo "Benchmarking $problem..."

    # Run Version A on Core 0
    # 'taskset -c 0' forces the process to stay on the first CPU core
    time_a=$( { time taskset -c 0 $PROG_A $ARGS "$problem_path" > /dev/null 2>&1; } 2>&1 )

    # Run Version B on Core 0
    time_b=$( { time taskset -c 0 $PROG_B $ARGS "$problem_path" > /dev/null 2>&1; } 2>&1 )

    # Run Version C on Core 0
    time_c=$( { time taskset -c 0 $PROG_C $ARGS "$problem_path" > /dev/null 2>&1; } 2>&1 )

    # Append the results to the CSV output file
    echo "$problem,$time_a,$time_b,$time_c" >> "$OUTPUT_FILE"

done < "$INPUT_FILE"

echo "Done! Results are saved in $OUTPUT_FILE"
