#!/bin/bash

# Configuration
WORLDS_LIST="/home/dad/src/arsandbox-main/etc/worlds.list"
STATE_FILE="/tmp/arsandbox-current-world"

# Check if worlds.list exists
if [ ! -f "$WORLDS_LIST" ]; then
    echo "Error: World list file not found: $WORLDS_LIST"
    exit 1
fi

# Read all worlds into an array
readarray -t WORLDS < "$WORLDS_LIST"

# Check if we have any worlds
if [ ${#WORLDS[@]} -eq 0 ]; then
    echo "Error: No worlds found in $WORLDS_LIST"
    exit 1
fi

# Read current world index (default to 0 if file doesn't exist)
CURRENT_INDEX=0
if [ -f "$STATE_FILE" ]; then
    CURRENT_INDEX=$(cat "$STATE_FILE" 2>/dev/null || echo 0)
    # Validate that it's a number
    if ! [[ "$CURRENT_INDEX" =~ ^[0-9]+$ ]]; then
        CURRENT_INDEX=0
    fi
fi

# Calculate previous index (wrapping around)
# If current is 0, go to last index; otherwise decrement
if [ "$CURRENT_INDEX" -eq 0 ]; then
    PREV_INDEX=$(( ${#WORLDS[@]} - 1 ))
else
    PREV_INDEX=$(( CURRENT_INDEX - 1 ))
fi

# Save new index to state file
echo "$PREV_INDEX" > "$STATE_FILE"

# Get the world script path
WORLD_SCRIPT="${WORLDS[$PREV_INDEX]}"

# Check if the script exists
if [ ! -f "$WORLD_SCRIPT" ]; then
    echo "Error: World script not found: $WORLD_SCRIPT"
    exit 1
fi

# Check if the script is executable
if [ ! -x "$WORLD_SCRIPT" ]; then
    echo "Warning: World script is not executable: $WORLD_SCRIPT"
    echo "Making it executable..."
    chmod +x "$WORLD_SCRIPT"
fi

# Execute the world script
echo "Switching to world $((PREV_INDEX + 1)) of ${#WORLDS[@]}: $(basename "$WORLD_SCRIPT")"
exec "$WORLD_SCRIPT"