#!/bin/bash

# 1. Check if the input file was provided
if [ -z "$1" ]; then
    echo "Usage: $0 <problem_list.txt>"
    exit 1
fi

INPUT_FILE="$1"
OUTPUT_FILE="benchmark_results_p_28.csv"

# 2. Define your program versions here
PROG_A="../PROVER/eprover"
PROG_B="../INTMAP_1/eprover"
PROG_C="../INTMAP_2/eprover"
PROG_D="../INTMAP_4/eprover"
PROG_E="../INTMAP_8/eprover"
PROG_F="../INTMAP_16/eprover"
PROG_G="../INTMAP_32/eprover"

# Define the arguments for the theorem prover
ARGS="--auto --detsort-new --detsort-rw --soft-cpu-limit=300 --cpu-limit=350"

# 3. Create/Clear the output file and add a header
echo "Problem,Time_Version_Standard,Time_Version_1,Time_Version_2,Time_Version_4,Time_Version_8,Time_Version_16,Time_Version_32" > "$OUTPUT_FILE"

# Tell the Bash built-in 'time' to only output the "real" time in seconds (%R)
TIMEFORMAT="%R"

# 4. Read the input file line by line
while IFS= read -r problem || [ -n "$problem" ]; do
    
    # Skip empty lines
    if [ -z "$problem" ]; then
        continue
    fi

    dir_name="${problem:0:3}"
    problem_path="../../../some_problems/TPTP-problems/$problem"

    # Verify the problem file exists before running
    if [ ! -f "$problem_path" ]; then
        echo "Warning: $problem_path not found. Skipping..."
        continue
    fi

    echo "Benchmarking $problem on 7 cores simultaneously..."

    # Run all 7 versions in the background (&) on separate cores (0 through 6).
    # The '2> tmp_X.txt' captures the output of 'time' to a temporary file.
    { time taskset -c 32-63 $PROG_A $ARGS "$problem_path" > /dev/null 2>&1; } 2> tmp_a.txt &
    { time taskset -c 32-63 $PROG_B $ARGS "$problem_path" > /dev/null 2>&1; } 2> tmp_b.txt &
    { time taskset -c 32-63 $PROG_C $ARGS "$problem_path" > /dev/null 2>&1; } 2> tmp_c.txt &
    { time taskset -c 32-63 $PROG_D $ARGS "$problem_path" > /dev/null 2>&1; } 2> tmp_d.txt &
    { time taskset -c 32-63 $PROG_E $ARGS "$problem_path" > /dev/null 2>&1; } 2> tmp_e.txt &
    { time taskset -c 32-63 $PROG_F $ARGS "$problem_path" > /dev/null 2>&1; } 2> tmp_f.txt &
    { time taskset -c 32-63 $PROG_G $ARGS "$problem_path" > /dev/null 2>&1; } 2> tmp_g.txt &

    # WAIT pauses the script until all background processes (&) spawned above are finished
    wait

    # Read the results from the temp files
    time_a=$(cat tmp_a.txt)
    time_b=$(cat tmp_b.txt)
    time_c=$(cat tmp_c.txt)
    time_d=$(cat tmp_d.txt)
    time_e=$(cat tmp_e.txt)
    time_f=$(cat tmp_f.txt)
    time_g=$(cat tmp_g.txt)

    # Clean up the temporary files for the next loop
    rm -f tmp_a.txt tmp_b.txt tmp_c.txt tmp_d.txt tmp_e.txt tmp_f.txt tmp_g.txt

    # Append the results to the CSV output file (fixed missing time_c)
    echo "$problem,$time_a,$time_b,$time_c,$time_d,$time_e,$time_f,$time_g" >> "$OUTPUT_FILE"

done < "$INPUT_FILE"

echo "Done! Results are saved in $OUTPUT_FILE"

