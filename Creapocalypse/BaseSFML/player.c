#include "player.h"
#include "camera.h"
#include "myMath.h"
#include "core.h"
#include "tower.h"

//The player got a list of actions and when one is over, the next is executed.
//The player pathfiding is compute with an A* algorithm. 

void UpdatePlayer(GameData* _gd)
{
	if (_gd->player.actions)
	{
		//Get Vector
		sfVector2f playerPos = { _gd->player.x, _gd->player.y };
		sfVector2f nextMove = GetVector(playerPos, _gd->player.actions->pos);
		//if longer than 1 : normalize it and move;
		if (fabs(nextMove.x) > 0.1 || fabs(nextMove.y) > 0.1)
		{
			nextMove = NormalizeVector(nextMove);

			_gd->player.x += nextMove.x * SPEED * _gd->dt;
			_gd->player.y += nextMove.y * SPEED * _gd->dt;
			if (!IsBlockWalkable(_gd->player.x, _gd->player.y, sfTrue, 0, _gd))
			{
				_gd->player.x -= nextMove.x * SPEED * _gd->dt;
				_gd->player.y -= nextMove.y * SPEED * _gd->dt;
			}
		}
		//if at position, execute action
		else
		{
			if (_gd->player.actions->function != NULL)
			{
				_gd->player.actions->function(_gd->player.actions->data, _gd); //Build / Move / ...
			}
			FinishPlayerAction(&_gd->player);
		}
	}
}

//Free a player action
void FreeAction(Action* _action)
{
	free(_action);
}

//Add action to the end of the action list
void AddPlayerAction(Player* _player, Action* _action)
{
	if (_player->actions == NULL)
	{
		_player->actions = _action;
		return;
	}

	Action* last = _player->actions;
	while (last->next != NULL)
	{
		last = last->next;
	}
	last->next = _action;
}

//Free the current action and start the next
void FinishPlayerAction(Player* _player)
{
	Action* action = _player->actions;
	_player->actions = action->next;
	FreeAction(action);
}

//Free all thes playres actions
void ClearPlayerActions(Player* _player, GameData* _gd)
{
	Action* action = _player->actions;
	Action* next;

	while (action)
	{
		next = action->next;

		if (action->type == BUILD)
		{
			BuildData* data = (BuildData*)action->data;
			_gd->money += GetTowerPrice(data->type);
			SetBlock(data->BuildPos.x, data->BuildPos.y, GRASS, NULL, _gd);
		}

		FreeAction(action);
		action = next;
	}

	_player->actions = NULL;
}

//Create a new action :
Action* CreateMoveAction(sfVector2f _pos, GameData* _gd)
{
	Action* newAction = malloc(sizeof(Action));
	newAction->type = MOVE;
	newAction->pos = _pos;

	newAction->data = NULL;
	newAction->function = NULL;

	newAction->next = NULL;
	return newAction;
}
Action* CreateBuildAction(sfVector2f _pos, int _type, GameData* _gd)
{
	Action* newAction = malloc(sizeof(Action));
	newAction->type = BUILD;
	newAction->function = &Build;
	BuildData* data = malloc(sizeof(BuildData));
	data->BuildPos = _pos;
	data->type = _type;
	
	if (IsBlockWalkable(data->BuildPos.x - 1, data->BuildPos.y, 0, 0, _gd))
	{
		newAction->pos.x = (int)data->BuildPos.x - 0.5f;
		newAction->pos.y = (int)data->BuildPos.y + 0.5f;
	}
	else if (IsBlockWalkable(data->BuildPos.x + 1, data->BuildPos.y, 0, 0, _gd))
	{
		newAction->pos.x = (int)data->BuildPos.x + 1.5f;
		newAction->pos.y = (int)data->BuildPos.y + 0.5f;
	}
	else if (IsBlockWalkable(data->BuildPos.x, data->BuildPos.y - 1, 0, 0, _gd))
	{
		newAction->pos.x = (int)data->BuildPos.x + 0.5f;
		newAction->pos.y = (int)data->BuildPos.y - 0.5f;
	}
	else if (IsBlockWalkable(data->BuildPos.x, data->BuildPos.y + 1, 0, 0, _gd))
	{
		newAction->pos.x = (int)data->BuildPos.x + 0.5f;
		newAction->pos.y = (int)data->BuildPos.y + 1.5f;
	}
	else
	{
		return NULL;
	}

	SetBlockPreview((int)_pos.x, (int)_pos.y, data->type, _gd);
	newAction->data = data;
	newAction->next = NULL;
	_gd->money -= GetTowerPrice(data->type);
	return newAction;
}

//Actions 
void Build(void* _voidData, GameData* _gd)
{
	BuildData* data = ((BuildData*)_voidData);
	_gd->stat.towers += 1;
	SetBlock(data->BuildPos.x, data->BuildPos.y, data->type, NULL, _gd);
}

//A* pathfinding
enum state
{
	NONE,
	OPEN,
	CLOSED
};
struct Node
{
	int x;
	int y;
	
	int g; //from start
	int h; //from end
	int f; //g + h

	struct Node* next;
	struct Node* parent;
};
void RemoveNodeFromList(struct Node* _node, struct Node** _list)
{
	if (*_list == _node)
	{
		(*_list) = _node->next;
		return;
	}

	struct Node* current = *_list;
	while (current->next)
	{
		if (current->next == _node)
		{
			current->next = _node->next;
			return;
		}
		current = current->next;
	}
}
void AddNodeToList(struct Node* _node, struct Node** _list)
{
	_node->next = *_list;
	*_list = _node;
}
struct Node* GetNodeNeighbors(struct Node* _node, GameData* _gd)
{
	struct Node* list = NULL;
	struct Node* newNode;
	for (char x = -1; x <= 1; x++)
	{
		for (char y = -1; y <= 1; y++)
		{
			if (!(x == 0 && y == 0))
			{
				newNode = malloc(sizeof(struct Node));
				newNode->x = _node->x + x;
				newNode->y = _node->y + y;

				if (newNode->x >= 0 && newNode->y >= 0 && newNode->x < MAP_WIDTH && newNode->y < MAP_HEIGHT)
				{	
					if (IsBlockWalkable(_node->x + x, _node->y, 0, 0, _gd)
						&& IsBlockWalkable(_node->x, _node->y + y, 0, 0, _gd)
						&& IsBlockWalkable(_node->x + x, _node->y + y, 0, 0, _gd))
					{
						AddNodeToList(newNode, &list);
					}
				}
			}
		}
	}
	return list;
}
struct Node* GetNodeInList(struct Node* _node, struct Node** _list)
{
	struct Node* current = *_list;
	while (current)
	{
		if (current->x == _node->x && current->y == _node->y)
		{
			return current;
		}
		current = current->next;
	}
	return NULL;
}
int GetNodeDistance(struct Node* _nodeA, struct Node* _nodeB)
{
	//return an aproximation of 10* the distance
	int dstX = _nodeA->x - _nodeB->x;
	int dstY = _nodeA->y - _nodeB->y;
	dstX = dstX < 0 ? -dstX : dstX;
	dstY = dstY < 0 ? -dstY : dstY;

	if (dstX > dstY)
	{
		return 14 * dstY + 10 * (dstX - dstY);
	}
	else
	{
		return 14 * dstX + 10 * (dstY - dstX);
	}
}
void RetracePath(struct Node* _start, struct Node* _end, GameData* _gd)
{
	struct Node* current = _end->next;
	while (current)
	{
		sfVector2f pos = { current->x + 0.5f , current->y + 0.5f};
		Action* newAction = CreateMoveAction(pos, _gd);
		AddPlayerAction(&_gd->player, newAction);
		
		current = current->parent;
	}
}
int A_Star(sfVector2f _start, sfVector2f _end, GameData* _gd)
{
	//Initialize 2 sets : open and close, they are a list of node.
	struct Node* openSet = NULL;
	struct Node* closeSet = NULL;

	//Initialize a node for start and end.
	struct Node* startNode = malloc(sizeof(struct Node));
	struct Node* targetNode = malloc(sizeof(struct Node));

	//Initialze i : the number of iteration.
	int i = 0;

	//Set the values for the start and end nodes
	startNode->next = NULL;
	startNode->parent = NULL;
	startNode->x = _start.x;
	startNode->y = _start.y;
	
	targetNode->next = NULL;
	targetNode->parent = NULL;
	targetNode->x = _end.x;
	targetNode->y = _end.y;

	//A* work with : 
	//g-cost (distance from start node)
	//h-cost (distance from end node)
	//f-cost (g-cost + h-cost)

	startNode->g = 0;
	startNode->h = GetNodeDistance(startNode, targetNode);
	startNode->f = startNode->h;

	targetNode->g = startNode->h;
	targetNode->h = 0;
	targetNode->f = startNode->f;
	
	//Add the start node to the openSet
	AddNodeToList(startNode, &openSet);

	//while there is nodes in openSet (and restrict it to 2000 iteration to avoid 30s search for searching in all the 250x250 map.)
	while (openSet != NULL && i < 2000) 
	{
		//set CurrentNode on the lower f-cost in openSet
		struct Node* currentNode = openSet;
		struct Node* nextNode = openSet->next;
		struct Node* neighbour;

		while (nextNode)
		{
			if (nextNode->f < currentNode->f || (nextNode->f == currentNode->f && nextNode->h < currentNode->h))
			{
				currentNode = nextNode;
			}
			nextNode = nextNode->next;
		}

		//Move node from open to closed
		RemoveNodeFromList(currentNode, &openSet);
		AddNodeToList(currentNode, &closeSet);

		//Check if find our path (current node coordinates are same as end node)
		if (currentNode->x == targetNode->x && currentNode->y == targetNode->y)
		{
			RetracePath(startNode, currentNode, _gd);

			currentNode = openSet;
			while (currentNode)
			{
				
				RemoveNodeFromList(currentNode, &openSet);
				free(currentNode);
				currentNode = openSet;
			}

			currentNode = closeSet;
			while (currentNode)
			{

				RemoveNodeFromList(currentNode, &closeSet);
				free(currentNode);
				currentNode = closeSet;
			}

			return 1;
		}

		//Check neighbors
		neighbour = GetNodeNeighbors(currentNode, _gd);
		while (neighbour)
		{
			struct Node* nextNode = neighbour->next;
			//if the neighbor isn't allready in the closeSet
			if (!GetNodeInList(neighbour, &closeSet))
			{
				//check if the node is allready in the openSet
				int newMovementCost = currentNode->g + GetNodeDistance(currentNode, neighbour);
				struct Node* existing = GetNodeInList(neighbour, &openSet);

				//if it isn't, add it
				if (!existing)
				{
					neighbour->g = newMovementCost;
					neighbour->h = GetNodeDistance(neighbour, targetNode);
					neighbour->parent = currentNode;

					AddNodeToList(neighbour, &openSet);
				}
				//if it is already, check if the new g-cost is lower than the previous and replace it
				else 
				{
					if (newMovementCost < existing->g)
					{
						existing->g = newMovementCost;
						existing->h = GetNodeDistance(neighbour, targetNode);
						existing->parent = currentNode;
					}
					free(neighbour);
				}
			}
			//else free it
			else
			{
				free(neighbour);
			}
			neighbour = nextNode;
		}
		i++;
	}
	//if the while loop end, then the algorithm didn't find a path.
	printf("A* failed with %d blocks checked\n", i);
	return 0;
}