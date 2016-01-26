#ifndef _MODEL_LOADER_H_
#define _MODEL_LOADER_H_
#pragma once
#include <string>
#include "Mesh.h"
#include "General.h"

class ModelLoader
{
public:
	ModelLoader();
	~ModelLoader();

	Mesh* LoadModel(std::string filename);
};

#endif
