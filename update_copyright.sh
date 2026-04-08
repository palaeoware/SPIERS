#!/bin/bash

#
# Update Copyright Year Script for SPIERS
#
# This script updates all copyright notices in source files from 2008-2019 to 2008-2026.
# It recursively searches all src/ directories and updates .h and .cpp files.
#
# Usage: bash update_copyright.sh [--no-backup]
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

echo "========================================"
echo "SPIERS Copyright Year Update Script"
echo "========================================"
echo "Updating: 2008-2019 → 2008-2026"
echo ""

# Find all .h and .cpp files in src directories
FILES_FOUND=$(find . -name "src" -type d)

if [[ -z "$FILES_FOUND" ]]; then
    echo "Error: No src/ directories found."
    exit 1
fi

# Count files and changes before processing
TOTAL_FILES=0
CHANGED_FILES=0

for src_dir in $FILES_FOUND; do
    echo "Processing: $src_dir/"

    for file in "$src_dir"/*.h "$src_dir"/*.cpp; do
        # Skip if file doesn't exist (glob pattern matched nothing)
        [[ -e "$file" ]] || continue

        TOTAL_FILES=$((TOTAL_FILES + 1))

        # Check if file contains the old copyright string
        if grep -q "2008-2019" "$file"; then
            CHANGED_FILES=$((CHANGED_FILES + 1))

            if [[ "$BACKUP" == true ]]; then
                # Create backup and update file
                sed -i.bak 's/2008-2019/2008-2026/g' "$file"
                echo "  ✓ Updated: $(basename "$file") (backup: $(basename "$file").bak)"
            else
                # Update file without backup
                sed -i 's/2008-2019/2008-2026/g' "$file"
                echo "  ✓ Updated: $(basename "$file")"
            fi
        fi
    done
done

echo ""
echo "========================================"
echo "Summary:"
echo "  Total files scanned: $TOTAL_FILES"
echo "  Files updated: $CHANGED_FILES"
if [[ "$BACKUP" == true ]]; then
    echo "  Backups created: Yes (.bak files)"
else
    echo "  Backups created: No"
fi
echo "========================================"
echo ""

if [[ $CHANGED_FILES -eq 0 ]]; then
    echo "No files needed updating."
else
    echo "Copyright year update complete!"
    if [[ "$BACKUP" == true ]]; then
        echo "Backup files (.bak) have been created. Review the changes and delete them when satisfied."
    fi
fi
