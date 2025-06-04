/***********************************************************************
ElevationColorMap - Class to represent elevation color maps for
topographic maps.
Copyright (c) 2014-2024 Oliver Kreylos

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

#include "ElevationColorMap.h"

#include <string>
#include <Misc/StdError.h>
#include <Misc/FileNameExtensions.h>
#include <IO/ValueSource.h>
#include <IO/OpenFile.h>
#include <Math/Math.h>
#include <GL/gl.h>
#include <GL/GLContextData.h>

#include "Types.h"
#include "TextureTracker.h"
#include "DepthImageRenderer.h"

#include "Config.h"

/**********************************
Methods of class ElevationColorMap:
**********************************/

ElevationColorMap::ElevationColorMap(const char* heightMapName)
	:colorCyclingEnabled(false),colorCycleSpeed(1.0f),colorCyclePhase(0.0f),originalColors(0)
	{
	/* Load the given height map: */
	load(heightMapName);
	}

ElevationColorMap::~ElevationColorMap(void)
	{
	/* Clean up allocated memory: */
	delete[] originalColors;
	}

void ElevationColorMap::initContext(GLContextData& contextData) const
	{
	/* Initialize required OpenGL extensions: */
	TextureTracker::initExtensions();
	
	/* Create the data item and associate it with this object: */
	DataItem* dataItem=new DataItem;
	contextData.addDataItem(this,dataItem);
	}

void ElevationColorMap::load(const char* heightMapName)
	{
	/* Save the current color cycling state: */
	bool wasCycling = colorCyclingEnabled;
	float savedSpeed = colorCycleSpeed;
	
	/* Clean up any existing original colors backup: */
	delete[] originalColors;
	originalColors=0;
	
	/* Temporarily disable color cycling during load: */
	colorCyclingEnabled = false;
	
	/* Open the height map file: */
	std::string fullHeightMapName;
	if(heightMapName[0]=='/')
		{
		/* Use the absolute file name directly: */
		fullHeightMapName=heightMapName;
		}
	else
		{
		/* Assemble a file name relative to the configuration file directory: */
		fullHeightMapName=CONFIG_CONFIGDIR;
		fullHeightMapName.push_back('/');
		fullHeightMapName.append(heightMapName);
		}
	
	/* Open the height map file: */
	IO::ValueSource heightMapSource(IO::openFile(fullHeightMapName.c_str()));
	
	/* Load the height color map: */
	std::vector<Color> heightMapColors;
	std::vector<GLdouble> heightMapKeys;
	if(Misc::hasCaseExtension(heightMapName,".cpt"))
		{
		heightMapSource.setPunctuation("\n");
		heightMapSource.skipWs();
		int line=1;
		while(!heightMapSource.eof())
			{
			/* Read the next color map key value: */
			heightMapKeys.push_back(GLdouble(heightMapSource.readNumber()));
			
			/* Read the next color map color value: */
			Color color;
			for(int i=0;i<3;++i)
				color[i]=Color::Scalar(heightMapSource.readNumber()/255.0);
			color[3]=Color::Scalar(1);
			heightMapColors.push_back(color);
			
			if(!heightMapSource.isLiteral('\n'))
				throw Misc::makeStdErr(__PRETTY_FUNCTION__,"Color map format error in line %d of file %s",line,fullHeightMapName.c_str());
			++line;
			}
		}
	else
		{
		heightMapSource.setPunctuation(",\n");
		heightMapSource.skipWs();
		int line=1;
		while(!heightMapSource.eof())
			{
			/* Read the next color map key value: */
			heightMapKeys.push_back(GLdouble(heightMapSource.readNumber()));
			if(!heightMapSource.isLiteral(','))
				throw Misc::makeStdErr(__PRETTY_FUNCTION__,"Color map format error in line %d of file %s",line,fullHeightMapName.c_str());
			
			/* Read the next color map color value: */
			Color color;
			for(int i=0;i<3;++i)
				color[i]=Color::Scalar(heightMapSource.readNumber());
			color[3]=Color::Scalar(1);
			heightMapColors.push_back(color);
			
			if(!heightMapSource.isLiteral('\n'))
				throw Misc::makeStdErr(__PRETTY_FUNCTION__,"Color map format error in line %d of file %s",line,fullHeightMapName.c_str());
			++line;
			}
		}
	
	/* Create the color map: */
	setColors(heightMapKeys.size(),&heightMapColors[0],&heightMapKeys[0],256);
	
	/* Invalidate the color map texture object: */
	++textureVersion;
	
	/* Restore color cycling if it was enabled: */
	if(wasCycling)
		{
		setColorCycling(true, savedSpeed);
		}
	}

void ElevationColorMap::calcTexturePlane(const Plane& basePlane)
	{
	/* Scale and offset the camera-space base plane equation: */
	const Plane::Vector& bpn=basePlane.getNormal();
	Scalar bpo=basePlane.getOffset();
	Scalar hms=Scalar(getNumEntries()-1)/((getScalarRangeMax()-getScalarRangeMin())*Scalar(getNumEntries()));
	Scalar hmo=Scalar(0.5)/Scalar(getNumEntries())-hms*getScalarRangeMin();
	for(int i=0;i<3;++i)
		texturePlaneEq[i]=GLfloat(bpn[i]*hms);
	texturePlaneEq[3]=GLfloat(-bpo*hms+hmo);
	}

void ElevationColorMap::calcTexturePlane(const DepthImageRenderer* depthImageRenderer)
	{
	/* Calculate texture plane based on the given depth image renderer's base plane: */
	calcTexturePlane(depthImageRenderer->getBasePlane());
	}

void ElevationColorMap::uploadTexturePlane(Shader& shader) const
	{
	/* Upload the texture mapping plane equation: */
	shader.uploadUniform4v(1,texturePlaneEq);
	}

GLint ElevationColorMap::bindTexture(GLContextData& contextData,TextureTracker& textureTracker) const
	{
	/* Retrieve the data item: */
	DataItem* dataItem=contextData.retrieveDataItem<DataItem>(this);
	
	/* Bind the elevation texture to the next available texture unit: */
	GLint unit=textureTracker.bindTexture(GL_TEXTURE_1D,dataItem->textureObjectId);
	
	/* Check if the color map texture is outdated: */
	if(dataItem->textureObjectVersion!=textureVersion)
		{
		/* Upload the new color map into the texture: */
		glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexImage1D(GL_TEXTURE_1D,0,GL_RGB8,getNumEntries(),0,GL_RGBA,GL_FLOAT,getColors());
		
		dataItem->textureObjectVersion=textureVersion;
		}
	
	return unit;
	}

void ElevationColorMap::setColorCycling(bool enabled, float speed)
	{
	if(enabled&&!colorCyclingEnabled)
		{
		/* Starting color cycling - backup the original colors: */
		int numEntries=getNumEntries();
		if(originalColors==0)
			{
			originalColors=new GLfloat[numEntries*4];
			const Color* colors=getColors();
			const GLfloat* colorData=reinterpret_cast<const GLfloat*>(colors);
			for(int i=0;i<numEntries*4;++i)
				originalColors[i]=colorData[i];
			}
		}
	else if(!enabled&&colorCyclingEnabled)
		{
		/* Stopping color cycling - restore the original colors: */
		if(originalColors!=0)
			{
			int numEntries=getNumEntries();
			Color* colors=const_cast<Color*>(getColors());
			GLfloat* colorData=reinterpret_cast<GLfloat*>(colors);
			for(int i=0;i<numEntries*4;++i)
				colorData[i]=originalColors[i];
			
			/* Invalidate the texture to force re-upload: */
			++textureVersion;
			}
		}
	
	colorCyclingEnabled=enabled;
	colorCycleSpeed=speed;
	
	/* Reset phase when enabling: */
	if(enabled)
		colorCyclePhase=0.0f;
	}

void ElevationColorMap::updateColorCycle(float animationTime)
	{
	if(!colorCyclingEnabled||originalColors==0)
		return;
	
	/* Update the phase based on animation time and speed: */
	colorCyclePhase=animationTime*colorCycleSpeed;
	colorCyclePhase=colorCyclePhase-Math::floor(colorCyclePhase); // Keep in range [0,1)
	
	/* Calculate the shift amount in color entries: */
	int numEntries=getNumEntries();
	int shiftAmount=int(colorCyclePhase*float(numEntries));
	
	/* Update the color map with cycled colors: */
	Color* colors=const_cast<Color*>(getColors());
	GLfloat* colorData=reinterpret_cast<GLfloat*>(colors);
	for(int i=0;i<numEntries;++i)
		{
		/* Calculate source index with circular wrapping: */
		int srcIndex=(i+shiftAmount)%numEntries;
		
		/* Copy RGBA values: */
		for(int j=0;j<4;++j)
			colorData[i*4+j]=originalColors[srcIndex*4+j];
		}
	
	/* Invalidate the texture to force re-upload: */
	++textureVersion;
	}
