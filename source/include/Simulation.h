//
//  Simulation.h
//  Strategos
//
//  Created by guicc on 8/8/12.
//  Copyright (c) 2012 Fellowship of the Game. All rights reserved.
//

#ifndef _SIMULATION_H
#define _SIMULATION_H

#include "StateMachine.h"
#include "GuiElements.h"
#include "World.h"
#include "Camera.h"

class Simulation : public StateMachine
{
    private:
        bool gameRunning;
        int simulationSTATE;
        int simulationSpeed; // 0 = paused

        const Image *background[4];
        Image *humanShips;

        World *simulationWorld;
        Unit *selectedUnit;

        Camera *camera;
        float bgOffsetX, bgOffsetY;

        Unit* checkClickOn(const Army *army);
        void drawBG(const Image* img, double x, double y, double factor, SDL_Renderer *renderer);


    public:
        Simulation(STATE previous);
        ~Simulation();

        void onInputEvent(cGuiElement* element, INPUT_EVENT action, SDL_Keysym key, Uint8 button) override;
        void onKeyDownEvent(SDL_Keysym key) override;
        void onKeyUpEvent(SDL_Keysym key) override;
        void onKeyHeldEvent(const Uint8 *keystate) override;
        void onMouseDownEvent(Uint8 button) override;
        void onMouseUpEvent(Uint8 button) override;
        void Logic();
        void Render();
        void Clean();
};

#endif
