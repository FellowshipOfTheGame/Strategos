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
        Image *background[4];
        Image *humanShips;

        World *simulationWorld;
        Unit *selectedUnit;

        Camera *camera;
        float bgOffsetX, bgOffsetY;

        // Render
        SDL_Texture* renderCombat;

        Unit* checkClickOn(Army *army);
        void drawBG(Image* img, double x, double y, double factor, SDL_Renderer *renderer);


    public:
        Simulation(STATE previous);
        ~Simulation();

        void onInputEvent(cGuiElement* element, INPUT_EVENT action, SDL_Keysym key, Uint8 button);
        void onKeyDownEvent(SDL_Keysym key);
        void onKeyUpEvent(SDL_Keysym key);
        void onKeyHeldEvent(const Uint8 *keystate);
        void onMouseDownEvent(Uint8 button);
        void onMouseUpEvent(Uint8 button);
        void Logic();
        void Render();
        void Clean();
};

#endif
