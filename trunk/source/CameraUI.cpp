/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "CameraModel.h"
#include "Player.h"
#include "Main.h"

#include "IwUI.h"
#include "IwGxFont.h"
#include "IwResManager.h"

CIwUIButton* gameOverButton = NULL;

class GameOverEventHandler : public CIwUIElementEventHandler {
public:
    //HandleEvent is passed up the element hierarchy
    virtual bool HandleEvent(CIwEvent* pEvent)
    {
        if (pEvent->GetID() == IWUI_EVENT_BUTTON)
        {
            getPlayer()->resurrect();

            //return true, as event has been handled
            return true;
        }

        return false;
    }

    //FilterEvent is passed down the element hierarchy
    virtual bool FilterEvent(CIwEvent* pEvent)
    {
        //return false, since filtering is not necessary
        return false;
    }
};

void CameraUIInit()
{
	//Initialise the IwUI module
    IwUIInit();
	IwResManagerInit();
    IwGxFontInit();

    //Instantiate the view and controller singletons.
    //IwUI will not instantiate these itself, since they can be subclassed to add functionality.
    new CIwUIController;
    new CIwUIView;

    //Load group which contains the ui specifications
    IwGetResManager()->LoadGroup("ui/GameOverButton.group");

	gameOverButton = (CIwUIButton*) CIwUIElement::CreateFromResource("GameOverButton");
	gameOverButton->AddEventHandler(new GameOverEventHandler());

	// Font size is fixed so it looks stupid if the box is huge but the text is small.
	// Also it looks stupid if the box barely covers the text on a big screen.
	int gameOverW = IwGxGetScreenWidth()/2;
	int gameOverH = IwGxGetScreenHeight()/6;
	gameOverW = gameOverW < 220 ? 220 : gameOverW;
	gameOverH = gameOverH < 110 ? 110 : gameOverH;
	gameOverW = gameOverW > 350 ? 350 : gameOverW;
	gameOverH = gameOverH > 150 ? 150 : gameOverH;
	gameOverButton->SetSize(CIwVec2(gameOverW, gameOverH));
	gameOverButton->SetSizeMin(CIwVec2(gameOverW, gameOverH));
	
	gameOverButton->SetPos(CIwVec2(IwGxGetScreenWidth()/2 - gameOverButton->GetSize().x/2, 
		IwGxGetScreenHeight()/2 - gameOverButton->GetSize().y/2));

	gameOverButton->InvalidateLayout();
	gameOverButton->SetVisible(false);

    IwGetUIView()->AddElement(gameOverButton);
}

bool CameraUIUpdate()
{
    //Update the controller (this will generate control events etc.)
    IwGetUIController()->Update();

    //Update the view (this will do animations etc.)
	//Pass 24 fps to the update function
    IwGetUIView()->Update(FRAME_DURATION);

    return true;
}

void CameraUIRender() {
	//Lighting is required to colour fonts
    IwGxLightingOff();

	//Render the UI
    IwGetUIView()->Render();
}

void CameraUITerm()
{
	delete IwGetUIController();
    delete IwGetUIView();

	IwGxFontTerminate();
    IwResManagerTerminate();
	IwUITerminate();
}

void showGameOverButton(bool show) {
	gameOverButton->SetVisible(show);
}
