# 🎮 AR Sandbox Control Pipe - The Fun Way to Control Your Sandbox!

Hey there, sandbox enthusiast! 👋 

So you've got your AR Sandbox up and running, and now you want to control it like a boss without restarting the whole thing every time? You've come to the right place! The control pipe is like a remote control for your sandbox - you can change settings on the fly while it's running!

## 🚀 Quick Start

First, make sure you start your sandbox with the control pipe enabled:

```bash
./bin/SARndbox -uhm -fpv -cp /tmp/SARndbox.pipe
```

That `-cp /tmp/SARndbox.pipe` part creates a "pipe" (think of it as a mailbox) where you can send commands.

## 📨 Sending Commands

Once your sandbox is running, you can send it commands from another terminal like this:

```bash
echo "command parameters" > /tmp/SARndbox.pipe
```

Easy peasy! 🍋

## 🎨 Cool Stuff You Can Do

### Change Color Schemes
Bored of the default colors? Switch it up!

```bash
# Go neon!
echo "colorMap NeonRainbow.cpt" > /tmp/SARndbox.pipe

# Classic rainbow
echo "colorMap ClassicRainbow.cpt" > /tmp/SARndbox.pipe

# Feeling cosmic?
echo "colorMap NebulaRealm.cpt" > /tmp/SARndbox.pipe

# Pastel vibes
echo "colorMap PastelRainbow.cpt" > /tmp/SARndbox.pipe
```

### Water Simulation Fun 💧
Make it rain, change water colors, or just play with physics!

```bash
# Make it rain harder!
echo "rainStrength 0.8" > /tmp/SARndbox.pipe

# Speed up the water flow
echo "waterSpeed 2.0" > /tmp/SARndbox.pipe

# Change water color (RGB values 0-1)
echo "waterColor 0.2 0.6 1.0" > /tmp/SARndbox.pipe

# Make water more transparent
echo "waterAttenuation 0.5" > /tmp/SARndbox.pipe

# Change how water absorbs into sand
echo "waterAbsorption 0.3" > /tmp/SARndbox.pipe

# Control evaporation
echo "evaporationRate 0.001" > /tmp/SARndbox.pipe
```

### Contour Lines 📏
Want those nice topographic lines? Control them!

```bash
# Turn on contour lines
echo "useContourLines on" > /tmp/SARndbox.pipe

# Change spacing (in cm)
echo "contourLineSpacing 5.0" > /tmp/SARndbox.pipe

# Turn them off
echo "useContourLines off" > /tmp/SARndbox.pipe
```

### 🌈 Texture Warping (New Hotness!)
This is the really cool new feature - you can load images and have them warp with your terrain!

```bash
# Load a texture (use any image file)
echo "loadWarpTexture /path/to/your/cool/image.png" > /tmp/SARndbox.pipe

# Turn on texture warping
echo "useWarpTexture on" > /tmp/SARndbox.pipe

# Control how much it warps (0-1)
echo "warpIntensity 0.7" > /tmp/SARndbox.pipe

# Change the texture size
echo "textureScale 0.02" > /tmp/SARndbox.pipe

# Set how it blends (0=multiply, 1=overlay, 2=add, 3=replace)
echo "textureBlendMode 1" > /tmp/SARndbox.pipe

# Make it more transparent
echo "textureOpacity 0.6" > /tmp/SARndbox.pipe

# Change warp mode (0=follow contours, 1=radial, 2=flow)
echo "warpMode 0" > /tmp/SARndbox.pipe
```

### 🏔️ Geological Features
Want to simulate rock layers? We got you!

```bash
# Create dipping beds (angle strike dip thickness)
echo "dippingBed 45 0 30 5" > /tmp/SARndbox.pipe

# Or folded beds
echo "foldedDippingBed 45 0 30 10 2" > /tmp/SARndbox.pipe

# Change bed thickness
echo "dippingBedThickness 8" > /tmp/SARndbox.pipe
```

### 🎯 Color Cycling Animation
Make your sandbox psychedelic!

```bash
# Start color cycling (speed in degrees/second)
echo "colorCycle 30" > /tmp/SARndbox.pipe

# Stop the madness
echo "colorCycle 0" > /tmp/SARndbox.pipe
```

## 🛠️ Pro Tips

1. **Chain Commands**: You can send multiple commands quickly:
   ```bash
   echo "waterSpeed 3.0" > /tmp/SARndbox.pipe
   echo "rainStrength 1.0" > /tmp/SARndbox.pipe
   echo "waterColor 0 0.8 0.8" > /tmp/SARndbox.pipe
   ```

2. **Make Scripts**: Check out the example scripts in the `bin/` folder:
   - `neon.sh` - Quick neon colors
   - `texture-warp-test.sh` - Full texture warping demo
   - `next-world.sh` - Cycle through different "worlds"

3. **Pipe Location**: You can use any path for your pipe, just make sure both the sandbox and your commands use the same path!

4. **Troubleshooting**: 
   - If commands aren't working, check if the pipe exists: `ls -la /tmp/SARndbox.pipe`
   - Make sure the sandbox was started with `-cp` option
   - Check the sandbox terminal for error messages

## 🎪 Have Fun!

The control pipe turns your AR Sandbox into a live performance tool. Change settings on the fly during presentations, create dynamic demonstrations, or just have fun experimenting!

Remember: if you mess something up, just restart the sandbox - it won't bite! 🦈

Happy sandboxing! 🏖️