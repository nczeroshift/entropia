
/**
* NCK-DEMO SCENE
* https://github.com/nczeroshift/nck-demo
*/

#include "dsStage.h"

_DS_BEGIN

StageValue::StageValue() {
	m_Value = 0;
	m_BezierValue = NULL;
}

StageValue::StageValue(float value) {
	m_Value = value;
	m_BezierValue = NULL;
}

StageValue::~StageValue() {
	SafeDelete(m_BezierValue);
}

void StageValue::Parse(const JSONValue & value) {
	if (value.IsNumber()) {
		m_Value = value.AsNumber();
	}
	else if (value.IsObject()) {
		const JSONObject & obj = value.AsObject();
		std::string type = "bezier";
		
		if (obj.find("type") != obj.end())
			type = obj.find("type")->second->AsString();

		if (obj.find("nodes") != obj.end()) {
			const JSONArray & nodes = obj.find("nodes")->second->AsArray();
			if (type == "bezier") {
				m_BezierValue = new Scene::AnimationGraph();
				for (int i = 0; i < nodes.size(); i++){
					const JSONObject & obj = nodes[i]->AsObject();
					float l_t = obj.find("lt")->second->AsNumber();
					float l_v = obj.find("lv")->second->AsNumber();
					float c_t = obj.find("ct")->second->AsNumber();
					float c_v = obj.find("cv")->second->AsNumber();
					float n_t = obj.find("nt")->second->AsNumber();
					float n_v = obj.find("nv")->second->AsNumber();
					Scene::AnimationNode n = Scene::AnimationNode(l_t, l_v, c_t, c_v, n_t, n_v);
					m_BezierValue->AddNode(n);
				}
			}
			else if (type == "linear") {
				m_BezierValue = new Scene::AnimationGraph();
				float last_t = 0;
				float last_v = 0;
				for (int i = 0; i < nodes.size(); i+=2) {
					float t = nodes[i]->AsNumber();
					float v = nodes[i+1]->AsNumber();
					Scene::AnimationNode n = Scene::AnimationNode(last_t, last_v, t, v, t, v);
					last_t = t;
					last_v = v;
					m_BezierValue->AddNode(n);
				}	
			}
		}
	}
}

float StageValue::GetValue(float time) {
	if(m_BezierValue != NULL)
		return m_BezierValue->GetValue(time);
	return m_Value;
}


void StageProxy::AddStage(const Math::TimelineItem<DS::StageProxy*> stage) {
	m_Stages.Insert(stage);
}

int StageProxy::GetItems(double time, std::list<Math::TimelineItem<DS::StageProxy*>> * items) {
	m_Stages.Get(time, items);
	items->sort(DS::StageProxy::compareTL);
	return items->size();
}

//void Stage::AddStage(double start, double end, Stage * stage, int layer) {
//    m_Stages.Insert(Math::TimelineItem<DS::Stage*>(start, end, stage, layer));
//}

void StageProxy::BuildTimeline() {
    m_Stages.Build();
}

bool StageProxy::compareTL(const Math::TimelineItem<DS::StageProxy*> & a, const Math::TimelineItem<DS::StageProxy*> & b) {
	return a.GetObject()->GetOrder() < b.GetObject()->GetOrder();
}

StageProxy::StageProxy(Data * data) : Stage(data) {
    m_Offset = 0;
    m_Stage = NULL;
    m_RepeatDuration = 0;
}

StageProxy::StageProxy(Stage * stage, double offset) : Stage(stage!=NULL?stage->GetData():NULL) {
    m_Stage = stage;
    m_Offset = offset;
    m_RepeatDuration = 0;
}

StageProxy::~StageProxy() {

}

void StageProxy::Load() {

}

void StageProxy::Render(StageProxy * proxy, double start, double end, double time) {
    double t = time;

    if (m_RepeatDuration != 0)
        t -= m_RepeatDuration * floor(((t - start) / m_RepeatDuration));

    if (m_Stage)
        m_Stage->Render(proxy, start, end, t + m_Offset);
}

void StageProxy::RenderFBO(StageProxy * proxy, double start, double end, double time) {
    double t = time;

    if (m_RepeatDuration != 0)
        t -= m_RepeatDuration * floor(((t - start) / m_RepeatDuration));

    if (m_Stage)
        m_Stage->RenderFBO(proxy, start , end, t + m_Offset);
}

void StageProxy::SetOffset(double offset) {
    m_Offset = offset;
}

void StageProxy::SetStage(Stage * stage) {
    m_Stage = stage;
}

void StageProxy::Update(StageProxy * proxy, double start, double end, double time) {
    double t = time;

    if (m_RepeatDuration != 0)
        t -= m_RepeatDuration * floor(((t - start) / m_RepeatDuration));

    if (m_Stage)
        m_Stage->Update(proxy,start, end, t + m_Offset);
}

void StageProxy::Parse(const JSONObject & node) {
	if (node.find("offset") != node.end()){
		double off = node.find("offset")->second->AsNumber();
		m_Offset = off;
	}

	if (node.find("repeat") != node.end()) {
		double rep = (double)(node.find("repeat")->second->AsNumber() * 1e6);
		m_RepeatDuration = rep;
	}
}

Math::TimelineItem<DS::StageProxy*> StageProxy::Parse(const JSONObject & entry, const std::map<std::string, DS::Stage*> & stages) 
{	
	if (entry.find("name") == entry.end() ||
		entry.find("start") == entry.end() ||
		entry.find("end") == entry.end() ||
		entry.find("track") == entry.end())
		THROW_EXCEPTION("StageProxy \"name\",\"start\",\"end\" and \"track\" parameters are required");

	std::string name = entry.find("name")->second->AsString();

	DS::Stage * stage = NULL;
	if (stages.find(name) == stages.end())
		THROW_EXCEPTION("StageProxy stage with name \"" + name + "\" not found." );

	stage = stages.find(name)->second;

	DS::StageProxy * proxy = new DS::StageProxy(stage);
	
	proxy->Parse(entry);
	
	double start = entry.find("start")->second->AsNumber();
	double end = entry.find("end")->second->AsNumber();
	int track = entry.find("track")->second->AsNumber();

	if (entry.find("children") != entry.end()) {
		const JSONArray & childrenTL = entry.find("children")->second->AsArray();
		for (int i = 0; i < childrenTL.size(); i++) {
			const JSONObject & ee = childrenTL[i]->AsObject();
			proxy->AddStage(StageProxy::Parse(ee, stages));
		}
	}

	proxy->BuildTimeline();

	return Math::TimelineItem<DS::StageProxy*>(start, end, proxy, track);
}

_DS_END