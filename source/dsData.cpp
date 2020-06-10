
/**
* NCK-DEMO SCENE
* https://github.com/nczeroshift/nck-demo
*/

#include "dsData.h"
#include "nckUtils.h"

_DS_BEGIN

Data::Data(Core::Window * wnd, Graph::Device * gDev) {
    window = wnd;
    gDevice = gDev;
    shapeRenderer = NULL;
    widgetRenderer = NULL;

    Graph::Texture * fontTexture = this->LoadTexture(DS_FONT_TEXTURE);
    fontTexture->SetFilter(Graph::FILTER_MIPMAPPING, Graph::FILTER_NEAREST);
    fontTexture->SetFilter(Graph::FILTER_MAGNIFICATION, Graph::FILTER_LINEAR);
    fontTexture->SetFilter(Graph::FILTER_MINIFICATION, Graph::FILTER_LINEAR);

    Gui::FontMap * fontMap = this->LoadFontMap(DS_FONT_MAP);
    fontMap->SetPositionAccuracy(Gui::FONT_POSITION_ACCURACY_INTEGER);

    m_Textures.insert(std::pair<std::string, Graph::Texture*>(DS_FONT_TEXTURE, fontTexture));
    m_FontMap.insert(std::pair<std::string, Gui::FontMap*>(DS_FONT_MAP, fontMap));

    shapeRenderer = new Gui::ShapeRenderer(gDev);
    widgetRenderer = new Gui::BlenderWidgetRenderer(gDevice, shapeRenderer, fontMap, fontTexture);
    lastTimelineChange = 0;

	accessMutex = Core::CreateMutex();

	m_VirtualDisplayWidth = VIRTUAL_DISPLAY_WIDTH;
	m_VirtualDisplayHeight = VIRTUAL_DISPLAY_HEIGHT;
}

Data::~Data() {
    MapFor(std::string, Compound*, m_Compounds, i) {
        delete i->second;
    }
    m_Compounds.clear();
    
    MapFor(std::string, Graph::Texture*, m_Textures, i) {
        delete i->second;
    }
    m_Textures.clear();

    MapFor(std::string, Graph::Program*, m_Programs, i) {
        delete i->second;
    }
    m_Programs.clear();

    MapFor(std::string, Gui::FontMap*, m_FontMap, i) {
        delete i->second;
    }
    m_FontMap.clear();

    SafeDelete(widgetRenderer);
    SafeDelete(shapeRenderer);
	SafeDelete(accessMutex);
}

void Data::LockAccess() {
	accessMutex->Lock();
}

void Data::UnlockAccess() {
	accessMutex->Unlock();
}

void Data::GetShaderErrors(std::list<std::string>  * errors) {
	if (errors)
		*errors = shadersErrors;
}

ReloadResult Data::ReloadTextures() {
    int total = 0, success = 0;
    std::list<std::string> errors;
    total = gDevice->ReloadPrograms(&success, &errors);
    return ReloadResult(total, success, 0);
}

ReloadResult Data::ReloadCompounds() {
    int total = 0, success = 0;
    std::list<std::string> errors;
    MapFor(std::string, Compound * , m_Compounds, i) {
        int flag;
        
        try {
            flag = i->second->Reload();
            if (flag == 1)
                success++;
        }
        catch (const Core::Exception & e) {
            errors.push_back(e.ToString());
        }
        total++;
    }
    return ReloadResult(total, success, errors.size());
}

ReloadResult Data::ReloadShaders() {
    int total = 0, success = 0;
	shadersErrors.clear();
    total = gDevice->ReloadPrograms(&success, &shadersErrors);
    return ReloadResult(total, success, shadersErrors.size());
}

Graph::Device * Data::GetGraphicsDevice() {
    return gDevice;
}

Gui::ShapeRenderer * Data::GetShapeRenderer() {
    return shapeRenderer;
}

Gui::WidgetRenderer * Data::GetWidgetRenderer() {
    return widgetRenderer;
}

Compound * Data::LoadCompound(const std::string & filename) {
    std::map<std::string, Compound*>::iterator i = m_Compounds.find(filename);
    if (i != m_Compounds.end())
        return i->second;
    Compound * ret = new Compound(gDevice);
    ret->Load(filename);
    m_Compounds.insert(std::pair<std::string, Compound*>(filename,ret));
    return ret;
}

Compound * Data::LoadCompound(const std::string & filename, Scene::Processor * processor) {
    std::map<std::string, Compound*>::iterator i = m_Compounds.find(filename);
    if (i != m_Compounds.end())
        return i->second;
    Compound * ret = new Compound(gDevice);
    ret->Load(filename,processor);
    m_Compounds.insert(std::pair<std::string, Compound*>(filename, ret));
    return ret;
}

Graph::Texture * Data::LoadTexture(const std::string & filename) {
    std::map<std::string, Graph::Texture*>::iterator i = m_Textures.find(filename);
    if (i != m_Textures.end())
        return i->second;
    Graph::Texture * ret = gDevice->LoadTexture(filename);
    m_Textures.insert(std::pair<std::string, Graph::Texture*>(filename, ret));
    return ret;
}

Gui::FontMap * Data::LoadFontMap(const std::string & filename) {
    std::map<std::string, Gui::FontMap*>::iterator i = m_FontMap.find(filename);
    if (i != m_FontMap.end())
        return i->second;
    Gui::FontMap * ret = new Gui::FontMap(gDevice);
    ret->Load(filename);
    m_FontMap.insert(std::pair<std::string, Gui::FontMap*>(filename, ret));
    return ret;
}

Graph::Program * Data::LoadProgram(const std::string & filename) {
    std::map<std::string, Graph::Program*>::iterator i = m_Programs.find(filename);
    if (i != m_Programs.end())
        return i->second;
    Graph::Program * ret = gDevice->LoadProgram(filename);
    m_Programs.insert(std::pair<std::string, Graph::Program*>(filename, ret));
    return ret;
}

Graph::Program * Data::LoadProgramFromText(const std::string & name, const std::string & source) {
	std::map<std::string, Graph::Program*>::iterator i = m_Programs.find(name);
	if (i != m_Programs.end())
		return i->second;
	Graph::Program * ret = gDevice->LoadProgramFromText(source);
	m_Programs.insert(std::pair<std::string, Graph::Program*>(name, ret));
	return ret;
}

Graph::Texture * Data::GetTexture(const std::string & filename) {
    if (m_Textures.find(filename) != m_Textures.end())
        return m_Textures.find(filename)->second;
    return NULL;
}

Compound * Data::GetCompound(const std::string & filename) {
    if (m_Compounds.find(filename) != m_Compounds.end())
        return m_Compounds.find(filename)->second;
    return NULL;
}

Gui::FontMap * Data::GetFontMap(const std::string & filename) {
    if (m_FontMap.find(filename) != m_FontMap.end())
        return m_FontMap.find(filename)->second;
    return NULL;
}

Graph::Program * Data::GetProgram(const std::string & filename) {
    if (m_Programs.find(filename) != m_Programs.end())
        return m_Programs.find(filename)->second;
    return NULL;
}

void Data::SetTexture(const std::string & filename, Graph::Texture * texture) {
	if (m_Textures.find(filename) == m_Textures.end()) {
		m_Textures.insert(std::pair<std::string, Graph::Texture*>(filename, texture));
	}
}

float Data::GetAspect() {
    return window->GetWidth() / (float)window->GetHeight();
}

float Data::GetHeight() {
    return window->GetHeight();
}

float Data::GetWidth() {
    return window->GetWidth();
}

void Data::GetProgramKeys(std::vector<std::string> * names) {
    names->reserve(m_Programs.size());
    MapFor(std::string, Graph::Program *, m_Programs, i) {
        names->push_back(i->first);
    }
}

void Data::GetTextureKeys(std::vector<std::string> * names) {
    names->reserve(m_Programs.size());
    MapFor(std::string, Graph::Texture *, m_Textures, i) {
        names->push_back(i->first);
    }
}

void Data::GetCompoundsKeys(std::vector<std::string> * names) {
    names->reserve(m_Programs.size());
    MapFor(std::string, DS::Compound *, m_Compounds, i) {
        names->push_back(i->first);
    }
}

int Data::GetVirtualDisplayWidth() { 
	return m_VirtualDisplayWidth; 
}

int Data::GetVirtualDisplayHeight() { 
	return m_VirtualDisplayHeight; 
}

void Data::SetupVirtualDisplay(int width, int height) {
	m_VirtualDisplayWidth = width; 
	m_VirtualDisplayHeight = height; 
}

void Data::SetVirtualViewport() {
	float aspect = GetVirtualDisplayWidth() / (float)GetVirtualDisplayHeight();
	float tmpWidth, tmpHeight, tmpX, tmpY;
	if (aspect > 1) {
		tmpWidth = window->GetWidth();
		tmpHeight = tmpWidth / aspect;
		tmpX = 0;
		tmpY = (window->GetHeight() - tmpHeight) / 2.0;
	}
	else {
		tmpHeight = window->GetHeight();
		tmpWidth = tmpHeight * aspect;
		tmpX = (window->GetWidth() - tmpWidth) / 2.0;
		tmpY = 0;
	}

	gDevice->Viewport(tmpX, tmpY, tmpWidth, tmpHeight);
}

Math::Vec2 Data::GetVirtualViewportDimensions() {
	float aspect = GetVirtualDisplayWidth() / (float)GetVirtualDisplayHeight();
	float tmpWidth, tmpHeight, tmpX, tmpY;
	if (aspect > 1) {
		tmpWidth = window->GetWidth();
		tmpHeight = tmpWidth / aspect;
	}
	else {
		tmpHeight = window->GetHeight();
		tmpWidth = tmpHeight * aspect;
	}
	return Math::Vec2((int)tmpWidth, (int)tmpHeight);
}

_DS_END
