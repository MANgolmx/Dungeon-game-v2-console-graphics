#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <time.h>

HANDLE con = 0;

#pragma region COLORS

#define BLACK       0
#define DARKBLUE    1
#define DARKGREEN   2
#define LIGHTBLUE   3
#define RED         4
#define VIOLET      5
#define GOLD        6
#define LIGHTGRAY   7
#define GRAY        8
#define BLUE        9
#define GREEN      10
#define LAZURE     11
#define LIGHTRED   12
#define PINK       13
#define YELLOW     14
#define WHITE      15

#pragma endregion

void selectColor(int back, int fore)
{
	if (con == 0) con = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(con, back * 16 + fore);
}

#define EMPTY1     ' '
#define EMPTY2     '.'
#define EMPTY3     '~'
#define WALL	   'W'
#define EXIT	   'E'
#define DOOR	   'D'
#define KEY		   'k'
#define PLAYER     '@'
#define COIN       'c'
#define TRAP       'T'
#define SHOP       '$'
#define CASINO     '7'
#define ENEMY      'X'

#define ESC        27
#define UP		   72
#define DOWN       80
#define RIGHT      75
#define LEFT       77

char map[100][101];
bool isRunning = true;
bool cheats = false;
int end = 0;
int w, h;
int wt = 0, ht = 0;
int countTraps = 0;

enum AIType {Player, AI_Random, AI_RightHand, AI_StraightChaser, AI_Rat, AI_Chaser};

enum Directions {D_UP, D_RIGHT, D_DOWN, D_LEFT, D_STAY};

struct Character
{
	char type[30] = "Stranger";
	int x = 0, y = 0, lives = 3, coins = 0, keys = 0, min_dmg = 1, max_dmg = 1;
	AIType ai;
	Directions dir;
};

Character player = {"Player", 0, 0, 3, 0, 0, 1, 3, Player};

Character enemies[20];
int enemyCount = 0;

enum Words {Keys = 0, Coins = 1, Health = 2};

int getRandom(int a, int b)
{
	return rand() % (b - a + 1) + a;
}

Character createEnemy(int x, int y, const char* type)
{
	Character enemy;
	if (strcmp(type, "Goblin") == 0)
		enemy = { "Goblin", x, y, 3, getRandom(0, 2), 0, 1, 2, AI_StraightChaser, D_STAY };
	else	if (strcmp(type, "Prisoner") == 0)
		enemy = { "Prisoner", x, y, 1, 0, 0, 0, 3, AI_Random, D_STAY };
	else	if (strcmp(type, "Guard") == 0)
		enemy = { "Guard", x, y, 5, getRandom(2, 8), 999, 0, 9, AI_RightHand, D_UP };
	else	if (strcmp(type, "Swordsman") == 0)
		enemy = { "Swordsman", x, y, 4, getRandom(0, 5), 0, 1, 3, AI_RightHand, D_UP };
	else    if (strcmp(type, "Magician") == 0)
		enemy = { "Magician", x, y, 2, getRandom(2, 6), 0, 1, 6, AI_Chaser, D_STAY };
	else	if (strcmp(type, "Ork") == 0)
		enemy = { "Ork", x, y, 7, getRandom(0, 2), 0, 0, 1, AI_Random, D_STAY };
	else    if (strcmp(type, "Rat") == 0)
		enemy = { "Rat", x, y, 1, getRandom(0, 9), 999, 0, 2, AI_Rat, D_STAY };
		else
		{ enemy.x = x; enemy.y = y; enemy.ai = AI_Random; }
	return enemy;
}

char wordsForm[3][3][30] = {
	{"ключ","ключа","ключей"},
	{"монета","монеты","монет"},
	{"жизнь","жизни","жизней"},
};

void printNumber(int n, Words w)
{
	switch (n % 10)
	{
	case 1: printf("%i %s", n, wordsForm[w][0]); break;
	case 2: case 3:
	case 4: printf("%i %s", n, wordsForm[w][1]); break;
	case 5: case 6: case 7: case 8: case 9:
	case 0: printf("%i %s", n, wordsForm[w][2]); break;
	}
}

int loadLevel(const char filename[])
{
	FILE* f;
	if (fopen_s(&f, filename, "rt") != 0)
	{
		printf("Файл с уровнем не был открыт!\n");
		system("pause");
		return 1;
	}

	char trash_str[50];
	fscanf_s(f, "%i%i", &w, &h);
	fscanf_s(f, "%i%i", &player.x, &player.y);

	fgets(trash_str, 50, f);

	for (int i = 0; i < h; i++)
		fgets(map[i], 100, f);

	fscanf_s(f, "%i", &enemyCount);

	int x = 0, y = 0;
	char etype[30];
	for (int i = 0; i < enemyCount; i++)
	{
		fscanf_s(f, "%i%i %s", &x, &y, etype, 30);
		enemies[i] = createEnemy(x, y, etype);
	}

	fclose (f);
	return 0;
}

void printLevel()	
{
	printf("\n");

	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			if (i == player.y && j == player.x)
			{
				selectColor(YELLOW, BLUE);
				printf("%c", PLAYER);
			}
			else
			{
				bool isEnemy = false;
				for (int k = 0; k < enemyCount; k++)
				{
					if (i == enemies[k].y && j == enemies[k].x && enemies[k].lives > 0)
					{
						selectColor(YELLOW, RED);
						switch (enemies[k].dir)
						{
						case D_UP: printf("^"); break;
						case D_RIGHT: printf(">"); break;
						case D_DOWN: printf("v"); break;
						case D_LEFT: printf("<"); break;
						case D_STAY: printf("X"); break;
						}
						isEnemy = true;
						break;
					}
				}
				if (!isEnemy)
				{
					switch (map[i][j])
					{
					case EMPTY1:
						selectColor(YELLOW, YELLOW);
						printf("%c", EMPTY1);
						break;
					case WALL:
						selectColor(GRAY, GRAY);
						printf("%c", map[i][j]);
						break;
					case KEY:
						selectColor(YELLOW, LIGHTRED);
						printf("%c", KEY);
						break;
					case DOOR:
						selectColor(GRAY, LIGHTGRAY);
						printf("%c", DOOR);
						break;
					case SHOP:
						selectColor(YELLOW, DARKGREEN);
						printf("%c", SHOP);
						break;
					case EXIT:
						selectColor(YELLOW, DARKBLUE);
						printf("%c", EXIT);
						break;
					case COIN:
						selectColor(YELLOW, LIGHTRED);
						printf("%c", COIN);
						break;
					case TRAP:
						selectColor(YELLOW, YELLOW);
						printf("%c", TRAP);
						break;
					case CASINO:
						selectColor(YELLOW, DARKGREEN);
						printf("%c", map[i][j]);
						break;
					default:
						selectColor(YELLOW, DARKBLUE);
						printf("%c", map[i][j]);
						break;
					}
				}
			}
		}
		selectColor(BLACK, WHITE);
		printf("\n");
	}
	selectColor(BLACK, WHITE);
	printf("\n\n");
}

bool isPassable(Character p, int x, int y)
{
	if (x >= w || y >= h || x < 0 || y < 0)
		return false;
	if (map[y][x] == WALL)
		return false;
	if (map[y][x] == DOOR && p.keys <= 0)
		return false;
	return true;
}

void casino(Character& p)
{
	int k = 0;
	int n = 0, m = 0;
	int r1, r2, r3, r4;
	int s1 = 0, s2 = 0, s3 = 0, s4 = 0;
	system("cls");
	printf("Добро пожаловать! Если у вас нет денег можете сразу проваливать!\n");
	printf("Ну что сыграем? Как обычно три режима игры. Рассказать или уже знаешь? (1 - знаю, 0 - рассказывай) ");
	scanf_s("%i", &k);

	if (k == 0)
	{
		printf("\nНу значит слушай:\n");
		Sleep(300);
		printf("Первый режим игры: два человека бросают по два кубика, соответственно у кого больше, тот и выиграл\n");
		printf("Второй режим: ты выбираешь число и кидаешь кубики, выпадает то что ты загадал - ты выиграл!\n");
		printf("И третий вариант: у нас тут недавно построили стадион для скачек, ты ставишь на одну из лошадей, если она выигрывает, то и ты поднял бабла!\n");
		printf("Ну так что выбираешь?\n");
	}
	else
		printf("Ну тогда что выбираешь?\n");

	scanf_s("%i", &k);

	switch (k)
	{
	case 1:
		printf("Говори ставку! ");
		scanf_s("%i", &n);
		if (n > p.coins)
		{
			printf("\nВали отсюда, умник!\n");
			system("pause");
			return;
		}
		if (n <= 0)
		{
			printf("Я кидаю!\n");
			printf("У меня 1 и 1\n");
			printf("Теперь ты!\n");
			printf("НАЖМИТЕ ЛЮБУЮ КЛАВИШУ!\n");
			system("pause > null");
			printf("У тебя 6 и 6!\n");
			printf("Хаха, ты выиграл!\n");
			p.coins += n;
			system("pause");
			break;
		}
		printf("\nХорошо... ");
		Sleep(500);
		printf("Я кидаю!\n");
		r1 = getRandom(1, 6);
		r2 = getRandom(1, 6);
		s1 = r1 + r2;
		Sleep(777);
		printf("У меня %i и %i\n", r1, r2);
		printf("Теперь ты!\n");
		printf("НАЖМИТЕ ЛЮБУЮ КЛАВИШУ!\n");
		system("pause > null");
		r1 = getRandom(1, 6);
		r2 = getRandom(1, 6);
		s2 = r1 + r2;
		Sleep(777);
		printf("У тебя %i и %i!\n", r1, r2);
		if (s1 > s2)
		{
			printf("Хаха, я выиграл!\n");
			p.coins -= n;
		}
		if (s2 > s1)
		{
			printf("Ты выиграл... ");
			Sleep(333);
			printf("Я давно не проигрывал!\n");
			p.coins += n;
		}
		if (s2 == s1)
		{
			printf("Нуу... ");
			printf("Я не знаю... ");
			printf("Ладно просто закончим\n");
		}
		system("pause");
		break;
	case 2:
		printf("Говори ставку! ");
		scanf_s("%i", &n);
		if (n > p.coins)
		{
			printf("\nВали отсюда, умник!\n");
			system("pause");
			return;
		}
		if (n <= 0)
		{
			printf("Ладно, так уж и быть, загадывай число! ");
			scanf_s("%i", &s2);
			printf("Теперь если тебе выпадет в сумме это число, то ты выиграл!\n");
			printf("Ну кидай!\n");
			printf("НАЖМИТЕ ЛЮБУЮ КЛАВИШУ!\n");
			system("pause > null");
			printf("Тебе выпало %i и 1\n", s2 - 1);
			printf("Ты выиграл!\n");
			p.coins += n;
			system("pause");
			break;
		}
		printf("Загадывай число! ");
		scanf_s("%i", &s2);
		if (s2 > 12 || s2 < 0)
		{
			printf("Ты же понимаешь что ты только что сказал... ");
			Sleep(555);
			printf("Ну мне то всё равно. Это же твои деньги!");
		}
		printf("\nТеперь если тебе выпадет в сумме это число, то ты выиграл! ");
		if (s2 > 12 || s2 < 0)
			printf("(идиот!)");
		printf("\nНу кидай!\n");
		printf("НАЖМИТЕ ЛЮБУЮ КЛАВИШУ!\n");
		system("pause > null");
		r1 = getRandom(1, 6);
		r2 = getRandom(1, 6);
		s1 = r1 + r2;
		Sleep(777);
		printf("У тебя %i и %i!\n", r1, r2);
		if (s2 == s1)
		{
			printf("Как ты это сделал?!?\n");
			Sleep(333);
			printf("Вот твои деньги");
			p.coins += n * 5;
		}
		else
		{
			printf("Ты проиграл!\n");
			p.coins -= n;
		}
		system("pause");
		break;
	case 3:
		printf("Добро пожаловать на скачки!");
		printf("Сегодня участвуют 4 коня под номерами: 96, 55, 70, 13\n");
		printf("На кого ставите? ");
		scanf_s("%i", &m);
		if (m == 96)
			printf("Ваша лошадь первая! ");
		else
			if (m == 55)
				printf("Ваша лошадь вторая! ");
			else
				if (m == 70)
					printf("Ваша лошадь третья! ");
				else
					if (m == 13)
						printf("Ваша лошадь четвертая! ");
					else
					{
						printf("\nВали отсюда, умник!\n");
						system("pause");
						return;
					}
		printf("А сумма? ");
		scanf_s("%i", &n);
		if (n > p.coins)
		{
			printf("\nВали отсюда, умник!\n");
			system("pause");
			return;
		}
		if (n < 0)
		{
			printf("\nВали отсюда, умник!\n");
			system("pause");
			return;
		}
		printf("Мммм... ");
		printf("Ладно, скачки скоро начнуться! Немного подождите!\n");
		Sleep(3000);
		while (s1 < 25 || s2 < 25 || s3 < 25 || s4 < 25)
		{
			system("cls");
			r1 = getRandom(0, 3); s1 += r1;
			r2 = getRandom(0, 3); s2 += r2;
			r3 = getRandom(0, 3); s3 += r3;
			r4 = getRandom(0, 3); s4 += r4;
			for (int i = 0; i < s1; i++)
				printf(" ");
			printf("@\n");
			for (int i = 0; i < s2; i++)
				printf(" ");
			printf("@\n");
			for (int i = 0; i < s3; i++)
				printf(" ");
			printf("@\n");
			for (int i = 0; i < s4; i++)
				printf(" ");
			printf("@\n");
			Sleep(300);
		}
		if (m == 96 && (s1 > s2 && s1 > s3 && s1 > s4))
		{
			printf("Ты выиграл!\n");
			p.coins += n;
		}
		else
			if (m == 55 && (s2 > s1 && s2 > s3 && s2 > s4))
			{
				printf("Ты выиграл!\n");
				p.coins += n;
			}
			else
				if (m == 70 && (s3 > s2 && s3 > s1 && s3 > s4))
				{
					printf("Ты выиграл!\n");
					p.coins += n;
				}
				else
					if (m == 13 && (s4 > s2 && s4 > s3 && s4 > s1))
					{
						printf("Ты выиграл!\n");
						p.coins += n;
					}
					else
					{
						printf("Ты проиграл!\n");
						p.coins -= n;
					}
		break;
	case ESC:
		return;
		break;

	system("pause");
	break;
	}
}

void printInformation(Character p)
{
	printf("У вас "); printNumber(p.keys, Keys);

	printf("\nУ вас "); printNumber(p.coins, Coins);

	printf("\nУ вас "); printNumber(p.lives, Health);

	printf("\nВаш урон: %i - %i\n", p.min_dmg, p.max_dmg);

	if (map[p.y][p.x + 1] == TRAP || map[p.y][p.x - 1] == TRAP || map[p.y + 1][p.x] == TRAP || map[p.y - 1][p.x] == TRAP)
		printf("Вы чувствуете опасность. Будьте осторожны!");
	if (map[p.y][p.x] == TRAP)
		printf("Вы попали в ловушку!");

	printf("\n");
}

void printInformationEnemy(Character p)
{
	printf("%s", p.type);
	printf("\nУ него ");  printNumber(p.coins, Coins);
	printf("\nУ него "); printNumber(p.lives, Health);
	printf("\n");
}

void battle(Character& enemy)
{
	int at;
	end = 3;
	system("cls");
	printf("У вас "); printNumber(player.lives, Health);
	printf("\nТы встретил: ");
	printInformationEnemy(enemy);
	
	while (true)
	{
		printf("\nНажмите [ПРОБЕЛ] чтобы ударить!\n");
		system("pause > null");
		at = getRandom(player.min_dmg, player.max_dmg);
		enemy.lives -= at;
		if (enemy.lives <= 0)
		{
			printf("Вы убили врага и плучили "); printNumber(enemy.coins, Coins); printf("\n");
			player.coins += enemy.coins;
			system("pause");
			return;
		}
		if (at == 0)
			printf("Вы промазали!");
		printf("У врага "); printNumber(enemy.lives, Health);
		at = getRandom(enemy.min_dmg, enemy.max_dmg);
		player.lives -= at;
		if (at == 0)
			printf("Враг промахнулся!");
		else
			printf("\nВраг ударил вас!\n");
		if (player.lives <= 0)
		{
			printf("\n");
			isRunning = false;
			return;
		}
		printf("У вас "); printNumber(player.lives, Health);
	}
}

void checkEnemies()
{
	for (int i = 0; i < enemyCount; i++)
	{
		if (player.x == enemies[i].x && player.y == enemies[i].y && enemies[i].lives > 0)
		{
			battle(enemies[i]);
		}
	}
}

void endGame()
{
	if (end == 0)
	{
		printf("Вы прошли игру!");
		if (cheats)
			printf(" (а теперь давай без читов!)");
	}
	if (end == 1)
	{
		printf("Вы умерли от ловушки!");
		if (cheats)
			printf(" (даже с читами умер!)");
	}
	if (end == 2)
		printf("Заходи еще!");
	if (end == 3)
	{
		system("cls");
		printf("Вы умерли от врага!");
		if (cheats)
			printf(" (даже с читами умер!)");
	}
	if (end == 9)
	{
		printf("ТЫ КУДА ПОЛЕЗ!?!?!\n");
		Sleep(400);
		printf("ТЕБЕ КОНЕЦ!\n");
		Sleep(200);
		printf("Вы были убиты продавцом!\n");
	}
	
	printf("\n");
}

void checkLives(Character p)
{
	if (p.lives <= 0)
		isRunning = false;
}

int saveGame(Character p)
{
	FILE* f;
	if (fopen_s(&f, "save.txt", "wt") != 0)
	{
		printf("Файл с сохранением не был записан!\n");
		system("pause");
		return 0;
	}

	fprintf(f, "%i %i %i %i %i", p.x, p.y, p.lives, p.coins, p.keys);

	fclose(f);
}

int loadGame(Character& p)
{
	FILE* f;
	if (fopen_s(&f, "save.txt", "rt") != 0)
	{
		printf("Файл с сохранением не был открыт!\n");
		system("pause");
		return 0;
	}

	fscanf_s(f, "%i%i%i%i%i", &p.x, &p.y, &p.lives, &p.coins, &p.keys);

	fclose(f);
}

void playerInput(Character& p)
{
	char trash;
	int choice;
	choice = _getch();

	if (choice == 224)
	{
		choice = _getch();
		switch (choice)
		{
		case UP: if (isPassable(player, player.x, player.y - 1)) player.y--; break;
		case DOWN: if (isPassable(player, player.x, player.y + 1)) player.y++; break;
		case RIGHT: if (isPassable(player, player.x - 1, player.y)) player.x--; break;
		case LEFT: if (isPassable(player, player.x + 1, player.y)) player.x++; break;
		}
	}
	else
		switch (char(choice))
		{
		case 'p': case 'P': saveGame(player); break;
		case 'l': case 'L': loadGame(player); break;
		case 'n': case 'N': if (cheats) cheats = false; else cheats = true; break;
		case 'k': case 'K': if (cheats) p.keys += 10; break;
		case 'm': case 'M': if (cheats) p.coins += 10; break;
		case 'o': case 'O': if (cheats) p.lives += 5; break;
		case ESC: end = 2; isRunning = false; break;
		}

}

Directions getNewDir(Directions oldDir, int turn)
{
	return Directions((oldDir + turn) % 4);
}

void getCoordsInDir(int oldX, int oldY, Directions dir, int& newX, int& newY)
{
	switch (dir)
	{
	case D_UP:    newX = oldX; newY = oldY - 1; break;
	case D_RIGHT: newX = oldX + 1; newY = oldY; break;
	case D_DOWN:  newX = oldX; newY = oldY + 1; break;
	case D_LEFT:  newX = oldX - 1; newY = oldY; break;
	}
}

void enemyAI_Random(Character& enemy)
{
	Directions direction = Directions(getRandom(0, 4));

	switch (direction)
	{
	case D_UP:
		if (isPassable(enemy, enemy.x, enemy.y - 1))
			enemy.y--;
		break;
	case D_RIGHT:
		if (isPassable(enemy, enemy.x + 1, enemy.y))
			enemy.x++;
		break;
	case D_DOWN:
		if (isPassable(enemy, enemy.x, enemy.y + 1))
			enemy.y++;
		break;
	case D_LEFT:
		if (isPassable(enemy, enemy.x - 1, enemy.y))
			enemy.x--;
		break;
	default:
		break;
	}
}

void enemyAI_RightHand(Character& enemy)
{
	int x, y;
	Directions check = getNewDir(enemy.dir, 1);
	getCoordsInDir(enemy.x, enemy.y, check, x, y);
	if (isPassable(enemy, x, y))
	{
		enemy.x = x;
		enemy.y = y;
		enemy.dir = check;
		return;
	}
	
	check = getNewDir(enemy.dir, 0);
	getCoordsInDir(enemy.x, enemy.y, check, x, y);
	if (isPassable(enemy, x, y))
	{
		enemy.x = x;
		enemy.y = y;
		enemy.dir = check;
		return;
	}

	check = getNewDir(enemy.dir, 3);
	getCoordsInDir(enemy.x, enemy.y, check, x, y);
	if (isPassable(enemy, x, y))
	{
		enemy.x = x;
		enemy.y = y;
		enemy.dir = check;
		return;
	}

	check = getNewDir(enemy.dir, 2);
	getCoordsInDir(enemy.x, enemy.y, check, x, y);
	if (isPassable(enemy, x, y))
	{
		enemy.x = x;
		enemy.y = y;
		enemy.dir = check;
		return;
	}

	enemy.dir = D_STAY;
}

void enemyAI_StraightChaser(Character& enemy)
{
	if (player.x - enemy.x <= 5 && player.x - enemy.x >= 0 && (isPassable(enemy, enemy.x + 1, enemy.y)))
		enemy.x++;
	else
	{
		if (player.x - enemy.x >= -5 && player.x - enemy.x <= 0 && (isPassable(enemy, enemy.x - 1, enemy.y)))
			enemy.x--;
		else
		{
			if (player.y - enemy.y >= -5 && player.y - enemy.y <= 0 && (isPassable(enemy, enemy.x, enemy.y - 1)))
				enemy.y--;
			else
			{
				if (player.y - enemy.y <= 5 && player.y - enemy.y >= 0 && (isPassable(enemy, enemy.x, enemy.y + 1)))
					enemy.y++;
				else
				{
					enemyAI_Random(enemy);
				}
			}
		}
	}
}

void enemyAI_Rat(Character& enemy)
{
	if (player.x - enemy.x <= 5 && player.x - enemy.x >= 0 && (isPassable(enemy, enemy.x - 1, enemy.y)))
		enemy.x--;
	else
	{
		if (player.x - enemy.x >= -5 && player.x - enemy.x <= 0 && (isPassable(enemy, enemy.x + 1, enemy.y)))
			enemy.x++;
		else
		{
			if (player.y - enemy.y >= -5 && player.y - enemy.y <= 0 && (isPassable(enemy, enemy.x, enemy.y + 1)))
				enemy.y++;
			else
			{
				if (player.y - enemy.y <= 5 && player.y - enemy.y >= 0 && (isPassable(enemy, enemy.x, enemy.y - 1)))
					enemy.y--;
				else
				{
					enemyAI_Random(enemy);
				}
			}
		}
	}
}

void enemyAI_Chaser(Character& enemy)
{
	if (player.x == enemy.x && player.y == enemy.y)
		return;

	int wavemap[100][100];
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
		{
			if (isPassable(enemy, j, i))
				wavemap[i][j] = 99999;
			else
				wavemap[i][j] = -1;
		}

	wavemap[player.y][player.x] = 0;

	for (int d = 0; d < 100; d++)
		for (int i = 0; i < h; i++)
			for (int j = 0; j < w; j++)
				if (wavemap[i][j] == d)
				{
					if (i == enemy.y && j == enemy.x) break;
					if (i - 1 >= 0 && wavemap[i - 1][j] > d + 1) wavemap[i - 1][j] = d + 1;
					if (i + 1 <  h && wavemap[i + 1][j] > d + 1) wavemap[i + 1][j] = d + 1;
					if (j - 1 >= 0 && wavemap[i][j - 1] > d + 1) wavemap[i][j - 1] = d + 1;
					if (j + 1 <  w && wavemap[i][j + 1] > d + 1) wavemap[i][j + 1] = d + 1;
				}

	if (wavemap[enemy.y][enemy.x] > 999)
	{
		enemyAI_Random(enemy);
		return;
	}

	       if (wavemap[enemy.y - 1][enemy.x] == wavemap[enemy.y][enemy.x] - 1) enemy.y--;
	else   if (wavemap[enemy.y][enemy.x + 1] == wavemap[enemy.y][enemy.x] - 1) enemy.x++;
	else   if (wavemap[enemy.y + 1][enemy.x] == wavemap[enemy.y][enemy.x] - 1) enemy.y++;
	else   if (wavemap[enemy.y][enemy.x - 1] == wavemap[enemy.y][enemy.x] - 1) enemy.x--;
	else      {enemyAI_Random(enemy);}
}

void enemyTurn()
{
	for (int i = 0; i < enemyCount; i++)
	{
		if (enemies[i].lives <= 0)
			continue;

		if (player.x == enemies[i].x && player.y == enemies[i].y - 1 || 
		player.x == enemies[i].x && player.y == enemies[i].y + 1 ||
		player.x == enemies[i].x - 1 && player.y == enemies[i].y ||
		player.x == enemies[i].x + 1 && player.y == enemies[i].y)
		{
			enemies[i].x = player.x;
			enemies[i].y = player.y;
			continue;
		}
		switch (enemies[i].ai)
		{
		case AI_Random:	enemyAI_Random(enemies[i]);	break;
		case AI_RightHand: enemyAI_RightHand(enemies[i]); break;
		case AI_StraightChaser: enemyAI_StraightChaser(enemies[i]); break;
		case AI_Rat: enemyAI_Rat(enemies[i]); break;
		case AI_Chaser: enemyAI_Chaser(enemies[i]); break;
		}
	}
}

void checkPrison(Character& p)
{
	switch (map[p.y][p.x])
	{
	case KEY:
		p.keys++;
		map[p.y][p.x] = EMPTY1;
		break;
	case DOOR:
		p.keys--;
		map[p.y][p.x] = EMPTY1;
		break;
	case COIN:
		p.coins++;
		map[p.y][p.x] = EMPTY1;
		break;
	case TRAP:
		end = 1;
		p.lives--;
		break;
	case EXIT:
		end = 0;
		isRunning = false;
		break;
	}
}

void prison()
{
	printf("Вы попали в тюрьму!\n");
	printf("Если получиться выбраться, вы прождолжите играть!\n");
	system("pause");
	player.coins = 0; player.lives = 1; player.keys = 0; player.min_dmg = 0; player.max_dmg = 2;
	loadLevel("levelPrison.txt");
	while (isRunning)
	{
		system("cls");
		checkLives(player);
		printInformation(player);
		printLevel();

		playerInput(player);
		enemyTurn();
		checkEnemies();
		checkPrison(player);
	}
	if (player.lives <= 0)
	{
		endGame();
		return;
	}
	isRunning = true;
	return;
}

void inShop()
{
	int countTraps2 = countTraps;
	bool staying = true;
	system("cls");
	if (player.coins <= 0)
	{
		printf("Вали отсюда");
		Sleep(800);
		printf(" и не возвращайся! ");
		Sleep(1000);
		printf("Никогда!");
		Sleep(2000);
		printf(" Или тогда когда будут деньги!");
		Sleep(3000);
		return;
	}

	printf("У вас "); printNumber(player.coins, Coins);
	printf("\nУ вас "); printNumber(player.lives, Health);
	printf("\nДобро пожаловать! Надеюсь у вас есть деньги!\n");
	printf("У нас огромный ассортимент: ");
	Sleep(1250);

	while (staying)
	{
		printf("\n1) Аптечка (+1 жизнь, -10 монет)\n");
		printf("2) Большая аптечка (+2 жизни, -22 монеты, 50%% шанс что добавятся еще 2 жизни)\n");
		printf("3) МечЬ  (Урон: 2 - 4, -8 монет)\n");
		printf("4) Сабля (Урон: 1 - 7, -15 монет)\n");
		printf("5) Топор (Урон: 5 - 9, -30 монет)\n");
		printf("6) Могу рассказать где находится 1 ловушка! ( -25 монет)");
		printf("\n0) Выход (-1 монета)\n\n");
		if (player.max_dmg > 5)
			printf("9) Ограбить магазин\n");

		int choice;
		scanf_s("%i", &choice);

		if (choice >= 0 && choice <= 9)
		{
			switch (choice)
			{
			case 1:
				if (player.coins >= 10)
				{
					printf("Спасибо за покупку!");
					player.coins -= 10;
					player.lives += 1;
				}
				else
					printf("У тебя недостаточно монет! Ты что хотел меня обмануть?\n");
				break;
			case 2:
				if (player.coins >= 22)
				{
					printf("Спасибо за покупку!");
					player.coins -= 25;
					player.lives += 2;
					if (getRandom(0, 1) == 0)
						player.lives += 2;
				}
				else
					printf("У тебя недостаточно монет! Ты что хотел меня обмануть?\n");
				break;
			case 3:
				if (player.coins >= 8)
				{
					printf("Спасибо за покупку!");
					player.coins -= 8;
					player.min_dmg = 2;
					player.max_dmg = 4;
				}
				else
					printf("У тебя недостаточно монет! Ты что хотел меня обмануть?\n");
				break;
			case 4:
				if (player.coins >= 15)
				{
					printf("Спасибо за покупку!");
					player.coins -= 15;
					player.min_dmg = 1;
					player.max_dmg = 7;
				}
				else
					printf("У тебя недостаточно монет! Ты что хотел меня обмануть?\n");
				break;
			case 5:
				if (player.coins >= 30)
				{
					printf("Спасибо за покупку!");
					player.coins -= 30;
					player.min_dmg = 5;
					player.max_dmg = 9;
				}
				else
					printf("У тебя недостаточно монет! Ты что хотел меня обмануть?\n");
				break;
			case 6:
				if (player.coins <= 25)
				{
					printf("У тебя недостаточно монет! Ты что хотел меня обмануть?\n");
					return;
				}
				for (ht = 0; ht < h; ht++)
					for (wt = 0; wt < w; wt++)
					{
						if (map[ht][wt] == TRAP)
						{
							if (countTraps2 == 0)
							{
								printf("Координаты ловушки: %i %i\n", ht, wt);
								countTraps++;
								system("pause");
								return;
							}
							if (countTraps2 > 0)
								countTraps2--;
						}
						if (ht == h - 1 && wt == w - 1)
							printf("Больше ловушек нет!");
					}
				break;
			case 0:
				printf("Ну и вали!");
				staying = false;
				break;
			case 9:
				if (player.max_dmg > 7)
				{
					printf("Я вызываю полицию!\n");
					prison();
					return;
				}

			}
		}
		else
		{
			printf("Нормально разговаривай!\n");
		}
		Sleep(750);
	}
}

void checkPosition(Character& p)
{
	switch (map[p.y][p.x])
	{
	case KEY:
		p.keys++;
		map[p.y][p.x] = EMPTY1;
		break;
	case DOOR:
		p.keys--;
		map[p.y][p.x] = EMPTY1;
		break;
	case COIN:
		p.coins++;
		map[p.y][p.x] = EMPTY1;
		break;
	case TRAP:
		end = 1;
		p.lives--;
		break;
	case SHOP:
		inShop();
		break;
	case EXIT:
		end = 0;
		isRunning = false;
		break;
	case CASINO:
		casino(player);
		break;
	}
}

void start()
{
	char k;
	printf("Вы хотите пройти обучение? (1 - да, 0 - нет) ");
	scanf_s("%c", &k);

	if (k == '0')
		return;

	loadLevel("level0.txt");

	printf("Чтобы ходить используй стрелочки\n");
	printf("Буквой k обозначен ключ, с помощью него ты сможешь открыть дверь, которая выглядит как буква D\n");
	printf("Видишь букву с? Это монетка, с помощье нее ты сможешь купить вещи в магазине под символом $\n");
	printf("Если дойдешь до буквы E, то ты выиграл!\n");
	printf("Будь осторожен на уровне есть ловушки! Если ты наступишь на одну из них, то у тебя отнимутся жизни!\n");
	printf("Враги обозначены буквой x, если столкнешься с ним, то начнется битва\n");
	printf("Удачи!\n");
	system("pause");

	while (isRunning)
	{
		system("cls");
		checkLives(player);
		printInformation(player);
		printLevel();

		playerInput(player);
		checkEnemies();
		checkPosition(player);
	}
	isRunning = true;
	player.lives = 3; player.coins = 0; player.keys = 0;
}

int main()
{
	srand(time(0));
	system("chcp 1251");
	start();
	loadLevel("level1.txt");

	while (isRunning)
	{
		system("cls");
		checkLives(player);
		printInformation(player);
		printLevel();

		playerInput(player);
		enemyTurn();
		checkEnemies();
		checkPosition(player);
	}

	endGame();

	system("pause");
	return 0;
}