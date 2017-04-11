#pragma once

#include <KubeEngine.h>
#include "noise/noise.h"
#include "noise/noiseutils.h"

class Sample {
public:
	void run();

	void onChunkLoad(Chunk * chunk);


private:
	ChunkManager* m_chunkManager;
	Engine* m_engine;
	Camera* m_camera;

	module::Perlin noiseGenerator;
	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane heightMapBuilder;
};
