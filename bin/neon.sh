#!/bin/bash
# Improved neon world script with error handling

# Use non-blocking write with timeout
if command -v timeout >/dev/null 2>&1; then
    # Use timeout command if available
    timeout 1s bash -c 'echo "colormap NeonRainbow.cpt" > pipe.fifo' 2>/dev/null
else
    # Fallback: try to write with error handling
    echo "colormap NeonRainbow.cpt" > pipe.fifo 2>/dev/null || true
fi

# Exit successfully regardless of write result
exit 0