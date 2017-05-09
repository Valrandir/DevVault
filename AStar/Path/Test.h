void Test();

#include <set>

struct Node
{
	int Key;

	bool operator<(const Node& node) const
	{
		return Key < node.Key;
	}

	bool operator()(const Node* n1, const Node* n2) const
	{
		return n1->Key < n2->Key;
	}

	Node() : Key(0) {}
	Node(int Key) : Key(Key) {}
};

using namespace std;

class FnOpTest
{
	public:

	bool operator()(const Node* n1, const Node* n2) const
	{
		return n1->Key < n2->Key;
	}
};

void Test()
{
	set<Node*, Node> vNode;
	set<Node*, Node>::const_iterator it;
	int i;

	for(i=1; i<=20; i+=2)
		vNode.insert(new Node(i));

	for(i=2; i<=20; i+=2)
		vNode.insert(new Node(i));

}
