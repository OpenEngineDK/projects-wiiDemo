// Rotate a transformation node
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _OE_ROTATOR_H_
#define _OE_ROTATOR_H_

#include <Core/EngineEvents.h>
#include <Core/IListener.h>
#include <Scene/TransformationNode.h>

#include <Devices/WiiMoteManager.h>
#include <Devices/WiiMote.h>

#include <Math/Quaternion.h>
#include <Display/Camera.h>

#include <Utils/Timer.h>
#include <time.h>
#include <sys/time.h>
#include <ogc/lwp_watchdog.h>

using OpenEngine::Scene::TransformationNode;
using OpenEngine::Math::Quaternion;
using OpenEngine::Display::Camera;
using OpenEngine::Utils::Time;
using OpenEngine::Utils::Timer;

class Rotator : public OpenEngine::Core::IListener<OpenEngine::Core::ProcessEventArg>
              , public OpenEngine::Core::IListener<OpenEngine::Devices::WiiAccelerationEventArg> 
              , public OpenEngine::Core::IListener<OpenEngine::Devices::WiiButtonEventArg> 
              , public OpenEngine::Core::IListener<OpenEngine::Devices::WiiMoteConnectEventArg> 
              , public OpenEngine::Core::IListener<OpenEngine::Devices::WiiMoteDisconnectEventArg> {
private:
    Camera* cam;
    TransformationNode* t;
    Quaternion<float> old;
    bool move, left, right, forward, backward, firstConnect;
    Vector<3,float> dir;
    float speed;
    unsigned int time;
public:
    Rotator(Camera* cam, TransformationNode* t): cam(cam), t(t), move(false), left(false), right(false), forward(false), backward(false), firstConnect(true), speed(100.0) {
        // unsigned int c = clock();
        // time = float(c) / float(CLOCKS_PER_SEC);
        // struct timeval tv;
        // gettimeofday( &tv, NULL );
        // time = tv.tv_sec*1000 + tv.tv_usec;
        time = ticks_to_microsecs(gettick());

}
    virtual ~Rotator() {}

    void Handle(OpenEngine::Core::ProcessEventArg arg) {

        // unsigned int c = clock();
        // logger.info << "clock: " << c << logger.end;
        // float _time = float(c) / float(CLOCKS_PER_SEC);
        // logger.info << "time: " << _time << logger.end;
        // logger.info << "CLOCKS_PER_SEC: " << CLOCKS_PER_SEC << logger.end;

        // struct timeval tv;
        // gettimeofday( &tv, NULL );

        // float _time = tv.tv_sec*1000 + tv.tv_usec;

        // logger.info << "time of day sec:  " << tv.tv_sec << " usec: " << tv.tv_usec  << logger.end;
        // logger.info << "time: " << _time  << logger.end;

        unsigned int _time = ticks_to_microsecs(gettick());
        logger.info << "tic: " << _time << logger.end;

      //  Time _time = Timer::GetTime();
       //  if (_time < time) return;

        
       // unsigned int t = (_time - time).AsInt32();

        float dt = float(_time-time)*1e-6;
        logger.info << "dt: " << dt << logger.end;
        time = _time;
        float ms = dt*speed; // Key moving depends on the time
        
        // compute move difference
        float x=0, z=0;
        if (forward)  z += ms;
        if (backward) z -= ms;
        if (left)     x -= ms;
        if (right)    x += ms;
        
        
        // move the camera [ Move(long, tran, vert) ]
        if (x || z) cam->Move(z,x,0);
        // relative pitch (positive goes up)
        // cam->Move(dir);
        //t->Rotate(0.0,0.05,0.0);
    }

    void Handle(OpenEngine::Devices::WiiAccelerationEventArg arg) {
        Vector<3,float> acc = arg.gforce;
        Quaternion<float> q(acc[1], -acc[0], 0.0);
        old = Quaternion<float>(old, q, 0.1);
        old.Normalize();
        cam->SetDirection(old);
    }

    void Handle(OpenEngine::Devices::WiiMoteConnectEventArg arg) {
        if (firstConnect) {
            arg.mote.WiiAccelerationEvent().Attach(*this);
            arg.mote.WiiButtonEvent().Attach(*this);
            firstConnect = false;
        } 
    }

    void Handle(OpenEngine::Devices::WiiMoteDisconnectEventArg arg) {
        // arg.mote.WiiAccelerationEvent().Detach(*this);
        // arg.mote.WiiButtonEvent().Detach(*this);
    }

    void Handle(OpenEngine::Devices::WiiButtonEventArg arg) {
        if (arg.type == OpenEngine::Devices::EVENT_PRESS) {
            // Vector<3,float> forward = cam->GetDirection().RotateVector(Vector<3,float>(0.0,-1.0,0.0))
            switch (arg.btn) {
            case OpenEngine::Devices::WII_REMOTE_RIGHT: 
                right = true;
                // dir = cam->GetDirection().RotateVector(Vector<3,float>(0.0,0.0,-1.0));
                // dir = Vector<3,float>(0,0,-speed);
                break;
            case OpenEngine::Devices::WII_REMOTE_LEFT: 
                left = true;
                // dir = cam->GetDirection().RotateVector(Vector<3,float>(0.0, 0.0, 1.0));
                break;
            case OpenEngine::Devices::WII_REMOTE_DOWN: 
                backward = true;
                // dir = cam->GetDirection().RotateVector(Vector<3,float>(0.0, 0.0, 1.0));
                // dir = dir % Vector<3,float>(0.0,1.0,0.0);
                break;
            case OpenEngine::Devices::WII_REMOTE_UP:                 
                forward = true;
                // dir = cam->GetDirection().RotateVector(Vector<3,float>(0.0, 0.0, 1.0));
                // dir = dir % Vector<3,float>(0.0,-1.0,0.0);
                break;
            default: break;   
            }
        }
        if (arg.type == OpenEngine::Devices::EVENT_RELEASE) {
            switch (arg.btn) {
            case OpenEngine::Devices::WII_REMOTE_2:
                arg.mote.Disconnect();
                break;
            case OpenEngine::Devices::WII_REMOTE_1: 
                exit(0);
            case OpenEngine::Devices::WII_REMOTE_UP: 
                forward = false;
                break;
            case OpenEngine::Devices::WII_REMOTE_DOWN: 
                backward = false;
                break;
            case OpenEngine::Devices::WII_REMOTE_LEFT: 
                left = false;
                break;
            case OpenEngine::Devices::WII_REMOTE_RIGHT: 
                right = false;
            default: break;
            }
        }
    }
};

#endif // _OE_ROTATOR_H_
