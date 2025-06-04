#!/bin/bash
# Texture warping test script for AR Sandbox
# Tests the contour-based texture warping system

PIPE="${1:-/tmp/SARndbox.pipe}"

echo "Starting texture warping test..."

# Check if pipe exists
if [ ! -p "$PIPE" ]; then
    echo "Error: Control pipe $PIPE does not exist"
    echo "Make sure AR Sandbox is running with control pipe enabled"
    exit 1
fi

echo "Loading test texture..."

# Load a texture (you'll need to provide an actual texture file)
# For testing, you can use any image file
echo "loadWarpTexture /usr/share/pixmaps/debian-logo.png" > "$PIPE"
sleep 0.5

echo "Configuring texture warping parameters..."

# Set texture scale (adjust based on your sandbox size)
echo "textureScale 0.01" > "$PIPE"
sleep 0.1

# Set warping intensity (0.0-1.0)
echo "warpIntensity 0.5" > "$PIPE"
sleep 0.1

# Set gradient threshold
echo "gradientThreshold 0.1" > "$PIPE"
sleep 0.1

# Set warp mode (0=contour follow, 1=radial, 2=flow)
echo "warpMode 0" > "$PIPE"
sleep 0.1

# Set blend mode (0=multiply, 1=overlay, 2=add, 3=replace)
echo "textureBlendMode 0" > "$PIPE"
sleep 0.1

# Set texture opacity
echo "textureOpacity 0.8" > "$PIPE"
sleep 0.1

echo "Enabling texture warping..."

# Enable the warped texture
echo "useWarpTexture on" > "$PIPE"

echo "Texture warping enabled!"
echo ""
echo "You should now see the texture following the terrain contours."
echo "Try creating different terrain features to see the warping effect:"
echo "  - Hills and valleys for radial stretching"
echo "  - Ridges for linear stretching along contours"
echo "  - Steep slopes for compression effects"
echo ""
echo "To disable texture warping, run:"
echo "  echo 'useWarpTexture off' > $PIPE"