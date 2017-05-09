#pragma once
#include <set>
#include <deque>

class Path
{
	public:
	struct Point
	{
		int x, y;
		bool operator==(const Point& pt) const;
		Point& operator+=(const Point& pt);
	};

	typedef bool(*CanPathCallback)(int, int, void*);
	typedef std::deque<Point> PointList;

	private:
	struct Node
	{
		enum NodeStateEnum
		{
			Open, Closed
		} NodeState;

		int PathSpan, GoalSpan, Score;
		Point Position;
		Node* PrevNode;
		int Key;

		static bool CompareKey(const Node* ln, const Node* rn);
		static bool CompareScore(const Node* ln, const Node* rn);
		static int MakeKey(const Point& pt, int GridSizeX);
		static void Eval(const Point& Position, const Point& Goal, const Node* PrevNode, int& PathSpan, int& GoalSpan, int& Score);

		Node();
		Node(const Point& Position, const Point& Goal, Node* PrevNode, int GridSizeX);
		Node(const Point& Position, int PathSpan, int GoalSpan, int Score, Node* PrevNode, int GridSizeX);
	};

	typedef std::set<Node*, bool(*)(const Node*, const Node*)> NodeSet;
	typedef std::multiset<Node*, bool(*)(const Node*, const Node*)> NodeMultiSet;

	static const int DistanceStraight;
	static const int DistanceDiagonal;
	static const Point Area[8];

	void NodeSetClear(NodeSet& ns);
	Node* GetBestOpenNode(NodeMultiSet::const_iterator* NodeIterator = NULL) const;
	Node* NodeSetFind(const Point& pt, const NodeSet& ns) const;
	bool ProcessNode(Node& SourceNode, const Point& TargetPoint);
	bool CanPath(int x, int y);

	int GridSizeX;
	Point Source, Target;
	Node *lpLink;
	NodeSet vNodeKey;
	NodeMultiSet vNodeScore;
	CanPathCallback CanPathProc;
	void* CallbackParam;

	public:
	Path();
	~Path();
	void Clear();
	void Setup(const Point& Source, const Point& Target, int GridSizeX, CanPathCallback CanPathProc, void* CallbackParam);
	int Process();
	int Tick();
	PointList* TracePath();
};
