﻿#include "Game.h"

Game::Game(SDL_Renderer * renderer) :
	Scene(renderer)
{
	// Загрузка текстуры карты
	gameTextures.push_back(IMG_LoadTexture(renderer, "Resources/TopPanel.png"));
	// Загрузка текстуры башен
	gameTextures.push_back(IMG_LoadTexture(renderer, "Resources/TowerTileSet.png"));
	// Загрузка текстуры врагов
	gameTextures.push_back(IMG_LoadTexture(renderer, "Resources/Enemy.png"));
	// Загрузка текстуры снаряда
	gameTextures.push_back(IMG_LoadTexture(renderer, "Resources/Bullet.png"));
	topPanel = new Object(gameTextures[0]);
	// Типы тайлов на карте
	char map[11][17] =
	{
		"0000000000000000",
		"0000000000000000",
		"0000000000000000",
		"1111111100000000",
		"0000000100000000",
		"0000000100000000",
		"0000000100000000",
		"0000000111111111",
		"0000000000000000",
		"0000000000000000",
		"0000000000000000",
	};
	for (int i = 0; i < 16; i++)
	{
		towers.push_back(std::vector<Tower*>());
		for (int j = 0; j < 11; j++)
		{
			if (map[j][i] == '0')
				towers[i].push_back(new Tower(gameTextures[1], gameTextures[3], i * 50, j * 50 + 50, TileType::EmptyTile));
			if (map[j][i] == '1')
				towers[i].push_back(new Tower(gameTextures[1], gameTextures[3], i * 50, j * 50 + 50, TileType::RoadTile));

		}
		std::cout << std::endl;
	}
	paths.push_back({ Path::Directions::Right, 400 });
	paths.push_back({ Path::Directions::Bottom, 200 });
	paths.push_back({ Path::Directions::Right, 450 });
}

Game::~Game()
{
}

void Game::Execution()
{
	if (castleHealth > 0)
	{
		if (enemies.empty() && enemyRemains == 0)
		{
			Wave += 1;
			enemyRemains = 5 * Wave;
			respawnDelay -= 10;
			std::cout << std::endl;
			std::cout << "Wave " << Wave << " started" << std::endl;
			std::cout << "Current gold: " << gold << std::endl;
			std::cout << "Castle health: " << castleHealth << std::endl;
			std::cout << std::endl;
		}
		if (SDL_GetTicks() - lastRespawnTime >= respawnDelay && enemyRemains != 0)
		{
			enemyRemains -= 1;
			enemies.push_back(new Enemy(gameTextures[2], spawnX, spawnY, paths));
			lastRespawnTime = SDL_GetTicks();
			std::cout << "Enemy spawned" << std::endl;
		}
		for (int i = 0; i < bullets.size(); i++)
		{
			bullets[i]->Move();
			if (bullets[i]->EnemyExisting())
			{
				if (bullets[i]->Hit())
				{
					delete bullets[i];
					bullets.erase(bullets.begin() + i);
					if (i < bullets.size())
						i--;
				}
			}
			else
			{
				delete bullets[i];
				bullets.erase(bullets.begin() + i);
				if (i < bullets.size())
					i--;
			}
		}
		// Перемещение противников
		for (int i = 0; i < enemies.size(); i++)
		{
			enemies[i]->Move();
			if (enemies[i]->Complete())
			{
				delete enemies[i];
				castleHealth -= 1;
				enemies.erase(enemies.begin() + i);
				std::cout << "Castle damaged, health remaining: " << castleHealth << std::endl;
			}
			if (enemies[i]->Dead())
			{
				delete enemies[i];
				gold += 20;
				enemies.erase(enemies.begin() + i);
			}
		}
		for (int i = 0; i < 16; i++)
		{
			for (int j = 0; j < 11; j++)
			{
				towers[i][j]->Shoot(enemies, &bullets);
			}
		}
	}
	displayedObject.clear();
	if (topPanel)
	{
		displayedObject.push_back(topPanel);
	}
	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 11; j++)
		{
			displayedObject.push_back(towers[i][j]);
		}
	}
	for (int i = 0; i < enemies.size(); i++)
	{
		displayedObject.push_back(enemies[i]);
	}
	for (int i = 0; i < bullets.size(); i++)
	{
		displayedObject.push_back(bullets[i]);
	}
}

void Game::OnMouseClick(SDL_MouseButtonEvent buttonEvent)
{
	if (castleHealth > 0)
	{
		if (buttonEvent.y > 50)
			towers[buttonEvent.x / 50][buttonEvent.y / 50 - 1]->Upgrade(&gold);
	}
}