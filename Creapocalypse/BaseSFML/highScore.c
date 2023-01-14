#include "highScore.h"
#include "init.h"

void DisplayScore(GameData* _gd)
{
	char text[256];
	char timeText[20];
	int size = 30;
	if (_gd->isTyping)
	{
		
		//Display name typing
		sprintf(text, "Entrez votre pseudo \n%s|", _gd->stat.name);
		sfText_setString(_gd->text, text);
		BlitText(_gd->text, WINDOW_WIDTH/2 - 5 * size, WINDOW_HEIGHT/2 - size, 30, _gd->window);
	}
	else
	{
		//Display Higscore
		int nb = _gd->nbhighScores;
		nb = nb > 20 ? 20 : nb;

		for (int i = 0; i < nb; i++)
		{
			sprintf(text, "- %s", _gd->highScores[i].name);
			sfText_setString(_gd->text, text);
			BlitText(_gd->text, WINDOW_WIDTH / 2 - 13 * size, 50 + i * size, size, _gd->window);

			strftime(timeText, 20, "%D %R", localtime(&_gd->highScores[i].time));
			sprintf(text, ": %3d kill,\t %d$,\t %d turrets,\t%s", _gd->highScores[i].kill, _gd->highScores[i].money, _gd->highScores[i].towers, timeText);
			sfText_setString(_gd->text, text);
			BlitText(_gd->text, WINDOW_WIDTH / 2 - 5 * size, 50 + i * size, size, _gd->window);
		}
	}
}

//Inputs for the name entrance
void GetNameInputs(sfEvent* _event, GameData* _gd)
{
	if (_event->type == sfEvtTextEntered)
	{
		switch (_event->text.unicode)
		{
		case 8 : //BackSpace
			if (strlen(_gd->stat.name) > 0)
			{
				_gd->stat.name[strlen(_gd->stat.name) - 1] = 0;
			}
			break;
		case 13: //Enter
			if (strlen(_gd->stat.name) > 0)
			{
				_gd->isTyping = 0;
				SaveHighscores(_gd);
				GetHighscores(_gd);
			}
		default:
			if (strlen(_gd->stat.name) < NAME_MAX_LEN - 1)
			{
				strcat(_gd->stat.name, &_event->text.unicode);
			}
			break;
		}
		
	}
}

//Inputs while highsccrore is displayed
void GetHighscoreInputs(GameData* _gd)
{
	sfEvent event;
	while (sfRenderWindow_pollEvent(_gd->window, &event))
	{
		if (event.type == sfEvtClosed || (event.type == sfEvtKeyPressed && event.key.code == sfKeyEscape))
		{
			exit(0);
		}

		if (_gd->isTyping)
		{
			GetNameInputs(&event, _gd);
		}
		else
		{
			if (event.type == sfEvtKeyPressed)
			{
				//Space : Restart the game
				if (event.key.code == sfKeySpace)
				{
					UnloadGameData(_gd);
					LoadGameData(_gd);
				}
			}
		}
	}
}

void SaveHighscores(GameData* _gd)
{
	FILE* file = fopen("../HighScores.bin", "r+");
	int nb = 0;

	if (!file)
	{
		printf("Error while reading \"HighScores.bin\"");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	fread(&nb, sizeof(int), 1, file);
	rewind(file);
	nb++;
	
	_gd->stat.time = time(NULL);

	fwrite(&nb, sizeof(int), 1, file);
	fseek(file, 0, SEEK_END);
	fwrite(&_gd->stat, sizeof(Statistics), 1, file);

	fclose(file);
}

int SortStats(const void* _a, const void* _b)
{
	if (((Statistics*)_a)->kill >  ((Statistics*)_b)->kill) return -1;
	if (((Statistics*)_a)->kill == ((Statistics*)_b)->kill) return 0;
	if (((Statistics*)_a)->kill <  ((Statistics*)_b)->kill) return 1;
}


void GetHighscores(GameData* _gd)
{
	FILE* file = fopen("../HighScores.bin", "rb");
	Statistics* scores;
	int nb;

	if (!file)
	{
		printf("Error while reading \"HighScores.bin\"");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	fread(&nb, sizeof(int), 1, file);
	scores = malloc(sizeof(Statistics) * nb);
	if (!scores)
	{
		printf("Error while allocating \"scores\"");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	fread(scores, sizeof(Statistics), nb, file);

	qsort(scores, nb, sizeof(Statistics), SortStats);
	
	_gd->highScores = scores;
	_gd->nbhighScores = nb;
	fclose(file);
}