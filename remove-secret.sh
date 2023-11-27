#!/bin/bash

# Check if secrets.txt exists
if [ ! -f secrets.txt ]; then
    echo "secrets.txt not found"
    exit 1
fi

# Loop through each line in secrets.txt
while IFS= read -r line
do
    # Use find to iterate over all files in the current directory and subdirectories
    # It's important to exclude binary files to avoid corrupting them
    find . -type f -not -path '*/\.*' -exec grep -Il '' {} \; | while read -r file
    do
        # Use sed to replace the secret line with ***REMOVED***
        # -i for in-place editing, '' is necessary for macOS compatibility
        sed -i '' "s/${line//\//\\/}/***REMOVED***/g" "$file"
    done
done < secrets.txt
