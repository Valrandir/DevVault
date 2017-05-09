#include <assert.h>
#include "Path.h"

//warning C4482: nonstandard extension used: enum 'Path::Node::NodeStateEnum' used in qualified name
#pragma warning(disable:4482)

const int Path::DistanceStraight = 10;
const int Path::DistanceDiagonal = 14;
const Path::Point Path::Area[8] = {-1,0,0,-1,1,0,1,0,0,1,0,1,-1,0,-1,0};

void Path::NodeSetClear(NodeSet& ns)
{
	NodeSet::const_iterator it, end = ns.end();
	for(it = ns.begin(); it != end; it++)
		delete *it;
	ns.clear();
}

Path::Node* Path::GetBestOpenNode(NodeMultiSet::const_iterator* NodeIterator) const
{
	*NodeIterator = vNodeScore.end();
	return *(--(*NodeIterator));
}

Path::Node* Path::NodeSetFind(const Point& pt, const NodeSet& ns) const
{
	Node n;
	NodeSet::const_iterator it;

	n.Key = Node::MakeKey(pt, GridSizeX);
	it = ns.find(&n);

	return it == ns.end() ? NULL : *it;
}

Path::Path() :
	vNodeKey(Node::CompareKey),
	vNodeScore(Node::CompareScore)
{}

Path::~Path()
{
	Clear();
}

void Path::Clear()
{
	vNodeScore.clear();
	NodeSetClear(vNodeKey);
}

void Path::Setup(const Point& Source, const Point& Target, int GridSizeX, CanPathCallback CanPathProc, void* CallbackParam)
{	
	Node* lpNode;

	this->Source = Source;
	this->Target = Target;
	this->GridSizeX = GridSizeX;
	this->CanPathProc = CanPathProc;
	this->CallbackParam = CallbackParam;

	Clear();
	lpNode = new Node(Source, Target, NULL, GridSizeX);
	vNodeKey.insert(lpNode);
	vNodeScore.insert(lpNode);
}

int Path::Process()
{
	int ret;
	while(!(ret = Tick()));
	return ret;
}

/*Returns
	0 when not found yet
	1 when found
	-1 when no path
*/
int Path::Tick()
{
	Point pt;
	NodeMultiSet::const_iterator NodeIterator;
	int i;

	//Valid Movement Check
	if(!CanPath(Target.x, Target.y))
		return -1;

	//Find the Open Node with the smallest PathSpan
	assert(!vNodeScore.empty());
	lpLink = GetBestOpenNode(&NodeIterator);

	//Process each Node around it
	pt = lpLink->Position;
	for(i = 0; i < 8; i++)
	{
		pt += Area[i];
		if(ProcessNode(*lpLink, pt))
			return 1;
	}

	//Set NodeState to Closed
	lpLink->NodeState = Node::NodeStateEnum::Closed;
	vNodeScore.erase(NodeIterator);

	//No path exists
	if(vNodeScore.empty())
	{
		lpLink = NULL;
		return -1;
	}

	return 0;
}

bool Path::ProcessNode(Node& SourceNode, const Point& TargetPoint)
{
	Node* lpNode;
	int PathSpan, GoalSpan, Score;

	//Is Previous Node check
	if(SourceNode.PrevNode && SourceNode.PrevNode->Position == TargetPoint)
		return false;

	//Target reached check
	if(TargetPoint == Target)
	{
		lpLink = new Node(TargetPoint, Target, &SourceNode, GridSizeX);
		return true;
	}

	//Valid movement check
	if(!CanPath(TargetPoint.x, TargetPoint.y))
		return false;

	//TargetPoint might be in vNodeKey
	lpNode = NodeSetFind(TargetPoint, vNodeKey);

	//Closed Node Check
	if(lpNode && lpNode->NodeState == Node::NodeStateEnum::Closed)
		return false;

	//Process Node
	if(lpNode == NULL)
	{
		//Create the Node and add it to the Open list
		lpNode = new Node(TargetPoint, Target, &SourceNode, GridSizeX);
		vNodeKey.insert(lpNode);
		vNodeScore.insert(lpNode);
	}
	else
	{
		//Check whether Node is better off having it's parent set to SourceNode
		Node::Eval(TargetPoint, Target, &SourceNode, PathSpan, GoalSpan, Score);
		if(Score < lpNode->Score)
		{
			//Make Node path from SourceNode instead
			lpNode->PathSpan = PathSpan;
			lpNode->GoalSpan = GoalSpan;
			lpNode->Score = Score;
			lpNode->PrevNode = &SourceNode;
		}
	}

	return false;
}

bool Path::CanPath(int x, int y)
{
	return CanPathProc ? CanPathProc(x, y, CallbackParam) : false;
}

Path::PointList* Path::TracePath()
{
	PointList* deque = new PointList();

	while(lpLink)
	{
		deque->push_front(lpLink->Position);
		lpLink = lpLink->PrevNode;
	}

	Clear();
	return deque;
}
