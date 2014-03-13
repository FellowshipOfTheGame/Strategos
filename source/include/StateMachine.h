/*
 *  Strategos - StateMachine.h
 *
 *  Esta classe eh uma definicao generica de Maquina de Estados.
 *	Os estados em si para o Strategos estao declarados como 'enumeration'
 *	no arquivo Global.h
 *
 *	Last update: 06/07/2012
 */

#ifndef _STATE_MACHINE_H_
#define _STATE_MACHINE_H_

#include <vector>

#include "GuiElements.h"
#include "GuiTactics.h"

typedef enum {NONE, MENU, CREATE_ARMY, UNIT_SETUP, SIMULATE, GAMEPLAY, OPTIONS, RESULTS, EXIT} STATE;



class StateMachine
{
private:
	STATE previous;
	STATE current;
	STATE next;

	std::vector <cGuiElement*> guiElements;
	cGuiElement *focus;
public:
	StateMachine(STATE previous, STATE current, STATE next)
    {
        this->previous = previous;
        this->current = current;
        this->next = next;
        focus = NULL;
    }

	virtual ~StateMachine() { } //Deve ser Virtual pois essa clase eh usada como base apenas.

    void setPrevious(STATE previous) {this->previous = previous;}
    void setCurrent(STATE current) {this->current = current;}
    void setNext(STATE next) {this->next = next;}

    STATE getPrevious() {return previous;}
    STATE getCurrent() {return current;}
    STATE getNext() {return next;}

    void addGuiElement(cGuiElement *element)
    {
        guiElements.push_back(element);
    }

    virtual void onInputEvent(cGuiElement* element, INPUT_EVENT action, SDL_Keysym key, Uint8 button) = 0;
    virtual void onKeyDownEvent(SDL_Keysym key){}
    virtual void onKeyUpEvent(SDL_Keysym key){}
    virtual void onMouseDownEvent(Uint8 button){}
    virtual void onMouseUpEvent(Uint8 button){}
    virtual void onKeyHeldEvent(const Uint8 *keystate){}

    void Input()
    {
        SDL_Event event;

        while(SDL_PollEvent(&event))
        {
            switch (event.type){
                case SDL_QUIT:
                    setNext(EXIT);
                break;

                case SDL_KEYDOWN:
                    onKeyDownEvent(event.key.keysym);
                break;

                case SDL_KEYUP:
                    onKeyUpEvent(event.key.keysym);
                break;

                case SDL_MOUSEBUTTONDOWN:
                    onMouseDownEvent(event.button.button);
                break;

                case SDL_MOUSEBUTTONUP:
                    onMouseUpEvent(event.button.button);
                break;

                default:
//                    printf("Unhandled event: %d\n", event.type);
                break;
            }

            //Atualizar todos os elementos de GUI
            for (unsigned int i = 0; i < guiElements.size(); i++)
            {
                INPUT_EVENT newEvent = guiElements[i]->input(event);
                if ( dynamic_cast<TextField*>(guiElements[i]) != NULL)
                {
                    if (newEvent == MOUSE_RELEASED_EVENT)
                    {
                        if (focus != NULL)
                        {
                            focus->setActive(false);
                        }
                        focus =  guiElements[i];
                        focus->setActive(true);
                    }
                }

                if (newEvent != NO_EVENT) // Nao ocorre evento quando recebe NO_EVENT (valor 0)
                {
                    printf("input()");
                    //Quando necessario chamamos a funcao que indica que um evento ocorreu
                    onInputEvent(guiElements[i], newEvent, event.key.keysym, event.button.button);
                }
            }
        }

        //Get KeyStates in case the key is being held
        const Uint8 *keystate = SDL_GetKeyboardState(NULL);
        onKeyHeldEvent(keystate);
    }

    void updateGuiElements()
    {
        std::vector <cGuiElement*>::iterator it = guiElements.begin();
        while (it != guiElements.end())
        {
            if((*it)->isEnabled())
                (*it)->update();

            it++;
        }
    }

    void drawGuiElements()
    {
        std::vector <cGuiElement*>::iterator it = guiElements.begin();
        while (it != guiElements.end())
        {
            if((*it)->isVisible())
            {
                (*it)->draw();
            }

            it++;
        }
    }

    virtual void Logic() { }
    virtual void Render() { }
    virtual void Clean() { }
};

#endif
