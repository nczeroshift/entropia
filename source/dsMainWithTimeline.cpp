
/**
* NCK-DEMO 
* https://github.com/nczeroshift/nck-demo
*/

#include "dsMainWithTimeline.h"

#include "scenes/dsText2D.h"
#include "scenes/dsPlane2D.h"
#include "scenes/dsModel3D.h"
#include "scenes/dsTextureCapture.h"

static DS::Stage * instanceWithId(const std::string & type, DS::Data * data) {
	if (type == "text")
		return new dsText2D(data);
	else if (type == "plane")
		return new dsPlane2D(data);
	else if (type == "model")
		return new dsModel3D(data);
	else if (type == "capture")
		return new dsTextureCapture(data);
	return NULL;
}

_DS_BEGIN

dsMainWithTimeline::dsMainWithTimeline(Core::Window * window) : dsMain(window) {

}

dsMainWithTimeline::~dsMainWithTimeline() {

}

void dsMainWithTimeline::LoadTimeline() 
{    
	const std::string fileName = "script://timeline.json";

	// Check if file was changed
	const int64_t lastModified = Core::GetFileLastModified(fileName);
	if (m_LastTimelineDate == lastModified)
		return;
	
	JSONValue * obj = ReadFileAsJSON(fileName);

	mainTimeline.Clear();
	
	const JSONObject & root = obj->AsObject();
	
	if (root.find("scenes") != root.end()) 
	{
		const JSONArray & entries = root.find("scenes")->second->AsArray();

		for (int i = 0; i < entries.size(); i++) 
		{
			renderLoading(100 * i / (float)entries.size());
			Core::Thread::Wait(100);

			const JSONObject & entry = entries[i]->AsObject();
			std::string name = entry.find("name")->second->AsString();
			std::string type = entry.find("type")->second->AsString();

			DS::Stage * stage = NULL;
			if (m_InstancedStages.find(name) != m_InstancedStages.end())
				stage = m_InstancedStages.find(name)->second;
			else {
				stage = instanceWithId(type, data);
				if (stage == NULL)
					THROW_EXCEPTION("Scene \""+type+"\" type not found");
				stage->Load();
				m_InstancedStages.insert(std::pair<std::string, DS::Stage*>(name, stage));
			}

			stage->Parse(entry);
		}
	}

	if (root.find("timeline") != root.end()) {
		const JSONArray & entries = root.find("timeline")->second->AsArray();
		for (int i = 0; i < entries.size(); i++) {
			const JSONObject & entry = entries[i]->AsObject();
			mainTimeline.Insert(DS::StageProxy::Parse(entry, m_InstancedStages));
		}
	}

	mainTimeline.Build();

	m_LastTimelineDate = lastModified;

	delete obj;
}

_DS_END
