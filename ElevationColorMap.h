/***********************************************************************
ElevationColorMap - Class to represent elevation color maps for
topographic maps.
Copyright (c) 2014-2023 Oliver Kreylos

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

#ifndef ELEVATIONCOLORMAP_INCLUDED
#define ELEVATIONCOLORMAP_INCLUDED

#include <GL/gl.h>
#include <GL/GLColorMap.h>
#include <GL/GLTextureObject.h>

#include "Types.h"

/* Forward declarations: */
class TextureTracker;
class Shader;
class DepthImageRenderer;

class ElevationColorMap:public GLColorMap,public GLTextureObject
	{
	/* Elements: */
	private:
	GLfloat texturePlaneEq[4]; // Texture mapping plane equation in GLSL-compatible format
	
	/* Color cycling state: */
	bool colorCyclingEnabled; // Flag to enable/disable color cycling
	float colorCycleSpeed; // Speed of color cycling in cycles per second
	float colorCyclePhase; // Current phase of color cycling (0.0 to 1.0)
	GLfloat* originalColors; // Backup of original colors for restoration
	
	/* Constructors and destructors: */
	public:
	ElevationColorMap(const char* heightMapName); // Creates an elevation color map by loading the given height map file
	~ElevationColorMap(); // Destructor
	
	/* Methods from GLObject: */
	virtual void initContext(GLContextData& contextData) const;
	
	/* New methods: */
	void load(const char* heightMapName); // Overrides elevation color map by loading the given height map file
	void calcTexturePlane(const Plane& basePlane); // Calculates the texture mapping plane for the given base plane equation
	void calcTexturePlane(const DepthImageRenderer* depthImageRenderer); // Calculates the texture mapping plane for the given depth image renderer
	void uploadTexturePlane(Shader& shader) const; // Uploads the texture mapping plane equation into a GLSL 4-vector at the next uniform location in the given shader
	GLint bindTexture(GLContextData& contextData,TextureTracker& textureTracker) const; // Binds the elevation color map texture object to the next available texture unit and returns the unit's index
	
	/* Color cycling methods: */
	void setColorCycling(bool enabled, float speed=1.0f); // Enable/disable color cycling with optional speed
	void updateColorCycle(float animationTime); // Update color cycling based on animation time
	bool isColorCycling() const { return colorCyclingEnabled; } // Check if color cycling is enabled
	};

#endif
