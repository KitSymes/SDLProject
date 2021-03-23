#include "MapScreen.h"
#include "Texture2D.h"
#include "Character.h"
#include "GameScreenManager.h"
#include "BattleScreen.h"
#include "SaveData.h"

MapScreen::MapScreen(SDL_Renderer* renderer, GameScreenManager* gsm) : GameScreen(renderer, gsm)
{
	SetUpLevel();
}

MapScreen::~MapScreen()
{
	currentPos = nullptr;

	delete m_background_texture;
	m_background_texture = nullptr;
	delete myChar;
	myChar = nullptr;
	delete m_levelLocked_texture;
	m_levelLocked_texture = nullptr;
	delete m_levelUnlocked_texture;
	m_levelUnlocked_texture = nullptr;
	delete m_levelComplete_texture;
	m_levelComplete_texture = nullptr;

	delete start;
	delete firstLevel;
	delete forestLevel;
	delete alternateLevel;
	delete beforeBridge;
	delete bridgeLevel;
	delete lastSplit;
	delete castleLevel;

	SaveData::Instance()->Save("AutoSave");
}

bool MapScreen::SetUpLevel()
{
	start = new PoI(11, 9);
	start->locked = false;
	start->complete = true;
	start->north = new int[] {0, 0};

	beforeBridge = new PoI(7, 4);
	beforeBridge->locked = false;
	beforeBridge->complete = true;
	beforeBridge->north = new int[] {0, 1, 1, 1, 1, 1};
	beforeBridge->west = new int[] {3};
	beforeBridge->south = new int[] {2, 2, 2};

	lastSplit = new PoI(2, 4);
	lastSplit->locked = false;
	lastSplit->complete = true;
	lastSplit->north = new int[] {0, 0};
	lastSplit->south = new int[] {2, 2};
	lastSplit->east = new int[] {1, 1, 1, 1};


	firstLevel = new PoI(11, 7);
	firstLevel->mapFile = "map1.txt";
	firstLevel->locked = false;
	firstLevel->returnDir = 2;
	firstLevel->south = new int[] {2, 2};
	firstLevel->east = new int[] {1, 0, 0, 0, 0};
	firstLevel->west = new int[] {3, 3, 3, 3};

	forestLevel = new PoI(7, 7);
	forestLevel->returnDir = 1;
	forestLevel->east = new int[] {1, 1, 1, 1};
	forestLevel->north = new int[] {0, 0, 0};

	alternateLevel = new PoI(12, 3);
	alternateLevel->returnDir = 2;
	alternateLevel->west = new int[] {3, 3, 3, 3, 3, 2};
	alternateLevel->south = new int[] {2, 2, 2, 2, 3};

	bridgeLevel = new PoI(6, 4);
	bridgeLevel->returnDir = 1;
	bridgeLevel->west = new int[] {3, 3, 3, 3};
	bridgeLevel->east = new int[] {1};

	castleLevel = new PoI(2, 2);
	castleLevel->returnDir = 2;
	castleLevel->south = new int[] {2, 2};

	start->northPoI = firstLevel;
	firstLevel->southPoI = start;
	firstLevel->eastPoI = alternateLevel;
	firstLevel->westPoI = forestLevel;
	forestLevel->eastPoI = firstLevel;
	forestLevel->northPoI = beforeBridge;
	alternateLevel->southPoI = firstLevel;
	alternateLevel->westPoI = beforeBridge;
	beforeBridge->southPoI = forestLevel;
	beforeBridge->northPoI = alternateLevel;
	beforeBridge->westPoI = bridgeLevel;
	bridgeLevel->eastPoI = beforeBridge;
	bridgeLevel->westPoI = lastSplit;
	lastSplit->eastPoI = bridgeLevel;
	lastSplit->northPoI = castleLevel;
	castleLevel->southPoI = lastSplit;

	m_background_texture = new Texture2D(m_renderer, 512, 416);
	if (!m_background_texture->LoadFromFile("Images/WorldMap/WorldMap.png"))
	{
		std::cout << "Failed to load background texture!" << std::endl;
	}

	m_levelLocked_texture = new Texture2D(m_renderer, 32, 64);
	if (!m_levelLocked_texture->LoadFromFile("Images/WorldMap/Locked.png"))
	{
		std::cout << "Failed to load level locked texture!" << std::endl;
	}

	m_levelUnlocked_texture = new Texture2D(m_renderer, 32, 64);
	if (!m_levelUnlocked_texture->LoadFromFile("Images/WorldMap/Unlocked.png"))
	{
		std::cout << "Failed to load level unlocked texture!" << std::endl;
	}

	m_levelComplete_texture = new Texture2D(m_renderer, 32, 64);
	if (!m_levelComplete_texture->LoadFromFile("Images/WorldMap/Complete.png"))
	{
		std::cout << "Failed to load level complete texture!" << std::endl;
	}

	myChar = new Character(m_renderer, "Images/Allies/Infantry/Lancer.png", Vector2D(11 * 32 - 10, 9 * 32 - 10));
	currentPos = start;

	for (Character* c : SaveData::Instance()->m_allies)
		c->SetAlive(false);

	return true;
}

void MapScreen::Render()
{
	m_background_texture->Render(Vector2D(), SDL_FLIP_NONE);

	PoI* poiPointer = firstLevel;
	while (true)
	{
		if (poiPointer->complete)
			m_levelComplete_texture->Render(Vector2D(poiPointer->x * 32.0f, poiPointer->y * 32.0f), SDL_FLIP_NONE);
		else if (poiPointer->locked)
			m_levelLocked_texture->Render(Vector2D(poiPointer->x * 32.0f, poiPointer->y * 32.0f), SDL_FLIP_NONE);
		else
			m_levelUnlocked_texture->Render(Vector2D(poiPointer->x * 32.0f, poiPointer->y * 32.0f), SDL_FLIP_NONE);

		if (poiPointer == firstLevel)
			poiPointer = forestLevel;
		else if (poiPointer == forestLevel)
			poiPointer = alternateLevel;
		else if (poiPointer == alternateLevel)
			poiPointer = bridgeLevel;
		else if (poiPointer == bridgeLevel)
			/*poiPointer = castleLevel;
		else if (poiPointer == castleLevel)*/
			break;
	}
	myChar->Render();
}

void MapScreen::Update(float deltaTime, SDL_Event e)
{
	moving = false;
	myChar->Update(deltaTime, e);
	if (!moving) // TODO actually move smoothly
		switch (e.type)
		{
		case SDL_KEYDOWN:
			switch (e.key.keysym.sym)
			{
			case SDLK_w:
				if (currentPos->north != nullptr && (currentPos->complete || currentPos->returnDir == 0))
				{
					currentPos = currentPos->northPoI;
					myChar->SetRawPosition(Vector2D(currentPos->x * 32 - 10, currentPos->y * 32 - 10));
					moving = true;
				}
				break;
			case SDLK_s:
				if (currentPos->south != nullptr && (currentPos->complete || currentPos->returnDir == 2))
				{
					currentPos = currentPos->southPoI;
					myChar->SetRawPosition(Vector2D(currentPos->x * 32 - 10, currentPos->y * 32 - 10));
					moving = true;
				}
				break;
			case SDLK_d:
				if (currentPos->east != nullptr && (currentPos->complete || currentPos->returnDir == 1))
				{
					currentPos = currentPos->eastPoI;
					myChar->SetRawPosition(Vector2D(currentPos->x * 32 - 10, currentPos->y * 32 - 10));
					moving = true;
				}
				break;
			case SDLK_a:
				if (currentPos->west != nullptr && (currentPos->complete || currentPos->returnDir == 3))
				{
					currentPos = currentPos->westPoI;
					myChar->SetRawPosition(Vector2D(currentPos->x * 32 - 10, currentPos->y * 32 - 10));
					moving = true;
				}
				break;
			case SDLK_RETURN:
			case SDLK_KP_ENTER:
				if (!currentPos->mapFile.empty())
				{
					std::cout << currentPos->mapFile << std::endl;
					// TODO pass player's troops
					BattleScreen* bs = new BattleScreen(m_renderer, m_gsm, (char*)("BattleMaps/" + currentPos->mapFile).c_str());
					m_gsm->ChangeScreen(bs);
				}
				break;
			}
			break;
		}
}
