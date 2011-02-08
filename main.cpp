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
#include <Display/PerspectiveViewingVolume.h>
#include <Display/Camera.h>
#include <Scene/SceneNode.h>
#include <Renderers/TextureLoader.h>
#include <Renderers/WiiRenderer.h>
#include <Renderers/WiiRenderingView.h>
#include <Renderers/WiiLightVisitor.h>
#include <Math/Math.h>
#include <Resources/DirectoryManager.h>
#include <Resources/ResourceManager.h>
#include <Resources/OBJResource.h>
#include <Resources/TGAResource.h>
#include <Resources/AssimpResource.h>
#include <Scene/PointLightNode.h>
#include <Utils/MeshCreator.h>
#include <Scene/MeshNode.h>

#include <Devices/WiiMoteManager.h>

#include <stdio.h>
#include <stdlib.h>
#include <fat.h>
#include <iostream>
#include <fstream>

#include "Rotator.h"

#include <network.h>
#include <debug.h>

#include <Animations/Animator.h>
#include <Scene/AnimationNode.h>
#include <Scene/SearchTool.h>


#include <Physics/BulletPhysics.h>
#include <Physics/IRigidBody.h>
#include <Geometry/AABBShape.h>

// #include <Logging/WiiLogger.h>

// extern "C" {
// #include "net_print.h"
// }

using namespace OpenEngine::Logging;
using namespace OpenEngine::Core;
using namespace OpenEngine::Utils;
using namespace OpenEngine::Display;
using namespace OpenEngine::Devices;
using namespace OpenEngine::Renderers;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Scene;
using namespace OpenEngine::Math;
using namespace OpenEngine::Utils;
using namespace OpenEngine::Animations;

using namespace OpenEngine::Physics;

AnimationNode* GetAnimationNode(ISceneNode* node) {
    SearchTool st;
    return st.DescendantAnimationNode(node);
}

int main(int argc, char** argv) {
    // if_config(NULL, NULL, NULL, true);
    // DEBUG_Init(100,5656);
    // net_print_init("coddwii.dyndns.org", 8001);

    // net_print_string(__FILE__, __LINE__, "HEST\n");
	if (!fatInitDefault()) {
		// printf("fatInitDefault failure: terminating\n");
	}
    ofstream* outfile = new ofstream("/test.txt");

    // Setup logging facilities.
    Logger::AddLogger(new StreamLogger(outfile));
    //Logger::AddLogger(new WiiLogger());

    // Print usage info.
    logger.info << "========= Running OpenEngine Nintendo Wii Demo Project =========" << logger.end;
    outfile->flush();


    string resources = "/";
    DirectoryManager::AppendPath(resources);
    // ResourceManager<IModelResource>::AddPlugin(new OBJPlugin()); 
    ResourceManager<IModelResource>::AddPlugin(new AssimpPlugin());
    ResourceManager<ITexture2D>::AddPlugin(new TGAPlugin());

    IEngine* engine       = new Engine();
    // _break();
    // IEnvironment* env     = new WiiEnvironment();
    // IFrame& frame         = env->CreateFrame();

    // IPhysicsEngine* phys = new BulletPhysics(AABBShape(Vector<3,float>(-100.0), Vector<3,float>(100.0)));
    // phys->SetGravity(Vector<3,float>(0.0, -9.82, 0.0));
    // engine->ProcessEvent().Attach(*phys);


    IFrame* frame         = new WiiFrame();
    IRenderCanvas* canvas = new WiiCanvas();
    IRenderer* r   = new WiiRenderer();
    IRenderingView* rv    = new WiiRenderingView();
    Camera* cam = new Camera(*(new PerspectiveViewingVolume(1.0, 
                                                            1000.0,
                                                            4.0/3.0,
                                                            //float(frame->GetWidth())/frame->GetHeight(),
                                                            PI/4)));
    ISceneNode* scene     = new SceneNode();
    
    WiiMoteManager* wiimote = new WiiMoteManager();
    
    r->SetBackgroundColor(Vector<4,float>(0.0));

    TextureLoader* tl = new TextureLoader(*r, TextureLoader::RELOAD_QUEUED);
    //r->PreProcessEvent().Attach(*tl);
    WiiLightVisitor* lv   = new WiiLightVisitor();
    r->PreProcessEvent().Attach(*lv);

    
    //IModelResourcePtr res = ResourceManager<IModelResource>::Create("FutureTank/model.obj");
    // IModelResourcePtr res = ResourceManager<IModelResource>::Create("Dragon/DragonHead.obj");
    IModelResourcePtr res = ResourceManager<IModelResource>::Create("finn/Finn08_org.DAE");
    // IModelResourcePtr res = ResourceManager<IModelResource>::Create("arme_arne/ArmeArne02.DAE");
    res->Load();

    ISceneNode* fishModel = res->GetSceneNode();
    AnimationNode* animations = GetAnimationNode(fishModel);
    if( animations ){
        Animator* animator = new Animator(animations);
        if( animator->GetSceneNode() ){
            TransformationNode* fishTrans = new TransformationNode();
            fishTrans->AddNode(animator->GetSceneNode());
            fishModel = fishTrans;
        }
        engine->ProcessEvent().Attach(*animator);
        animator->SetActiveAnimation(0);
        animator->Play();
    }

    // ISceneNode* tank;
    // res->Load();
    // tank = res->GetSceneNode();
    // res->Unload();

    TransformationNode* tn = new TransformationNode();
    Rotator* rt = new Rotator(cam, tn);
    
    TransformationNode* ltn = new TransformationNode();
    PointLightNode* pln = new PointLightNode();
    ltn->AddNode(pln);
    ltn->Move(0.0,300.0,300.0);
    scene->AddNode(ltn);
    
    //tn->AddNode(fishModel);
    //scene->AddNode(tn);
    scene->AddNode(fishModel);

    // IRigidBody& body = phys->CreateRigidBody(new AABBShape(Vector<3,float>(-100.0), Vector<3,float>(100.0)),
    //                                          tn
    //                                          );
    

    r->ProcessEvent().Attach(*rv);
    cam->SetPosition(Vector<3,float>(0,0,50));
    cam->LookAt(Vector<3,float>(0.0,0.0,-1.0));
    canvas->SetScene(scene);
    canvas->SetViewingVolume(cam);
    canvas->SetRenderer(r);
    frame->SetCanvas(canvas);

    // Bind to the engine for processing time
    engine->InitializeEvent().Attach(*frame);
    engine->ProcessEvent().Attach(*frame);
    engine->ProcessEvent().Attach(*rt);
    engine->DeinitializeEvent().Attach(*frame);


    engine->InitializeEvent().Attach(*wiimote);
    engine->ProcessEvent().Attach(*wiimote);
    engine->DeinitializeEvent().Attach(*wiimote);

    wiimote->WiiMoteConnectEvent().Attach(*rt);
    wiimote->WiiMoteDisconnectEvent().Attach(*rt);
    

    tl->Load(*scene);
    engine->Start();

    delete engine;
    // delete env;

    // Return when the engine stops.
    return EXIT_SUCCESS;
}


