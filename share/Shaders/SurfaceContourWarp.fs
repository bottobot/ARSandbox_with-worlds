/***********************************************************************
SurfaceContourWarp - Shader fragment to apply contour-based texture
warping to the surface, making textures follow terrain contours.
Copyright (c) 2025 Oliver Kreylos

This file is part of the Augmented Reality Sandbox (SARndbox).

The Augmented Reality Sandbox is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The Augmented Reality Sandbox is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License along
with the Augmented Reality Sandbox; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
***********************************************************************/

#extension GL_ARB_texture_rectangle : enable

uniform sampler2D warpTexture;
uniform sampler2DRect depthSampler;
uniform bool useWarpTexture;
uniform float warpIntensity;
uniform float textureScale;
uniform float gradientThreshold;
uniform int warpMode;
uniform int textureBlendMode;
uniform float textureOpacity;

varying vec2 lastWarpedCoords; // Store for water integration

// Get elevation at a specific position
float getElevation(vec2 pos) {
	return texture2DRect(depthSampler, pos).r;
}

// Calculate elevation gradient using finite differences
vec2 calculateGradient(vec2 pos) {
	float delta = 2.0; // Sampling distance in pixels
	
	// Get elevation at 4 neighboring points
	float e_left  = getElevation(pos + vec2(-delta, 0.0));
	float e_right = getElevation(pos + vec2(+delta, 0.0));
	float e_down  = getElevation(pos + vec2(0.0, -delta));
	float e_up    = getElevation(pos + vec2(0.0, +delta));
	
	// Calculate partial derivatives (finite differences)
	float dE_dx = (e_right - e_left) / (2.0 * delta);
	float dE_dy = (e_up - e_down) / (2.0 * delta);
	
	return vec2(dE_dx, dE_dy);
}

// Main warping function for contour-following mode
vec2 warpTextureCoordinates(
	vec2 baseCoord,      // Original texture coordinate
	vec2 gradient,       // Elevation gradient at this point
	float elevation,     // Current elevation
	float warpIntensity  // User-controlled intensity (0-1)
) {
	// Step 1: Calculate gradient magnitude (steepness)
	float gradientMagnitude = length(gradient);
	
	// Step 2: Apply threshold to avoid warping on flat areas
	if (gradientMagnitude < gradientThreshold) {
		return baseCoord; // No warping on flat areas
	}
	
	// Step 3: Calculate warping strength based on gradient
	float warpStrength = smoothstep(
		gradientThreshold,  // Start warping
		1.0,                // Maximum warping at gradient = 1
		gradientMagnitude
	) * warpIntensity;
	
	// Step 4: Get normalized directions
	vec2 gradientDir = gradient / gradientMagnitude;
	vec2 contourDir = vec2(-gradientDir.y, gradientDir.x);
	
	// Step 5: Calculate displacement
	vec2 displacement = vec2(0.0);
	
	// Stretch along contour lines (perpendicular to gradient)
	displacement += contourDir * warpStrength * 0.1;
	
	// Compress along gradient direction (up/down slope)
	displacement -= gradientDir * warpStrength * 0.05;
	
	// Step 6: Apply elevation-based scaling
	float elevationScale = 1.0 + (elevation / 100.0) * warpIntensity * 0.1;
	
	// Step 7: Combine all effects
	vec2 warpedCoord = baseCoord + displacement;
	warpedCoord *= elevationScale;
	
	return warpedCoord;
}

// Flow mode - texture appears to flow downhill
vec2 flowWarp(vec2 baseCoord, vec2 gradient, float elevation, float waterAnimationTime) {
	vec2 flowDir = -normalize(gradient);
	float flowStrength = length(gradient) * warpIntensity;
	float flowTime = waterAnimationTime * 0.1;
	
	return baseCoord + flowDir * flowStrength * sin(elevation + flowTime);
}

// Radial mode - warp radiating from peaks and valleys
vec2 radialWarp(vec2 baseCoord, vec2 gradient, float elevation, vec2 vertexPosition) {
	// Simple radial implementation - would need more complex analysis for proper peaks
	float gradientMagnitude = length(gradient);
	vec2 radialDir = normalize(baseCoord - vec2(0.5));
	float radialStrength = gradientMagnitude * warpIntensity * 0.1;
	
	return baseCoord + radialDir * radialStrength;
}

// Sample the warped texture
vec4 sampleWarpedTexture() {
	// Get current fragment position and elevation
	vec2 fragPos = gl_FragCoord.xy;
	float elevation = getElevation(fragPos);
	
	// Calculate gradient at this position
	vec2 gradient = calculateGradient(fragPos);
	
	// Calculate base texture coordinates
	vec2 baseCoord = fragPos * textureScale;
	
	// Apply warping based on mode
	vec2 warpedCoord;
	if (warpMode == 0) {
		// Contour follow mode
		warpedCoord = warpTextureCoordinates(baseCoord, gradient, elevation, warpIntensity);
	} else if (warpMode == 1) {
		// Radial mode
		warpedCoord = radialWarp(baseCoord, gradient, elevation, fragPos);
	} else if (warpMode == 2) {
		// Flow mode (requires water animation time)
		// Note: waterAnimationTime would need to be passed as uniform
		warpedCoord = flowWarp(baseCoord, gradient, elevation, 0.0);
	} else {
		// Default to no warping
		warpedCoord = baseCoord;
	}
	
	// Store warped coordinates for water integration
	lastWarpedCoords = warpedCoord;
	
	// Sample the texture
	return texture2D(warpTexture, warpedCoord);
}

// Apply texture warping to base color
void applyTextureWarping(inout vec4 baseColor) {
	if (!useWarpTexture) {
		return;
	}
	
	// Sample the warped texture
	vec4 texColor = sampleWarpedTexture();
	
	// Apply blend mode
	if (textureBlendMode == 0) {
		// Multiply blend - preserves elevation colors
		baseColor.rgb *= texColor.rgb;
	}
	else if (textureBlendMode == 1) {
		// Overlay blend - balanced mix
		vec3 overlayColor = mix(
			2.0 * baseColor.rgb * texColor.rgb,
			1.0 - 2.0 * (1.0 - baseColor.rgb) * (1.0 - texColor.rgb),
			step(0.5, baseColor.rgb)
		);
		baseColor.rgb = mix(baseColor.rgb, overlayColor, textureOpacity);
	}
	else if (textureBlendMode == 2) {
		// Additive blend - brightens
		baseColor.rgb += texColor.rgb * textureOpacity * 0.5;
	}
	else if (textureBlendMode == 3) {
		// Replace blend - full texture
		baseColor = mix(baseColor, texColor, textureOpacity);
	}
}