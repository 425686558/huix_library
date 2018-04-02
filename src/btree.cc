#include <btree>
#include <stdlib.h>
namespace Huix {
BTree::BTree()
{
}

BTroot::BTroot(int kcnt): kcnt(kcnt)
{
	child = new BTree();
}

};
