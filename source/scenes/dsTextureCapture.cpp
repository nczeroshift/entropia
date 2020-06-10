
#include "dsTextureCapture.h"

dsTextureCapture::dsTextureCapture(DS::Data * data) : DS::Stage(data) {
    fboManager = NULL;
    fboTexture = NULL;
}

dsTextureCapture::~dsTextureCapture() {
    SafeDelete(fboManager);
    SafeDelete(fboTexture);
}

void dsTextureCapture::Parse(const JSONObject & obj) {
	Graph::Device * const dev = m_Data->GetGraphicsDevice();

	if (obj.find("texture") != obj.end()) {
		std::string texName = "texture://"+obj.find("texture")->second->AsString();
		m_Data->SetTexture(texName, fboTexture);
	}
}

void dsTextureCapture::Load() {
    Graph::Device * const dev = m_Data->GetGraphicsDevice();

	// This is the adjusted viewport size
	const Math::Vec2 dim = m_Data->GetVirtualViewportDimensions();

    fboManager = dev->CreateRTManager(dim.GetX(), dim.GetY());

    fboTexture = dynamic_cast<Graph::Texture2D*>(
        dev->CreateTexture(Graph::TEXTURE_2D, dim.GetX(), dim.GetY(), 0, Graph::FORMAT_RGBA_8B, true)
        );
    fboManager->Attach(0, fboTexture);
}

void dsTextureCapture::Update(DS::StageProxy * proxy, double start, double end, double time) {
	std::list<Math::TimelineItem<DS::StageProxy*>> items;
	proxy->GetItems(time, &items);

    ListFor(Math::TimelineItem<DS::StageProxy*>, items, i) {
        i->GetObject()->Update(i->GetObject(), i->GetStart(), i->GetEnd(), time);
    }
}

void dsTextureCapture::RenderFBO(DS::StageProxy * proxy, double start, double end, double time) {
    Graph::Device * const dev = m_Data->GetGraphicsDevice();

    std::list<Math::TimelineItem<DS::StageProxy*>> items;
	proxy->GetItems(time, &items);
    
    ListFor(Math::TimelineItem<DS::StageProxy*>, items, i) {
        i->GetObject()->RenderFBO(i->GetObject(),i->GetStart(), i->GetEnd(), time);
    }

    fboManager->Enable();
    {
        dev->Clear();
        dev->Viewport(0, 0, fboTexture->GetWidth(), fboTexture->GetHeight());

        ListFor(Math::TimelineItem<DS::StageProxy*>, items, i) {
            i->GetObject()->Render(i->GetObject(), i->GetStart(), i->GetEnd(), time);
        }
    }
	
    fboManager->Disable();
}


void dsTextureCapture::Render(DS::StageProxy * proxy, double start, double end, double time) {
    
}
