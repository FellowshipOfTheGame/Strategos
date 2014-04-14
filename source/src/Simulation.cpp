//
//  Simulation.cpp
//  Strategos
//
//  Created by guicc on 8/8/12.
//  Copyright (c) 2012 Fellowship of the Game. All rights reserved.
//

#include "Simulation.h"
#include "Global.h"

#if defined(WIN32)
    #include "SDL2_gfx\SDL2_gfxPrimitives.h"
#elif defined(__APPLE__)
    #include "SDL2_gfx/SDL2_gfxPrimitives.h"
#endif // _MAC_OS_

Simulation::Simulation(STATE previous) : StateMachine(previous, GAMEPLAY, GAMEPLAY)
{
    printf("Starting Simulation...\n");
    gameRunning = true;
    simulationSTATE = _SIM_CONTINUE_;
    Resource *resource = Game::getGlobalGame()->getResourceMNGR();
    int screenWidth = Game::getGlobalGame()->getWidth();
    int screenHeight = Game::getGlobalGame()->getHeight();

    resource->AddImage("assets/base.gfx", "simulation-bg-Far");
    resource->AddImage("assets/base.gfx", "simulation-bg-Mid");
    resource->AddImage("assets/base.gfx", "simulation-bg-Close");
    resource->AddImage("assets/base.gfx", "simulation-bg-Nebula");
    resource->AddImage("assets/army.gfx", "human-ships");

    background[0] = resource->GetImage("simulation-bg-Far");
    background[1] = resource->GetImage("simulation-bg-Mid");
    background[2] = resource->GetImage("simulation-bg-Close");
    background[3] = resource->GetImage("simulation-bg-Nebula");
//
    SDL_SetTextureBlendMode(background[0]->getSDLTexture(), SDL_BLENDMODE_ADD);
    SDL_SetTextureBlendMode(background[1]->getSDLTexture(), SDL_BLENDMODE_ADD);
    SDL_SetTextureBlendMode(background[2]->getSDLTexture(), SDL_BLENDMODE_ADD);
    SDL_SetTextureBlendMode(background[3]->getSDLTexture(), SDL_BLENDMODE_ADD);

    if (!Game::getGlobalGame()->getArmy1()){
    	printf ("primeiro ponteiro nulo\n");
    }else if (!Game::getGlobalGame()->getArmy2()){
        printf ("segundo ponteiro nulo\n");
    }else{
    	printf ("ambos nao nulos");
    }
    //simulationWorld = new World("rbrararb", "rarbrbra");
    Game::getGlobalGame()->generateSprites(Game::getGlobalGame()->getArmy1()->getDictionary());
    Game::getGlobalGame()->generateSprites(Game::getGlobalGame()->getArmy2()->getDictionary());
    simulationWorld = new World(Game::getGlobalGame()->getArmy1(), Game::getGlobalGame()->getArmy2());

    camera = new Camera(0, 0, screenWidth, screenHeight, screenWidth*2, screenHeight*1.5f);
    bgOffsetX = bgOffsetY = 0;

    renderCombat = SDL_CreateTexture(Game::getGlobalGame()->getRenderer(), SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_TARGET, 2048, 2048);

    if(renderCombat == NULL)
    {
        printf("SDL_CreateTexture failed: %s\n", SDL_GetError());
        exit(-9);
    }

    selectedUnit = NULL;
    printf("Simulation Ready!\n");
}

Simulation::~Simulation()
{
    delete simulationWorld;
    delete camera;
}

void Simulation::onInputEvent(cGuiElement* element, INPUT_EVENT action, SDL_Keysym key, Uint8 button)
{

}

void Simulation::onKeyDownEvent(SDL_Keysym key)
{
    switch (key.sym)
    {
        case SDLK_ESCAPE:
            setNext(MENU);
        break;

        case SDLK_RETURN:
            if (!gameRunning)
                setNext(RESULTS);
        break;

        default: break;
    }
}

void Simulation::onKeyUpEvent(SDL_Keysym key)
{

}

void Simulation::onKeyHeldEvent(const Uint8 *keystate)
{
    if(keystate[SDL_SCANCODE_LEFT])
    {
        camera->moveH(-15);
        bgOffsetX += 1.25f;
    }

    if(keystate[SDL_SCANCODE_RIGHT])
    {
        camera->moveH(15);
        bgOffsetX -= 1.0f;
    }

    if(keystate[SDL_SCANCODE_UP])
    {
        camera->moveV(-10);
    }

    if(keystate[SDL_SCANCODE_DOWN])
    {
        camera->moveV(10);
    }
}

void Simulation::onMouseDownEvent(Uint8 button)
{
    if(button == SDL_BUTTON_LEFT)
    {
        selectedUnit = checkClickOn(simulationWorld->getArmy(0));
        if (!selectedUnit)
            selectedUnit = checkClickOn(simulationWorld->getArmy(1));
        // TODO: Assim temos preferencia para exercito 0... ????
    }
}

Unit* Simulation::checkClickOn(Army *army)
{
    const vector<Unit*>& units = army->getUnits();
    for(unsigned int i = 0; i < units.size(); i++)
    {
        if( units[i]->hover(camera->getX(), camera->getY()) ){
            return units[i];
        }
    }

    return 0;
}

void Simulation::onMouseUpEvent(Uint8 button)
{

}

void Simulation::Logic()
{
    camera->update();
    bgOffsetX = camera->getX();
    bgOffsetY = camera->getY();

    if (gameRunning)
    {
         if (simulationSTATE == _SIM_CONTINUE_)
         {
             // Aumentar velocidade da simulacao
            for (int i = 0; i < 1 && simulationSTATE == _SIM_CONTINUE_; ++i){
                 simulationSTATE = simulationWorld->simulateStep();
                 //printf("END STEP - SIMULATION: %d\n", simulationSTATE);
            }
        }
        else
        {
            switch (simulationSTATE)
            {
                case _SIM_ARMY0_WIN_:
                    printf("Army 1 venceu\n");
                break;

                case _SIM_ARMY1_WIN_:
                    printf("Army 2 venceu\n");
                break;

                case _SIM_DRAW_:
                    printf("Empate\n");
                break;
            }
            gameRunning = false;
            printf("Enter to see the results!\n");
         }
    }
 }

void Simulation::Render()
{
    SDL_Renderer *renderer = Game::getGlobalGame()->getRenderer();

    Game::getGlobalGame()->setBackgroundColor(0, 0, 0);

    // Render do Fundo
//    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    drawBG(background[0], bgOffsetX, bgOffsetY, -0.1, renderer);
    drawBG(background[1], bgOffsetX, bgOffsetY, -0.13, renderer);
    drawBG(background[2], bgOffsetX, bgOffsetY, -0.2, renderer);
    drawBG(background[3], bgOffsetX, bgOffsetY, -0.28, renderer);

    //Just TESTING
    SDL_SetRenderTarget(renderer, renderCombat);
        // Limpar textura
        SDL_SetTextureBlendMode(renderCombat, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_NONE);
        SDL_SetTextureBlendMode(renderCombat, SDL_BLENDMODE_BLEND);

        simulationWorld->render();
    SDL_SetRenderTarget(renderer, 0);

    // Renderizar imagem de combate na tela
    SDL_Rect rect;
    rect.x = -camera->getX();  rect.w = 2048;
    rect.y = -camera->getY();  rect.h = 2048;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_NONE);
    SDL_RenderCopy( renderer, renderCombat, 0, &rect);

    if(selectedUnit)
    {
        filledCircleRGBA( renderer, selectedUnit->getAvgX()-camera->getX(), selectedUnit->getAvgY()-camera->getY(), 64, 0, 0, 255, 128 );
    }
}

void Simulation::drawBG(Image* img, double x, double y, double factor, SDL_Renderer *renderer)
{
    img->DrawImage(x*factor-512, y*factor-512, renderer);
}

void Simulation::Clean()
{


}


