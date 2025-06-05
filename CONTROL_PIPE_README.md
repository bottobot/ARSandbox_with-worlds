# AR Sandbox Control Pipe Documentation

## Overview

The control pipe feature enables runtime configuration changes to the AR Sandbox without requiring application restart. This interface allows dynamic modification of visualization parameters, water simulation settings, and texture rendering options through a named pipe mechanism.

## Quick Start

Initialize the AR Sandbox with control pipe functionality:

```bash
./SARndbox -uhm -fpv -cp pipe.fifo
```

The `-cp pipe.fifo` parameter creates a named pipe for command communication. This assumes you're running SARndbox from the bin/ directory where the pipe.fifo file will be created.

## Command Interface

Send commands to the running sandbox instance via the named pipe:

```bash
echo "command parameters" > pipe.fifo
```

## Available Commands

### Color Map Configuration

Modify the terrain color scheme during runtime:

```bash
# Neon color scheme
echo "colorMap NeonRainbow.cpt" > pipe.fifo

# Classic rainbow gradient
echo "colorMap ClassicRainbow.cpt" > pipe.fifo

# Nebula color scheme
echo "colorMap NebulaRealm.cpt" > pipe.fifo

# Pastel color scheme
echo "colorMap PastelRainbow.cpt" > pipe.fifo
```

### Water Simulation Parameters

Control water physics and rendering properties:

```bash
# Adjust rain intensity (0.0-1.0)
echo "rainStrength 0.8" > pipe.fifo

# Modify water flow speed multiplier
echo "waterSpeed 2.0" > pipe.fifo

# Set water color (RGB values 0-1)
echo "waterColor 0.2 0.6 1.0" > pipe.fifo

# Control water transparency
echo "waterAttenuation 0.5" > pipe.fifo

# Configure water absorption rate
echo "waterAbsorption 0.3" > pipe.fifo

# Set evaporation rate
echo "evaporationRate 0.001" > pipe.fifo
```

### Contour Line Settings

Configure topographic contour line display:

```bash
# Enable contour lines
echo "useContourLines on" > pipe.fifo

# Set contour line spacing (in centimeters)
echo "contourLineSpacing 5.0" > pipe.fifo

# Disable contour lines
echo "useContourLines off" > pipe.fifo
```

### Texture Warping System

Apply and control image textures that deform with terrain geometry:

```bash
# Load texture from file
echo "loadWarpTexture /path/to/your/image.png" > pipe.fifo

# Enable texture warping
echo "useWarpTexture on" > pipe.fifo

# Set warp intensity (0.0-1.0)
echo "warpIntensity 0.7" > pipe.fifo

# Adjust texture scale factor
echo "textureScale 0.02" > pipe.fifo

# Configure blend mode (0=multiply, 1=overlay, 2=add, 3=replace)
echo "textureBlendMode 1" > pipe.fifo

# Set texture opacity (0.0-1.0)
echo "textureOpacity 0.6" > pipe.fifo

# Select warp mode (0=follow contours, 1=radial, 2=flow)
echo "warpMode 0" > pipe.fifo
```

### Geological Feature Simulation

Create geological structure visualizations:

```bash
# Generate dipping beds (angle strike dip thickness)
echo "dippingBed 45 0 30 5" > pipe.fifo

# Create folded beds (angle strike dip thickness fold_count)
echo "foldedDippingBed 45 0 30 10 2" > pipe.fifo

# Modify bed thickness parameter
echo "dippingBedThickness 8" > pipe.fifo
```

### Color Cycling Animation

Enable animated color transitions:

```bash
# Start color cycling (enable flag followed by speed in degrees/second)
echo "colorCycle 1 30" > pipe.fifo

# Stop color cycling
echo "colorCycle 0" > pipe.fifo
```

## Implementation Notes

1. **Command Sequencing**: Multiple commands can be sent in succession:
   ```bash
   echo "waterSpeed 3.0" > pipe.fifo
   echo "rainStrength 1.0" > pipe.fifo
   echo "waterColor 0 0.8 0.8" > pipe.fifo
   ```

2. **Example Scripts**: Reference implementations are provided in the `bin/` directory:
   - `neon.sh` - Applies neon color configuration
   - `texture-warp-test.sh` - Demonstrates texture warping functionality
   - `next-world.sh` - Cycles through predefined visualization presets

3. **Pipe Location**: The default pipe location is `pipe.fifo` in the bin/ directory. When running SARndbox from the bin/ directory, use this relative path. The pipe path can be customized if needed, but ensure consistency between the sandbox initialization and command execution.

4. **Troubleshooting**:
   - Verify pipe existence (from bin/ directory): `ls -la pipe.fifo`
   - Confirm sandbox was started with `-cp pipe.fifo` parameter
   - Monitor sandbox terminal output for error messages
   - Ensure all commands are executed from the bin/ directory

## Technical Details

The control pipe interface provides real-time parameter modification capabilities for AR Sandbox installations, enabling dynamic presentations and interactive demonstrations without interrupting the visualization process.