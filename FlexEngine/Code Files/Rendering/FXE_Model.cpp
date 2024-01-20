#include "FXE_Model.h"
#include "FXE_RendererManager.h"

Model::Model()
{
	FXE::ModelManager.emplace_back(this);
}
