#!/bin/bash

# chmod +x convert_ini.sh
# Define the input and output files
inputFile="imgui.ini"
outputFile="ini_str.txt"

# Clear the output file
> "$outputFile"

# Write text to the file
while IFS= read -r line
do
echo "\"$line\\n\"" >> "$outputFile"
done < "$inputFile"

# Confirm the write operation
if [ $? -eq 0 ]; then
    echo "Text written to $outputFile successfully."
else
    echo "Failed to write to $outputFile."
fi
