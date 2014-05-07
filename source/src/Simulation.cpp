//  Simulation.cpp

#include "Simulation.h"

#include "Global.h"
#include "Game.h"

#include "SDL2_gfx/SDL2_gfxPrimitives.h"

Simulation::Simulation(STATE previous)
    : StateMachine(previous, GAMEPLAY, GAMEPLAY),
        simulationSpeed(0)
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
    	exit(-5);
    }else if (!Game::getGlobalGame()->getArmy2()){
        printf ("segundo ponteiro nulo\n");
    	exit(-5);
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

    if(renderCombat == nullptr)
    {
        printf("SDL_CreateTexture failed: %s\n", SDL_GetError());
        exit(-9);
    }

    selectedUnit = nullptr;
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

        case SDLK_SPACE:
            simulationSpeed = 0;
        break;

        case SDLK_0:
            simulationSpeed = 0;
        break;

        case SDLK_1:
            simulationSpeed = 1;
        break;

        case SDLK_KP_PLUS:
        case SDLK_PLUS:
        case SDLK_PAGEUP:
            ++simulationSpeed;
        break;

        case SDLK_KP_MINUS:
        case SDLK_MINUS:
        case SDLK_PAGEDOWN:
            if (simulationSpeed > 0)
                --simulationSpeed;
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
        if (selectedUnit == nullptr)
            selectedUnit = checkClickOn(simulationWorld->getArmy(1));

        if (selectedUnit != nullptr)
        {
            selectedUnit->printInfo();
        }
    }
}

Unit* Simulation::checkClickOn(const Army *army)
{
    int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);

	mouseX += camera->getX();
	mouseY += camera->getY();

    const std::vector<Unit*>& units = army->getUnits();
    for(unsigned int i = 0; i < units.size(); i++)
    {
        if( units[i]->getNShipsAlive() > 0 && units[i]->hover(mouseX, mouseY) ){
            return units[i];
        }
    }

    return nullptr;
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
            for (int i = 0; i < simulationSpeed && simulationSTATE == _SIM_CONTINUE_; ++i){
                 simulationSTATE = simulationWorld->simulateStep();
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
            printf ("\t\tarmy 0\n");
            CombatLog *log = simulationWorld->getCombatLog(0);
            log->setRegister(simulationWorld->getArmy(0)->unifyCombatRound());
            Game::getGlobalGame()->setCombatLog(0, log);
            printf ("\t\tarmy 1\n");
            log = simulationWorld->getCombatLog(1);
			log->setRegister(simulationWorld->getArmy(1)->unifyCombatRound());
            Game::getGlobalGame()->setCombatLog(1, log);
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


