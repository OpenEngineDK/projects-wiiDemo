// main
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

// OpenEngine Core stuff
#include <Logging/Logger.h>
#include <Logging/StreamLogger.h>
#include <Core/Engine.h>

// Extensions
#include <Display/WiiEnvironment.h>
#include <Display/WiiCanvas.h>
#include <Display/ViewingVolume.h>
#include <Scene/SceneNode.h>
#include <Renderers/WiiRenderer.h>
#include <Renderers/WiiRenderingView.h>

#include <stdio.h>
#include <stdlib.h>

using namespace OpenEngine::Logging;
using namespace OpenEngine::Core;
using namespace OpenEngine::Utils;
using namespace OpenEngine::Display;
using namespace OpenEngine::Renderers;
using namespace OpenEngine::Scene;

int main(int argc, char** argv) {
    // Setup logging facilities.
    Logger::AddLogger(new StreamLogger(&std::cout));

    // Print usage info.
    logger.info << "========= Running OpenEngine Nintendo Wii Demo Project =========" << logger.end;

    IEngine* engine       = new Engine();
    IEnvironment* env     = new WiiEnvironment();
    IFrame& frame         = env->CreateFrame();
    IRenderCanvas* canvas = new WiiCanvas();
    IRenderer* renderer   = new WiiRenderer();
    IRenderingView* rv    = new WiiRenderingView();
    IViewingVolume* vv    = new ViewingVolume();
    ISceneNode* scene     = new SceneNode();

    canvas->SetScene(scene);
    canvas->SetViewingVolume(vv);

    // renderer->ProcessEvent().Attach(*rv);
    //canvas->SetRenderer(renderer);
    frame.SetCanvas(canvas);

    // Bind to the engine for processing time
    engine->InitializeEvent().Attach(*env);
    engine->ProcessEvent().Attach(*env);
    engine->DeinitializeEvent().Attach(*env);
    
    engine->Start();

    delete engine;
    delete env;

    // Return when the engine stops.
    return EXIT_SUCCESS;
}


