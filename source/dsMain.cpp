
/**
* NCK-DEMO SCENE
* https://github.com/nczeroshift/nck-demo
*/

#include "dsMain.h"
#include "nckUtils.h"

_DS_BEGIN

dsMain::dsMain(Core::Window * window) : Threadable() {
    wnd = window;
    dev = NULL;
	data = NULL;
    fontMap = NULL;
    fontTexture = NULL;
    fontTimeTexture = NULL;
    fontTimeMap = NULL;
    mutePlayback = false;
    player = NULL;
    lastTime = 0;
	m_MaxDuration = 10 * 60;
    showUI = false;
    sDensity = Core::Window::GetDisplayDensity();
    hScale = wnd->GetHeight() / (float)REFERENCE_HEIGHT;
    timer = Core::CreateChronometer();
    frTime = Core::CreateChronometer();
    shpRender = NULL;
    server = NULL;
	m_VirtualWidth = VIRTUAL_DISPLAY_WIDTH;
	m_VirtualHeight = VIRTUAL_DISPLAY_HEIGHT;

	m_AudioEnable = false;
	m_AudioSampleRate = AUDIO_SAMPLERATE;
	m_AudioFTTSize = AUDIO_FFT;

    for (int i = 0; i < 3; i++) {
        reloadShaderFlag[i] = 0;
        reloadTextureCounter[i] = 0;
        reloadCompoundCounter[i] = 0;
        reloadShaderFlag[i] = 0;
    }

    reloadResources = false;
    reloadTimeline = false;
}

dsMain::~dsMain() {
    SafeDelete(timer);
    SafeDelete(frTime);
    std::list<Subtitle*> subs;
    subtitlesTimeline.GetAll(&subs);
    ListFor(Subtitle*, subs, i) {
        delete (*i);
    }
}

void dsMain::AddSub(float start, float duration, const std::string & text) {
    Subtitle * s = subtitlesTimeline.Insert(Math::TimelineItem<Subtitle*>(start, start+duration, new Subtitle(dev, fontTexture, fontMap)));
    s->Set(text, hScale * REFERENCE_SUBTITLE_SIZE);
}
    
void dsMain::Run() {
    try {
        dev = Graph::CreateDevice(wnd, Graph::DEVICE_AUTO, wnd->GetWidth(), wnd->GetHeight());
        dev->ClearColor(1.0, 1.0, 1.0, 1.0);
        dev->ClearFlags(Graph::BUFFER_COLOR_BIT | Graph::BUFFER_DEPTH_BIT);

        dev->Enable(Graph::STATE_ZBUFFER_WRITE);

		// Loading basic resources
        RenderLoading(wnd, dev); 

        data = new Data(wnd, dev);
		data->SetupVirtualDisplay(m_VirtualWidth, m_VirtualHeight);

        shpRender = data->GetShapeRenderer();

        fontTexture = data->LoadTexture(DS_FONT_TEXTURE);
		fontMap = data->LoadFontMap(DS_FONT_MAP);
	
        iconTexture = data->LoadTexture("texture://ui/iconTexture.png");
        iconShader = data->LoadTexture("texture://ui/iconShader.png");
        iconCompound = data->LoadTexture("texture://ui/iconCompound.png");

        loading = new dsLoading(data);
        loading->Load();

        // Load demo music.
        renderLoading(0);
		LoadAudio();

		// Load hardcoded timeline.
		LoadTimeline();

        renderLoading(100);
		Core::Thread::Wait(100);

        server = new Server(data);
        server->Start();
    }
    catch (const Core::Exception & ex) {
        ex.PrintStackTrace();
        ReleaseStuff();
        Teardown();
        return;
    }
               
    if(player)
        player->Play();

    timer->Start();
    frTime->Start();

    dev->ClearColor(0.3, 0.3, 0.3, 1.0);
        
	ReloadResult rCompounds, rShaders, rTextures;

    while (!IsTearingDown())
    {
        /*if (server) {
            if (!server->FreeRun()) {
                Core::Thread::Wait(32);
            }
        }*/

        dev->ClearColor(0, 0, 0, 1);
        dev->Clear();       

		// Reload from server request.
		if (server != NULL) {
			ServerOperation * op = NULL;
			if (server->FetchOperation(SERVER_OP_RELOAD_SHADER, &op)){
				rShaders = data->ReloadShaders();

				if (rShaders.GetFailure()) {
					std::list<std::string> shadersErrors;
					data->GetShaderErrors(&shadersErrors);
					std::string shdErrorString = "";

					ListFor(std::string, shadersErrors, j) {
						if (shdErrorString.empty())
							shdErrorString += (*j);
						else
							shdErrorString += "\n" + (*j);
					}

					op->SetError(shdErrorString);
				}

				op->Finish();
			}
		}

		// Reload from keyboard action.
		if (reloadResources)
		{
			rShaders = data->ReloadShaders();
			rCompounds = data->ReloadCompounds();
			rTextures = data->ReloadTextures();

			reloadResources = false;
			
			std::list<std::string> shadersErrors;
			data->GetShaderErrors(&shadersErrors);
			std::string shdErrorString = "";
			ListFor(std::string, shadersErrors, j) {
				if (shdErrorString.empty())
					shdErrorString += (*j);
				else
					shdErrorString += "\n" + (*j);
			}

			Core::DebugLog(shdErrorString);
		}

		// Reload timeline from keyboard action
		if (reloadTimeline) {
			try{
				LoadTimeline();
			}
			catch (Core::Exception & e) {
				e.PrintStackTrace();
			}
			reloadTimeline = false;
		}

        std::list<Math::TimelineItem<DS::StageProxy*>> items;
          
        double time = GetTime() / 1e6;
            
        if (time > this->GetMaxDuration())
            break;

		Graph::Program::SetGlobalTime(time);

        mainTimeline.Get(time, &items);
		items.sort(DS::StageProxy::compareTL);

        ListFor(Math::TimelineItem<DS::StageProxy*>, items, i) {
            i->GetObject()->Update(i->GetObject(), i->GetStart(), i->GetEnd(), time);
        }

		data->SetVirtualViewport();

        ListFor(Math::TimelineItem<DS::StageProxy*>, items, i) {
            i->GetObject()->RenderFBO(i->GetObject(), i->GetStart(), i->GetEnd(), time);
        }

        dev->Viewport(0, 0, wnd->GetWidth(), wnd->GetHeight());

        ListFor(Math::TimelineItem<DS::StageProxy*>, items, i) {
            i->GetObject()->Render(i->GetObject(), i->GetStart(), i->GetEnd(), time);
        }
        
        if (server)
            server->FetchImage(dev, wnd->GetWidth(), wnd->GetHeight());

        if (showUI) {
			const double fps = UpdateFPS();
			RenderUI(rCompounds, rShaders, rTextures, fps);
        }

        dev->PresentAll();
    }
               
    if(player)
        player->Stop();

    ReleaseStuff();
}

void dsMain::RenderUI(const ReloadResult & rCompounds,const ReloadResult & rShaders,const ReloadResult & rTextures, float fps) {
	dev->Viewport(0, 0, wnd->GetWidth(), wnd->GetHeight());
	dev->Enable(Graph::STATE_BLEND);
	dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

	dev->Disable(Graph::STATE_DEPTH_TEST);
	dev->MatrixMode(Graph::MATRIX_PROJECTION);
	dev->Identity();
	dev->Ortho2D(wnd->GetWidth(), wnd->GetHeight());

	dev->MatrixMode(Graph::MATRIX_VIEW);
	dev->Identity();

	dev->MatrixMode(Graph::MATRIX_MODEL);
	dev->Identity();

	dev->Enable(Graph::STATE_BLEND);
	dev->Disable(Graph::STATE_DEPTH_TEST);
	dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

	DrawFrameRate(10, 20, fps);
	DrawText(10, 45, 24, false, "space - play/pause");
	DrawText(10, 65, 24, false, "z - reload timeline");
	DrawText(10, 85, 24, false, "x - reload resources");
	DrawText(10, 105, 24, false, "c - hide/show UI");
	DrawText(10, 125, 24, false, "esc - exit");

	DrawFPS(10, 140, 200, 100);

	if (!mutePlayback) {
		DrawFFT(10, 260, 200, 100);
	}

	DrawSeekbar(wnd->GetHeight() - 20, 20);
	DrawTimeText(wnd->GetHeight() - 45);

	Gui::ShapeRenderer * shp = data->GetShapeRenderer();

	if (rCompounds.GetTotal() > 0) {
		iconCompound->Enable();
		shp->Square(wnd->GetWidth() - 140, 10, 24, 24, Math::Color4ub());
		iconCompound->Disable();

		DrawText(wnd->GetWidth() - 100, 20, 24, false, Math::IntToString(rCompounds.GetTotal()));
		DrawText(wnd->GetWidth() - 65, 20, 24, false, Math::IntToString(rCompounds.GetSuccess()), Math::Color4ub(0, 255, 0));
		DrawText(wnd->GetWidth() - 30, 20, 24, false, Math::IntToString(rCompounds.GetFailure()), Math::Color4ub(255, 0, 0));
	}

	if (rShaders.GetTotal() > 0) {
		iconShader->Enable();
		shp->Square(wnd->GetWidth() - 140, 38, 24, 24, Math::Color4ub());
		iconShader->Disable();

		DrawText(wnd->GetWidth() - 100, 50, 24, false, Math::IntToString(rShaders.GetTotal()));
		DrawText(wnd->GetWidth() - 65, 50, 24, false, Math::IntToString(rShaders.GetSuccess()), Math::Color4ub(0, 255, 0));
		DrawText(wnd->GetWidth() - 30, 50, 24, false, Math::IntToString(rShaders.GetFailure()), Math::Color4ub(255, 0, 0));
	}

	if (rTextures.GetTotal() > 0) {
		iconTexture->Enable();
		shp->Square(wnd->GetWidth() - 140, 68, 24, 24, Math::Color4ub());
		iconTexture->Disable();

		DrawText(wnd->GetWidth() - 100, 80, 24, false, Math::IntToString(rTextures.GetTotal()));
		DrawText(wnd->GetWidth() - 65, 80, 24, false, Math::IntToString(rTextures.GetSuccess()), Math::Color4ub(0, 255, 0));
		DrawText(wnd->GetWidth() - 30, 80, 24, false, Math::IntToString(rTextures.GetFailure()), Math::Color4ub(255, 0, 0));
	}
}

double dsMain::UpdateFPS() {
    const double ttime = frTime->GetElapsedTime();
    double fps = 1.00f / (ttime / 1.0e6);
    if (fps > 200)
        fps = 200;
    fpsQueue.push_front(fps);
    if (fpsQueue.size() > 50)
        fpsQueue.pop_back();
    double meanFps = 0.0;
    ListFor(float, fpsQueue, i)
        meanFps += (*i);
    meanFps /= fpsQueue.size();
    frTime->Start(); // restart
    return meanFps;
}
    
void dsMain::ReleaseStuff() {
    SafeDelete(data);
    SafeDelete(player);
    SafeDelete(dev);
}

int64_t dsMain::GetTime() {
    if (mutePlayback) {
        return lastTime + timer->GetElapsedTime();
    }
    return player->GetTime();
}

void dsMain::DrawText(float x, float y, float size, bool bold, const std::string & str, const Math::Color4ub & col) {
    fontTexture->Enable();
    dev->Color(0, 0, 0, col.GetA()*0.8);
    fontMap->Draw(x+1, y+1, size, str);

    dev->Color(col.GetR(), col.GetG(), col.GetB(), col.GetA());
    fontMap->Draw(x, y, size, str);
    fontTexture->Disable();
}
    
void dsMain::DrawTimeText(float y) {
    dev->Color(255, 255, 255, 255);
    DrawText(10, y, 50,true, Math::TimeToString(GetTime()));
    dev->Color(255, 0, 0, 255);
    DrawText(210, y + 7, 24,false, Math::TimeToString(GetMaxDuration()* 1e6));
}

void dsMain::DrawFrameRate(float x, float y, float fps) {
    DrawText(x, y, 32, true, "FPS: "+Math::FloatToString(fps,3));
}

void dsMain::DrawSeekbar(float y, float height) {
    int64_t duration = GetMaxDuration() * 1e6;
    int64_t audioTime = GetTime();
    float margin = 8;
    shpRender->Square(0, y, wnd->GetWidth(), height, Math::Color4ub(100, 100, 100,150));
    shpRender->Square(margin / 2, wnd->GetHeight() - (height-margin / 2), (wnd->GetWidth() - margin) * (audioTime / (float)duration), height-margin, Math::Color4ub(255, 255, 255,200));
}

void dsMain::DrawFFT(float x, float y, float width, float height) {
    float groupWidth = 2;
    const float diffDB = player->GetFFT()->GetMax_dB() - player->GetFFT()->GetMin_dB();
    const float maxDB = player->GetFFT()->GetMax_dB();

    float freq_start = 40;
    float freq_max = 4000;
    float freq_step = 40;

    groupWidth =floor(width/((freq_max - freq_start) / freq_step));

    shpRender->Square(x, y, width, height, Math::Color4ub(100, 100, 100,155));
        
    int steps = diffDB / 10;
    for (int i = 0; i <= steps; i++)
    {
        float yP = y + i * (height / steps);
        shpRender->Square(x, yP, width, 1, Math::Color4ub(0, 0, 0, 50));
        DrawText(x + width + 10, yP, 16, false, Math::FloatToString(maxDB - (i * diffDB / steps), 1) + "dB");
    }

    for (int i = 0, f = freq_start; f <= freq_max; f += freq_step, i++)
    {
        double dB = player->GetdB(f);
        dB = (dB / diffDB) * height;
        shpRender->Square(x+groupWidth * i, y - dB + height, groupWidth, dB, Math::Color4ub(255, 255, 255,200));
    }

    shpRender->Square(x, y, 1, height, Math::Color4ub(0, 0, 0, 50));
    shpRender->Square(x + width, y, 1, height, Math::Color4ub(0, 0, 0, 50));
}

void dsMain::DrawFPS(float x, float y, float width, float height) {
    const float fpsLimits[] = {30,60,120,180,240};

    shpRender->Square(x, y, width, height, Math::Color4ub(100, 100, 100, 155));
   
    float maxFPS = 10.0;
    ListFor(float, fpsQueue, i) {
        if (*i > maxFPS)
            maxFPS = *i;
    }

    float bestFPS = maxFPS;
    for (int i = 4; i >= 0; i--) {
        if (fpsLimits[i] - maxFPS > 10) {
            bestFPS = fpsLimits[i];
        }
    }
        
    int steps = 4;
    for (int i = 0; i <= steps; i++)
    {
        float yP = y + i * (height / steps);
        shpRender->Square(x, yP, width, 1, Math::Color4ub(0, 0, 0, 50));
        DrawText(x + width + 10, yP, 16, false, Math::FloatToString(bestFPS - i*(bestFPS / steps), 1) + "fps");
    }

    dev->Color(255, 255, 255, 200);
    dev->Begin(Graph::PRIMITIVE_LINES);
    float rx = x + width;
    Math::Vec2 lastPos;
    ListFor(float, fpsQueue, i) {
        float ry = y+height  - height * *i / bestFPS;

        if (lastPos.GetX() > 0) {
            dev->Vertex(rx, ry);
            dev->Vertex(lastPos.GetX(), lastPos.GetY());
        }
        lastPos = Math::Vec2(rx, ry);
        rx -= (width / (fpsQueue.size() -1));
    }
    dev->End();
     
    shpRender->Square(x, y, 1, height, Math::Color4ub(0, 0, 0, 50));
    shpRender->Square(x + width, y, 1, height, Math::Color4ub(0, 0, 0, 50));
}

void dsMain::PlayPauseToggle() {
    if (mutePlayback) {
		int64_t tmp = lastTime + timer->GetElapsedTime();
        if (timer->IsRunning()) {
            timer->Stop();
			timer->Clear();
            lastTime = tmp;
        }
        else
            timer->Start();
    }
    else {
        if (player->isPlaying())
            player->Pause();
        else
            player->Play();
    }
}

void dsMain::SeekTo(int64_t pos) {
    if (mutePlayback) {
        timer->Clear();
        timer->Start();
        lastTime = pos;
    }
    else {
        player->Seek(pos);
    }

    //Core::DebugLog(Math::IntToString(pos) + "\n");
}

void dsMain::RequestReloadResources() {
    reloadResources = true;
}

void dsMain::RequestReloadTimeline() {
	reloadTimeline = true;
}

void dsMain::ToggleUI() {
    showUI = !showUI;
}

void dsMain::UpdateWndInput() {
    static bool wasLeftClickDown = false;
    static bool wasPausePressed = false;
    static bool wasZPressed = false;
    static bool wasXPressed = false;
    static bool wasCPressed = false;

    if (wnd->GetMouseButtonStatus(Core::BUTTON_MOUSE_LEFT) == Core::BUTTON_STATUS_DOWN) {
        if(!wasLeftClickDown && showUI){
            Core::Point p = wnd->GetMousePosition();
            float off = (p.GetX() - 4) / (float)(wnd->GetWidth() - 8);
            if (p.GetY() > wnd->GetHeight() - 40) {
                SeekTo(off * (GetMaxDuration() * 1e6));
            }
        }
        wasLeftClickDown = true;
    }
    else
        wasLeftClickDown = false;

    if (wnd->GetKeyStatus(Core::KeyboardButton::BUTTON_SPACE) == Core::ButtonStatus::BUTTON_STATUS_DOWN) {
        if (!wasPausePressed) PlayPauseToggle();
        wasPausePressed = true;
    }
    else
        wasPausePressed = false;

    if (wnd->GetKeyStatus(Core::KeyboardButton::BUTTON_Z) == Core::ButtonStatus::BUTTON_STATUS_DOWN) {
        if(!wasZPressed) RequestReloadTimeline();
        wasZPressed = true;
    }
    else
        wasZPressed = false;

    if (wnd->GetKeyStatus(Core::KeyboardButton::BUTTON_X) == Core::ButtonStatus::BUTTON_STATUS_DOWN) {
        if(!wasXPressed) RequestReloadResources();
        wasXPressed = true;
    }
    else
        wasXPressed = false;

    if (wnd->GetKeyStatus(Core::KeyboardButton::BUTTON_C) == Core::ButtonStatus::BUTTON_STATUS_DOWN) {
        if(!wasCPressed) ToggleUI();
        wasCPressed = true;
    }
    else
        wasCPressed = false;


    if (wnd->GetKeyStatus(Core::BUTTON_ESCAPE) != Core::BUTTON_STATUS_UP)
        Teardown();
}

void dsMain::renderLoading(float progress) {
    dev->ClearColor(0, 0, 0, 1);
    dev->Clear();
    dev->Viewport(0, 0, wnd->GetWidth(), wnd->GetHeight());
    loading->Render(NULL, 0, 10e6, progress / 100.0);
    dev->PresentAll();
}

void dsMain::LoadAudio() {
	if (m_AudioEnable) {
		player = new AudioPlayer(m_AudioSampleRate, m_AudioFTTSize, AUDIO_BUFFERS);
		player->Start();

		if(!m_AudioStreamFilename.empty())
			player->LoadBG(m_AudioStreamFilename);
		//m_MaxDuration = player->GetDuration() / 1e6;

		return;
	}

	mutePlayback = true;
}

void dsMain::SetupAudioSettings(int audioSampleRate, int audioFFTSize, const std::string & audioStream) {
	m_AudioEnable = true;
	m_AudioSampleRate = audioSampleRate;
	m_AudioFTTSize = audioFFTSize;
	m_AudioStreamFilename = audioSampleRate;
}

JSONValue * ReadFileAsJSON(const std::string & fileName) {
	Core::FileReader * reader = Core::FileReader::Open(fileName);
	if (reader == NULL)
		THROW_EXCEPTION("File \"" + fileName + "\" not found ");

	int64_t size = Core::FileReader::Size(fileName);
	char * str = new char[size + 1];
	memset(str, 0, size + 1);
	reader->Read(str, size);

	JSONValue * obj = JSON::Parse(str);

	SafeArrayDelete(str);

	SafeDelete(reader);

	if (obj == NULL)
		THROW_EXCEPTION("Invalid JSON in \"" + fileName + "\"");

	const JSONObject & root = obj->AsObject();

	return obj;
}


_DS_END
