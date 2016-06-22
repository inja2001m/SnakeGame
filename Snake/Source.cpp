#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

#define WIDTH 20
#define HEIGHT 10
enum Tile
{
	ROAD,
	WALL,
	FRUIT,
	SNAKE,
	SNAKEHEAD
};
enum CharDir
{
	NONE = 0,
	UP = 72,     
	LEFT = 75,   
	RIGHT = 77,  
	DOWN = 80    
};

class Snake 
{
public:
	Snake() {};
	Snake(int x, int y) : posX(x), posY(y) {};
	int posX, posY;
};

int map[HEIGHT][WIDTH] = { Tile::ROAD };
int tailNum;
bool isBeFruit, isRun;
CharDir playerDir;
Snake fruit;
vector<Snake> snakes;

void createFruit()
{
	int x, y;
	
	x = rand() % (WIDTH - 2) + 1;
	y = rand() % (HEIGHT - 2) + 1;

	while (map[y][x] == Tile::SNAKE || map[y][x] == Tile::SNAKEHEAD)
	{
		x = rand() % (WIDTH - 2) + 1;
		y = rand() % (HEIGHT - 2) + 1;
	}

	map[y][x] = Tile::FRUIT;

	fruit.posX = x;
	fruit.posY = y;

	isBeFruit = true;
}


void update()
{
	int oldPosX, oldPosY;
	static bool isFirstTouch = false;
	static bool isFirstEat = false;

	// 키 입력 구문
	while (_kbhit())
	{
		switch (_getch())
		{
		case CharDir::UP:     if (map[snakes[0].posY - 1][snakes[0].posX]     != Tile::SNAKE) playerDir = CharDir::UP;    break;
		case CharDir::LEFT:   if (map[snakes[0].posY]    [snakes[0].posX - 1] != Tile::SNAKE) playerDir = CharDir::LEFT;  break;
		case CharDir::RIGHT:  if (map[snakes[0].posY]    [snakes[0].posX + 1] != Tile::SNAKE) playerDir = CharDir::RIGHT; break;
		case CharDir::DOWN:   if (map[snakes[0].posY + 1][snakes[0].posX]     != Tile::SNAKE) playerDir = CharDir::DOWN;  break;
		default: break;
		}

		isFirstTouch = true;
	}

	if (isFirstTouch)
	{
		oldPosX = snakes[0].posX;
		oldPosY = snakes[0].posY;

		// 플레이어 머리 이동 구문
		switch (playerDir)
		{
		case CharDir::UP:    --snakes[0].posY; break;
		case CharDir::LEFT:  --snakes[0].posX; break;
		case CharDir::RIGHT: ++snakes[0].posX; break;
		case CharDir::DOWN:  ++snakes[0].posY; break;
		default: break;
		}
		
		// 머리에 따른 꼬리들의 이동 구문
		for (int i = 1; i <= tailNum; ++i)
		{
			int tempPosX, tempPosY;

			tempPosX = snakes[i].posX;
			tempPosY = snakes[i].posY;

			snakes[i].posX = oldPosX;
			snakes[i].posY = oldPosY;

			oldPosX = tempPosX;
			oldPosY = tempPosY;
		}

		// 벽과 몸통 충돌 구문
		if (map[snakes[0].posY][snakes[0].posX] == Tile::SNAKE || map[snakes[0].posY][snakes[0].posX] == Tile::WALL)
		{
			isRun = false;
			return;
		}

		// 열매 충돌 구문
		if (snakes[0].posX == fruit.posX && snakes[0].posY == fruit.posY)
		{
			Snake tail;

			if (tailNum == 0)
			{
				switch (playerDir)
				{
				case CharDir::UP:
					tail.posX = snakes[tailNum].posX;
					tail.posY = snakes[tailNum].posY + 1;
					break;
				case CharDir::LEFT:
					tail.posX = snakes[tailNum].posX + 1;
					tail.posY = snakes[tailNum].posY;
					break;
				case CharDir::RIGHT:
					tail.posX = snakes[tailNum].posX - 1;
					tail.posY = snakes[tailNum].posY;
					break;
				case CharDir::DOWN:
					tail.posX = snakes[tailNum].posX;
					tail.posY = snakes[tailNum].posY - 1;
					break;
				default: break;
				}

				isFirstEat = true;
			}
			else
			{
				tail.posX = snakes[tailNum].posX;
				tail.posY = snakes[tailNum].posY;
			}

			snakes.push_back(tail);
			++tailNum;
			isBeFruit = false;
		}

		map[oldPosY][oldPosX] = Tile::ROAD;

		map[snakes[0].posY][snakes[0].posX] = Tile::SNAKEHEAD;

		for (int i = 1; i <= tailNum; ++i)
			map[snakes[i].posY][snakes[i].posX] = Tile::SNAKE;
		

		if (!isBeFruit)
			createFruit();
	}
}
void render()
{
	system("cls");

	for (int i = 0; i < HEIGHT; ++i)
	{
		for (int j = 0; j < WIDTH; ++j)
		{
			switch (map[i][j])
			{
			case Tile::WALL:  cout << "# "; break;
			case Tile::ROAD:  cout << ". "; break;
			case Tile::FRUIT: cout << "F "; break;
			case Tile::SNAKE: cout << "O "; break;
			case Tile::SNAKEHEAD: cout << "@ "; break;
			default: break;
			}
		}
		cout << endl;
	}
	cout << "먹은 열매의 개수: " << tailNum << "개";
}
void init()
{
	srand((unsigned)time(NULL));

	for (int i = 0; i < HEIGHT; ++i)
		for (int j = 0; j < WIDTH; ++j)
			map[i][j] = Tile::ROAD;

	for (int i = 0; i < HEIGHT; ++i)
		map[i][WIDTH - 1] = map[i][0] = Tile::WALL;
	for (int i = 1; i < WIDTH - 1; ++i)
		map[HEIGHT - 1][i] = map[0][i] = Tile::WALL;

	snakes.clear();

	Snake head(1, 1);
	map[head.posY][head.posX] = Tile::SNAKEHEAD;
	snakes.push_back(head);

	isRun = true;
	tailNum = 0;
	playerDir = CharDir::NONE;

	createFruit();
	render();
}

int inGame()
{
	init();
	while (true)
	{
		update();

		if (!isRun) break;

		render();
		Sleep(100);
	}

	cout << endl << "GAME OVER" << endl;
	cout << "재시작 하려면 아무 키나 누르세요";

	if (_getch())
		return 1;
}

int main()
{
	int n;
	while (true)
	{
		if (n = inGame())
		{
			system("cls");
			continue;
		}
		else 
			break;
	}

	return 0;
}