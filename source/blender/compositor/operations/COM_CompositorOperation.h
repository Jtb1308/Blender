/*
 * Copyright 2011, Blender Foundation.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Contributor:
 *		Jeroen Bakker
 *		Monique Dewanchand
 */

#ifndef _COM_CompositorOperation_h
#define _COM_CompositorOperation_h
#include "COM_NodeOperation.h"
#include "DNA_scene_types.h"
#include "BLI_rect.h"

/**
 * @brief Compositor output operation
 */
class CompositorOperation : public NodeOperation {
private:
	/**
	 * @brief local reference to the scene
	 */
	const RenderData *rd;

	/**
	 * @brief reference to the output float buffer
	 */
	float *outputBuffer;

	/**
	 * @brief local reference to the input image operation
	 */
	SocketReader *imageInput;

	/**
	 * @brief local reference to the input alpha operation
	 */
	SocketReader *alphaInput;
public:
	CompositorOperation();
	void executeRegion(rcti *rect, unsigned int tileNumber, MemoryBuffer **memoryBuffers);
	void setRenderData(const RenderData *rd) { this->rd = rd; }
	bool isOutputOperation(bool rendering) const { return true; }
	void initExecution();
	void deinitExecution();
	const CompositorPriority getRenderPriority() const { return COM_PRIORITY_MEDIUM; }
	void determineResolution(unsigned int resolution[], unsigned int preferredResolution[]);
};
#endif
