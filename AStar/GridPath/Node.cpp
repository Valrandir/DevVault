#include <stdlib.h> //abs
#include "Path.h"

//warning C4482: nonstandard extension used: enum 'Path::Node::NodeStateEnum' used in qualified name
#pragma warning(disable:4482)

bool Path::Node::CompareKey(const Node* ln, const Node* rn)
{
	return ln->Key < rn->Key;
}

bool Path::Node::CompareScore(const Node* ln, const Node* rn)
{
	return ln->Score > rn->Score;
}

int Path::Node::MakeKey(const Point& pt, int GridSizeX)
{
	return pt.x + pt.y * GridSizeX;
}

void Path::Node::Eval(const Point& Position, const Point& Goal, const Node* PrevNode, int& PathSpan, int& GoalSpan, int& Score)
{
	if(PrevNode)
	{
		if(Position.x == PrevNode->Position.x || Position.y == PrevNode->Position.y)
			PathSpan = PrevNode->PathSpan + DistanceStraight;
		else
			PathSpan = PrevNode->PathSpan + DistanceDiagonal;
	}
	else
		PathSpan = 0;

	GoalSpan = abs(Position.x - Goal.x) + abs(Position.y - Goal.y);
	Score = PathSpan + GoalSpan * DistanceStraight;
}

Path::Node::Node() {}

Path::Node::Node(const Point& Position, const Point& Goal, Node* PrevNode, int GridSizeX) :
	Position(Position),
	PrevNode(PrevNode),
	Key(MakeKey(Position, GridSizeX)),
	NodeState(Node::NodeStateEnum::Open)
{
	Eval(Position, Goal, PrevNode, PathSpan, GoalSpan, Score);
}

Path::Node::Node(const Point& Position, int PathSpan, int GoalSpan, int Score, Node* PrevNode, int GridSizeX) :
	Position(Position),
	PathSpan(PathSpan),
	GoalSpan(GoalSpan),
	Score(Score),
	PrevNode(PrevNode),
	Key(MakeKey(Position, GridSizeX)),
	NodeState(Node::NodeStateEnum::Open)
{}
