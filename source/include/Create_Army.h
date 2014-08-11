/*
 *  Strategos - CREATE_ARMY.h
 *
 *  Este eh uma visualizacao do STATE CREATE_ARMY.
 *	O CREATE_ARMY sera a primeira tela a ser exibida e eh o retorno
 *  para quando os outros estados acabarem.
 *
 *	Last update: 06/07/2012
 */

#ifndef _CREATE_ARMY_H_
#define _CREATE_ARMY_H_

#include "StateMachine.h"
#include "GuiElements.h"
#include "Dictionary.h"

class Create_Army : public StateMachine
{
private:
    const Image *imgBackground;
    Button *btn_Back, *btn_Next, *btn_Load;

    Label *lbl_ArmyName, *lbl_GenName, *lbl_Descr, *lbl_Title;

    TextField *txt_ArmyName, *txt_GenName;
    ComboBox *cmb_species, *cmb_armys;

    StatusBox *st_box;
    ImageBox *bx1,*bx2,*bx3,*bx4;

    Dictionary *dct;

public:
    Create_Army(STATE previous);
    ~Create_Army();

    void onInputEvent(cGuiElement* element, INPUT_EVENT action, SDL_Keysym key, Uint8 button) override;
    void Logic();
    void Render();
    void Clean();
};

#endif
