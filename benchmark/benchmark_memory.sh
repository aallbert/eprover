#!/bin/bash

# 1. Check if the input file was provided
if [ -z "$1" ]; then
    echo "Usage: $0 <problem_list.txt>"
    exit 1
fi

INPUT_FILE="$1"
OUTPUT_FILE="benchmark_size_all.csv"

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
echo "Problem,Memory_Standard,Memory_V1,Memory_V2,Memory_V4,Memory_V8,Memory_V16,Memory_V32" > "$OUTPUT_FILE"

# Helper function to extract memory or return "NA" if the prover timed out/failed
extract_mem() {
    local file=$1
    # Grab the line, then print the 5th word (the actual byte number)
    local mem=$(grep "Total SizeMalloc()ed memory:" "$file" | awk '{print $5}')
    
    if [ -z "$mem" ]; then
        echo "NA"
    else
        echo "$mem"
    fi
}

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

    echo "Benchmarking Memory for $problem on 7 versions simultaneously..."

    # Run all 7 versions in the background (&). 
    # We redirect ALL output (stdout and stderr) to the temporary files.
    taskset -c 32-63 $PROG_A $ARGS "$problem_path" > tmp_a.txt 2>&1 &
    taskset -c 32-63 $PROG_B $ARGS "$problem_path" > tmp_b.txt 2>&1 &
    taskset -c 32-63 $PROG_C $ARGS "$problem_path" > tmp_c.txt 2>&1 &
    taskset -c 32-63 $PROG_D $ARGS "$problem_path" > tmp_d.txt 2>&1 &
    taskset -c 32-63 $PROG_E $ARGS "$problem_path" > tmp_e.txt 2>&1 &
    taskset -c 32-63 $PROG_F $ARGS "$problem_path" > tmp_f.txt 2>&1 &
    taskset -c 32-63 $PROG_G $ARGS "$problem_path" > tmp_g.txt 2>&1 &

    # WAIT pauses the script until all background processes (&) are finished
    wait

    # Extract the results from the temp files using our helper function
    mem_a=$(extract_mem tmp_a.txt)
    mem_b=$(extract_mem tmp_b.txt)
    mem_c=$(extract_mem tmp_c.txt)
    mem_d=$(extract_mem tmp_d.txt)
    mem_e=$(extract_mem tmp_e.txt)
    mem_f=$(extract_mem tmp_f.txt)
    mem_g=$(extract_mem tmp_g.txt)

    # Clean up the temporary files for the next loop
    rm -f tmp_a.txt tmp_b.txt tmp_c.txt tmp_d.txt tmp_e.txt tmp_f.txt tmp_g.txt

    # Append the results to the CSV output file
    echo "$problem,$mem_a,$mem_b,$mem_c,$mem_d,$mem_e,$mem_f,$mem_g" >> "$OUTPUT_FILE"

done < "$INPUT_FILE"

echo "Done! Results are saved in $OUTPUT_FILE"