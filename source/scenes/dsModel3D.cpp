
#include "dsModel3D.h"
#include "nckMaterialToProgram.h"
#include "../dsUtils.h"

dsModel3D::dsModel3D(DS::Data * data) : dsItem3D(data) {
	m_ShaderProgram = NULL;
	m_Camera = NULL;
	m_Model = NULL;
	m_CameraModel = NULL;
}

dsModel3D::~dsModel3D() {

}

void dsModel3D::Parse(const JSONObject & node) {
	dsItem3D::Parse(node);

	DS::Compound * oldModel = m_Model;

	m_ShaderProgram = NULL;
	m_Model = NULL;
	m_Camera = NULL;
	m_CameraModel = NULL;

	if (node.find("shader") != node.end())
		m_ShaderProgram = m_Data->LoadProgram("shader://" + node.find("shader")->second->AsString());

	if (node.find("model") != node.end())
		m_Model = m_Data->LoadCompound("model://" + node.find("model")->second->AsString());

	if (m_Model == NULL)
		THROW_EXCEPTION("Model not defined");

	if (node.find("camera") != node.end())
	{
		if (node.find("camera")->second->IsObject()) {
			const JSONObject & cameraNode = node.find("camera")->second->AsObject();

			if (cameraNode.find("timeline") != cameraNode.end()) {
				m_CameraModel = m_Data->LoadCompound("model://" + node.find("timeline")->second->AsString());
				if (!m_CameraModel->Get()->GetCameraForKeyframe(0))
					THROW_EXCEPTION("No camera in timeline");
			}
			else {
				DS::Compound * compound = NULL;
				if (cameraNode.find("model") != cameraNode.end())
					compound = m_Data->LoadCompound("model://" + node.find("model")->second->AsString());

				if (cameraNode.find("name") != cameraNode.end())
					m_Camera = compound->Get()->GetCamera(cameraNode.find("name")->second->AsString());
			}
		}
	}

	if (m_Camera == NULL && m_CameraModel == NULL) THROW_EXCEPTION("Camera not defined");


	if (m_Model != oldModel) {
		std::vector<Scene::Material*> mats;
		m_Model->Get()->GetAllMaterials(&mats);

		Scene::MaterialToProgram mtp(m_Data->GetGraphicsDevice());

		for (int i = 0; i < mats.size(); i++) {
			Scene::Material * mat = mats[i];
			mat->SetProgram(mtp.Generate(mat));
		}
	}
}

void dsModel3D::Load() {
  
}

void dsModel3D::Update(DS::StageProxy * proxy, double start, double end, double time) {
	dsItem3D::Update(proxy, start, end, time);
}

void dsModel3D::Render(DS::StageProxy * proxy, double start, double end, double time) {
	Graph::Device * const dev = m_Data->GetGraphicsDevice();
	const double t_offset = time - start;

	const float width = m_Data->GetVirtualDisplayWidth();
	const float height = m_Data->GetVirtualDisplayHeight();

	if (!m_Camera)
		return;

	m_Camera->SetAspect(width / height);

	dev->Enable(Graph::STATE_DEPTH_TEST);

	dev->MatrixMode(Graph::MATRIX_PROJECTION);
	dev->Identity();
	m_Camera->Enable(Graph::MATRIX_PROJECTION);
	
	dev->MatrixMode(Graph::MATRIX_VIEW);
	dev->Identity();
	m_Camera->Enable(Graph::MATRIX_VIEW);

	dev->MatrixMode(Graph::MATRIX_MODEL);
	dev->Identity();

	m_Camera->GetObject()->Play(t_offset);
	m_Camera->GetObject()->Update();

	std::vector<Scene::Object*> lampObjs;
	m_Model->Get()->GetObjectsWithLayer(&lampObjs, Scene::DATABLOCK_LAMP);

	Scene::LampUniforms lUniforms;
	Scene::Lamp::GenerateUniforms(lampObjs, m_Camera->GetMatrix(), &lUniforms);

	std::vector<Scene::Material*> mats;
	m_Model->Get()->GetAllMaterials(&mats);

	for (int i = 0; i < mats.size(); i++) {
		Scene::Material * mat = mats[i];
		lUniforms.Bind(mat->GetProgram());
	}


	if (m_ShaderProgram != NULL) m_ShaderProgram->Enable();
	
	dev->PushMatrix();
	dev->Translate(m_CurrentX, m_CurrentY, m_CurrentZ);
	m_Model->Get()->Render();
	dev->PopMatrix();

	if (m_ShaderProgram != NULL) m_ShaderProgram->Disable();
}
