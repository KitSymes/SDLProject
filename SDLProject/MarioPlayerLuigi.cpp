#include "MarioPlayerLuigi.h"
#include "Texture2D.h"
#include "Constants.h"

MarioPlayerLuigi::MarioPlayerLuigi(SDL_Renderer* renderer, std::string imagePath, Vector2D start_position, MarioScreen* screen) : MarioCharacter(renderer, imagePath, start_position, screen)
{
	m_jump_force = 12.0f;
}

MarioPlayerLuigi::~MarioPlayerLuigi()
{

}

void MarioPlayerLuigi::Update(float deltaTime, SDL_Event e)
{
	MarioCharacter::Update(deltaTime, e);

	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case SDLK_LEFT:
			m_moving_left = true;
			break;
		case SDLK_RIGHT:
			m_moving_right = true;
			break;
		case SDLK_UP:
			Jump();
			break;
		}
		break;
	case SDL_KEYUP:
		switch (e.key.keysym.sym)
		{
		case SDLK_LEFT:
			m_moving_left = false;
			m_velocity_left = 0;
			break;
		case SDLK_RIGHT:
			m_moving_right = false;
			m_velocity_right = 0;
			break;
		}
		break;
	}
}