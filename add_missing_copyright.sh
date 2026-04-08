#!/bin/bash

#
# Add Missing Copyright Headers Script for SPIERS
#
# This script adds GPL copyright headers to .h and .cpp files that don't already have them.
# The header is inserted at the beginning of each file.
#
# Usage: bash add_missing_copyright.sh [--no-backup]
#   --no-backup : Do not create backup files (default: creates .bak backups)
#

# Parse command line arguments
BACKUP=true
if [[ "$1" == "--no-backup" ]]; then
    BACKUP=false
fi

# Change to script directory (SPIERS root)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR" || exit 1

echo "=========================================="
echo "SPIERS Missing Copyright Headers Tool"
echo "=========================================="
echo "Adding GPL boilerplate to files without headers"
echo ""

# Find all .h and .cpp files in src directories
FILES_FOUND=$(find . -name "src" -type d)

if [[ -z "$FILES_FOUND" ]]; then
    echo "Error: No src/ directories found."
    exit 1
fi

# The standard copyright header template
read -r -d '' COPYRIGHT_TEMPLATE << 'EOF'
/**
 * @file
 * %FILE_DESC%
 *
 * All SPIERS code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERS code is Copyright 2008-2026 by Russell J. Garwood, Mark D. Sutton,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

EOF

# Count files
TOTAL_FILES=0
FILES_WITH_HEADER=0
FILES_ADDED_HEADER=0
FILES_SKIPPED=0

for src_dir in $FILES_FOUND; do
    echo "Processing: $src_dir/"

    for file in "$src_dir"/*.h "$src_dir"/*.cpp; do
        # Skip if file doesn't exist (glob pattern matched nothing)
        [[ -e "$file" ]] || continue

        TOTAL_FILES=$((TOTAL_FILES + 1))
        filename=$(basename "$file")
        filepath="$file"

        # Check if file already has GPL header
        if head -1 "$file" | grep -q "^/\*\*"; then
            FILES_WITH_HEADER=$((FILES_WITH_HEADER + 1))
            continue
        fi

        # Check if file is empty or only whitespace
        if [[ ! -s "$file" ]] || ! grep -q '[^ \t\n]' "$file"; then
            FILES_SKIPPED=$((FILES_SKIPPED + 1))
            echo "  ⊘ Skipped (empty): $filename"
            continue
        fi

        # Determine file description based on extension and name
        if [[ "$filename" == *.h ]]; then
            # Convert filename to description
            # e.g., brush.h -> "Header: Brush"
            base="${filename%.h}"
            # Capitalize first letter
            desc=$(echo "$base" | sed 's/\b\(.\)/\u\1/g')
            file_desc="Header: $desc"
        else
            # e.g., brush.cpp -> "Source: Brush"
            base="${filename%.cpp}"
            desc=$(echo "$base" | sed 's/\b\(.\)/\u\1/g')
            file_desc="Source: $desc"
        fi

        # Create the header for this file
        header="${COPYRIGHT_TEMPLATE//%FILE_DESC%/$file_desc}"

        # Create backup if requested
        if [[ "$BACKUP" == true ]]; then
            cp "$file" "$file.bak"
        fi

        # Add header to file: write header, then append original content
        {
            echo "$header"
            cat "$file"
        } > "$file.tmp"

        mv "$file.tmp" "$file"

        FILES_ADDED_HEADER=$((FILES_ADDED_HEADER + 1))
        if [[ "$BACKUP" == true ]]; then
            echo "  ✓ Added header: $filename (backup: $filename.bak)"
        else
            echo "  ✓ Added header: $filename"
        fi
    done
done

echo ""
echo "=========================================="
echo "Summary:"
echo "  Total files scanned: $TOTAL_FILES"
echo "  Files already had headers: $FILES_WITH_HEADER"
echo "  Files with headers added: $FILES_ADDED_HEADER"
echo "  Files skipped (empty): $FILES_SKIPPED"
if [[ "$BACKUP" == true ]]; then
    echo "  Backups created: Yes (.bak files)"
else
    echo "  Backups created: No"
fi
echo "=========================================="
echo ""

if [[ $FILES_ADDED_HEADER -eq 0 ]]; then
    echo "All files already have copyright headers."
else
    echo "Copyright headers added successfully!"
    if [[ "$BACKUP" == true ]]; then
        echo ""
        echo "⚠️  IMPORTANT: Please review the added headers carefully!"
        echo "The file descriptions are auto-generated based on filenames."
        echo "You may need to manually adjust descriptions in some files."
        echo ""
        echo "To accept the changes, delete the backup files:"
        echo "  find . -name '*.bak' -delete"
        echo ""
        echo "To revert, restore from backups:"
        echo "  for f in *.bak; do mv \"\$f\" \"\${f%.bak}\"; done"
    fi
fi
