#pragma once
#pragma pack(push, 1)

#include "avlExceptions.hpp"
#include "vector_c.hpp"

typedef unsigned int UINT;
typedef unsigned char BYTE;

#ifndef INIT_UINT
#define INIT_UINT 0xffffffff
#endif


// the _c is for compact
// usage: avlNexus<T> a_n; ... UINT avlNum = a_n.getNext(); ... a_n.avl_store[avlNum].{AVL method} 
typedef struct AVLDefragType
{
	AVLDefragType(void): sPtr(NULL) {}
	AVLDefragType(const AVLDefragType & a): sPtr(a.sPtr) {}
	AVLDefragType(pUINT pIn): sPtr(pIn) {}
	bool IsLT(const AVLDefragType & a) const {UINT A = *sPtr, B = *(a.sPtr); return A < B;}
	friend bool operator < (const AVLDefragType & a, const AVLDefragType & b) {return a.IsLT(b);}
	bool IsEQ(const AVLDefragType & a) const {UINT A = *sPtr, B = *(a.sPtr); return A == B;}
	friend bool operator == (const AVLDefragType & a, const AVLDefragType & b) {return a.IsEQ(b);}
	
	pUINT sPtr;

} AVLDefrag, * pAVLDefrag;

typedef struct AVLNodeType
{
	AVLNodeType(void): left(INIT_UINT), right(INIT_UINT), data(INIT_UINT), flags(0) {}
	AVLNodeType(const AVLNodeType & a): left(a.left), right(a.right), data(a.data), flags(a.flags) {}
	AVLNodeType(const UINT & l, const UINT & r, const UINT & d, const BYTE & f): left(l), right(r), data(d), flags(f) {}
	AVLNodeType & operator = (const AVLNodeType & a)
	{
		left = a.left; right = a.right; data = a.data; flags = a.flags;
		return *this;
	}
	void setDelFlag(void) {flags |= 0x4u;} // bit 2 is DELETE flag
	bool isDeleted(void) {return 0x4u == (flags & 0x4u);}
	void clearBalance(void) {flags &= 0xfcu;} // balance is in last 2 bits of flags
	void setRightHeavy(void) {clearBalance(); flags |= 0x1u;} // bit0 is RIGHT-HEAVY flag
	void setLeftHeavy(void) {clearBalance(); flags |= 0x2u;} // bit1 is LEFT-HEAVY flag
	int getBalance(void) 
	{
		int r = flags & 0x3u; // last 2 bits s/b 00, 10 or 01
		if (r == 3) throw AVLX::balance();
		if (r == 2) r = -1;
		return r; 
	}
	::std::string toStr(void) 
	{
		return "AVL_c Node right[" + ::std::to_string(right) + 
			"] left[" + ::std::to_string(left) + 
			"] data[" + ::std::to_string(data) + 
			"] flags[" + ::std::to_string(static_cast<UINT>(flags)) + 
			"]\n";
	}
    UINT right, left, data;
    BYTE flags;

} AVLNode, * pAVLNode;

typedef struct AVLPathElementType
{
	AVLPathElementType(void): node(INIT_UINT), direction(' ') {}
	AVLPathElementType(const AVLPathElementType & a): node(a.node), direction(a.direction) {}
	AVLPathElementType(const UINT n, const char d): node(n), direction(d) {}
	AVLPathElementType & operator = (const AVLPathElementType & a) {node = a.node; direction = a.direction; return *this;}

	UINT node;
	char direction;

} AVLPathElement, * pAVLPathElement;

// for a path: typename S tracks log(N) - so the tree max here is ~ 2^256
typedef struct AVLPathType: public vector_c<AVLPathElement, BYTE> 
{ // implementing a one-at-a-time vector:
	AVLPathType(void): vector_c() {}
	UINT push(const AVLPathElement & a) {return vector_c::push(a);}
	UINT push(const UINT & n, const char & d) {return vector_c::push(AVLPathElement(n, d));}

} AVLPath, * pAVLPath;


template<typename T> class AVL; // forward declaration

template<typename T> 
class avlNexus
{ // this will serve as a container for AVLs
  // it will handle memory as vectors for nodes, data, and roots
  // defrag will be available, it can cleanup the vectors after a number of delete calls
public:
	avlNexus(void) {}
	~avlNexus(void);

	void Defrag(void); // call sparingly (it's expensive timewise)
	UINT getNext(void);
	UINT Size(void);

	::std::vector<T> data_store;
	::std::vector<AVLNode> node_store;
	::std::vector<UINT> roots;
	::std::vector<class AVL<T> *> avl_store;
private:
	void wholeTrees(void);

};
template<typename T> using pavlNexus = class avlNexus<T> *;

template<typename T>
class AVL
{
	AVLPath sPath;
	avlNexus<T> & NEX;
public:
	const UINT ID;
    AVL(avlNexus<T> &, UINT);
	AVL(const AVL<T> &);
    ~AVL(void);
    bool Insert(T *&, UINT &);
    bool AVLEmpty(void);
    bool GetLeast(T &, UINT &);
	bool GetLE(T &, UINT &); // less or equal
	bool GetGE(T &, UINT &); // greater or equal (like lower_bound)
    void Delete(void); // deletes node at sPath[pathTop]
	void getAll(::std::vector<T> &);
	bool findData(T &, UINT &);
	UINT Size(void);
	UINT detailSizes(void);
	void getAllWith(::std::vector<T> &, ::std::vector<UINT> &);


private:
    UINT Find(const T &); // if found returns node index
    BYTE GetRealHeight(const UINT);
    void DeletePath(void);
    UINT rotateRight(const UINT);
    UINT rotateLeft(const UINT);
	UINT rotateRightLeft(const UINT);
	UINT rotateLeftRight(const UINT);
    UINT Rotate(pAVLPathElement, int &);
	pAVLNode getNode(const UINT); // use before the vector resizes
	UINT setNode(const AVLNode);
	void getAtNode(::std::vector<T> &, UINT);
	T * getData(UINT);
	UINT setData(const T &);
	UINT getRoot(void);
	void setRoot(const UINT);
	UINT getNodeCount(void);
	void countNode(UINT &, UINT);
	void getAtNodeWith(::std::vector<T> &, ::std::vector<UINT> &, UINT);
};

template<typename T>
AVL<T>::AVL(avlNexus<T> & nex, UINT id): NEX(nex), ID(id) {}

template<typename T>
AVL<T>::AVL(const AVL<T> & a): sPath(a.sPath), NEX(a.NEX), ID(a.ID) {}

template<typename T>
AVL<T>::~AVL(void) {}


template<typename T>
UINT AVL<T>::Find(const T & dataIn)
{ // class T must have ops == and < defined
  // this method creates a path in class vector sPath
  // if it finds input parameter *a the last path entry
  // will have the node index as element 'node'
  // and the default direction ' ' as element 'direction'
  // and, of course, the return will not be INIT_UINT
  // if *a is not found return will be INIT_UINT
  // last path entry will contain leaf last checked
  // if direction is 'r' dataIn is > *getData(getNode(leaf.node)->data)
  // if direction is 'l' dataIn is < *getData(getNode(leaf.node)->data)
	DeletePath();
	UINT nIdx = getRoot(), current_path = 0, current_node = 0;
	if (nIdx == INIT_UINT) return INIT_UINT; // nothing here
	while (nIdx != INIT_UINT)
	{
		sPath.push(AVLPathElement());
		pAVLPathElement pth = &sPath[current_path++]; // ptr into vector
		pth->node = nIdx;
		pAVLNode p = getNode(nIdx);
		T current_data = *getData(p->data);
		if (dataIn == current_data) return nIdx;
		nIdx = p->right;
		pth->direction = 'r';
		if (dataIn < current_data) 
		{
			nIdx = p->left; 
			pth->direction = 'l';
		}
		p = getNode(nIdx);
	}
	return nIdx;
}

template<typename T>
bool AVL<T>::Insert(T *& dataInOut, UINT & dIdx) 
	// returns true iff T is inserted i.e. not in tree before call
	// do not call Find before as this will duplicate call
{
	UINT nIdx = 0;
	if (AVLEmpty()) // no root: setting up
	{
		dIdx = setData(*dataInOut); 
		nIdx = setNode(AVLNode(INIT_UINT, INIT_UINT, dIdx, 0));
		setRoot(nIdx);
		return true;
	}
	nIdx = Find(*dataInOut);
	if(nIdx != INIT_UINT) 
	{ // Found It!
		dIdx = getNode(nIdx)->data;
		dataInOut = getData(dIdx);
		return false; // already in, so no insert
	}
	// Did not find, proceed w/ insert
	pAVLPathElement pth = NULL;
	if (sPath.size() == 0) throw(AVLX::root()); // do not set up root here
	BYTE pthIdx = sPath.size() - 1; // node here is leaf w/ our initial insertion point

	dIdx = setData(*dataInOut); 
	nIdx = setNode(AVLNode(INIT_UINT, INIT_UINT, dIdx, 0)); // our new leaf w/ node index = nIdx
	pth = &sPath[pthIdx];
	pAVLNode pthNode = getNode(pth->node);
	char d = pth->direction;
	if ((d != 'l') && (d != 'r')) throw(AVLX::path_dir());
	if (d == 'r')
	{
		if (pthNode->right != INIT_UINT) throw(AVLX::right_node());
		pthNode->right = nIdx;
	}
	else // left
	{
		if (pthNode->left != INIT_UINT) throw(AVLX::left_node());
		pthNode->left = nIdx;	
	}

	// our Insert balancing routine:
	sPath.push(AVLPathElement(nIdx, ' ')); // all path ptrs could now be invalid
	++pthIdx; // now points to new path element added in the last line
	do
	{
		--pthIdx; // we will process root here too
		pth = &sPath[pthIdx];
		nIdx = pth->node;
		pthNode = getNode(nIdx);

		int bf = pthNode->getBalance(), bfY = 0;
		
		d = pth->direction;
		if ((d != 'l') && (d != 'r')) throw(AVLX::path_dir());
		if (d == 'r') ++bf;
		else --bf;
		if (bf == 0)
		{ // tree is balanced:
			pthNode->clearBalance();
			DeletePath();
			return true;
		}
		if ((bf < -1) || (bf > 1))
		{
			nIdx = Rotate(pth, bfY); // Rotate will clear the balances of the rotated nodes
			if (pthIdx == 0) setRoot(nIdx);
			else 
			{
				pth = &sPath[pthIdx - 1];
				pthNode = getNode(pth->node);
				d = pth->direction;
				if (d == 'r') pthNode->right = nIdx;
				else pthNode->left = nIdx;
			}
			pthNode = getNode(nIdx);
			//assert (0 == pthNode->getBalance());
			DeletePath(); // path invalid: it's been re-routed by Rotate
			return true;
		}
		if ((bf != -1) && (bf != 1)) throw(AVLX::balance());
		if (bf == 1) pthNode->setRightHeavy();
		else pthNode->setLeftHeavy();
	} while (pthIdx > 0);
	
	return true;
}

template<typename T>
bool AVL<T>::AVLEmpty(void)
{
	if (getRoot() == INIT_UINT) return true;
	return false;
}

template<typename T>
bool AVL<T>::GetLeast(T & dataOut, UINT & dIdx)
{ 
	DeletePath();
	dIdx = INIT_UINT;
	if (AVLEmpty()) return false;
	UINT nIdx = getRoot();
	pAVLNode n = getNode(nIdx); 
	while (n != NULL)
	{
		sPath.push(AVLPathElement(nIdx, 'l'));
		nIdx = n->left;
		n = getNode(nIdx);
	}
	BYTE pathTop = sPath.size() - 1;
	pAVLPathElement pth = &sPath[pathTop];
	nIdx = pth->node;
	n = getNode(nIdx);
	dIdx = n->data;
	dataOut = *getData(dIdx);
	Delete();
	return true;
}

template<typename T>
bool AVL<T>::GetLE(T & dataInOut, UINT & dIdx)
{
	UINT nIdx = Find(dataInOut);
	dIdx = INIT_UINT;
	pAVLNode p = NULL;
	if (nIdx != INIT_UINT) 
	{
		p = getNode(nIdx);
		dIdx = p->data;
		dataInOut = *getData(dIdx);
		return true;
	}
	BYTE pthIdx = sPath.size();
	if (pthIdx == 0) return false;
	do
	{
		--pthIdx;
		pAVLPathElement pth = &sPath[pthIdx];
		nIdx = pth->node;
		p = getNode(nIdx);
		dIdx = p->data;
		char d = pth->direction;
		if (d == 'r') 
		{
			dataInOut = *getData(dIdx);
			return true;
		}
	} while (pthIdx > 0);

	return false;

}

template<typename T>
bool AVL<T>::GetGE(T & dataInOut, UINT & dIdx)
{
	UINT nIdx = Find(dataInOut);
	dIdx = INIT_UINT;
	pAVLNode p = NULL;
	if (nIdx != INIT_UINT) 
	{
		p = getNode(nIdx);
		dIdx = p->data;
		dataInOut = *getData(dIdx);
		return true;
	}
	BYTE pthIdx = sPath.size();
	if (pthIdx == 0) return false;
	do
	{
		--pthIdx;
		pAVLPathElement pth = &sPath[pthIdx];
		nIdx = pth->node;
		p = getNode(nIdx);
		dIdx = p->data;
		char d = pth->direction;
		if (d == 'l') 
		{
			dataInOut = *getData(dIdx);
			return true;
		}
	} while (pthIdx > 0);
	return false;
}

template<typename T>
BYTE AVL<T>::GetRealHeight(const UINT tNodeIdx)
{
	if (tNodeIdx == INIT_UINT) return 0;
	pAVLNode tNode = getNode(tNodeIdx);
	return (1 + _MAX(GetRealHeight(tNode->right), GetRealHeight(tNode->left)));
}

template<typename T>
void AVL<T>::DeletePath(void)
{
	sPath.clear();
}

template<typename T>
UINT AVL<T>::rotateRight(const UINT xIdx)
{  
	pAVLNode x = getNode(xIdx);
	UINT yIdx = x->left;
	pAVLNode y = getNode(yIdx);
	x->left = y->right;
	y->right = xIdx;
	if (0 == y->getBalance())
	{
		//::std::cout << "rr0";
		x->setLeftHeavy();
		y->setRightHeavy();
	}
	else
	{
		x->clearBalance();
		y->clearBalance();
	}
	return yIdx;
}

template<typename T>
UINT AVL<T>::rotateLeft(const UINT xIdx)
{  // prerequisites: x!=NULL && x->right!=NULL
	pAVLNode x = getNode(xIdx);
	UINT yIdx = x->right;
	pAVLNode y = getNode(yIdx);
	x->right = y->left;
	y->left = xIdx;
	if (0 == y->getBalance())
	{
		//::std::cout << "rl0";
		x->setRightHeavy();
		y->setLeftHeavy();
	}
	else
	{
		x->clearBalance();
		y->clearBalance();
	}
	return yIdx;
}

template<typename T>
UINT AVL<T>::rotateRightLeft(const UINT xIdx)
{
	pAVLNode x = getNode(xIdx);
	UINT yIdx = x->right;
	pAVLNode y = getNode(yIdx);
	UINT zIdx = y->left;
	pAVLNode z = getNode(zIdx);
	y->left = z->right;
	z->right = yIdx;
	x->right = z->left;
	z->left = xIdx;
	switch(z->getBalance())
	{
		case -1:
			x->clearBalance();
			y->setRightHeavy();
			break;
		case  0:
			x->clearBalance();
			y->clearBalance();
			break;
		case  1:
			x->setLeftHeavy();
			y->clearBalance();
			break;
	}
	z->clearBalance();
	return zIdx;
}

template<typename T>
UINT AVL<T>::rotateLeftRight(const UINT xIdx)
{
	pAVLNode x = getNode(xIdx);
	UINT yIdx = x->left;
	pAVLNode y = getNode(yIdx);
	UINT zIdx = y->right;
	pAVLNode z = getNode(zIdx);
	y->right = z->left;
	z->left = yIdx;
	x->left = z->right;
	z->right = xIdx;
	switch(z->getBalance())
	{
		case -1:
			x->setRightHeavy();
			y->clearBalance();
			break;
		case  0:
			x->clearBalance();
			y->clearBalance();
			break;
		case  1:
			x->clearBalance();
			y->setLeftHeavy();
			break;
	}
	z->clearBalance();
	return zIdx;
}

template<typename T>
UINT AVL<T>::Rotate(pAVLPathElement rPth, int & sBF)
{
	// uses vector_c sPath to perform proper rotation at pth_idx node
	//pAVLPathElement rPth = &sPath[pth_idx], sPth = &sPath[pth_idx + 1]; 
	char rd = rPth->direction, sd = 'r'; //sPth->direction;
	UINT rn = rPth->node, sn = INIT_UINT;
	pAVLNode r = getNode(rn), s = NULL;
	sn = rd == 'r'? r->right: r->left;
	s = getNode(sn);

	sBF = s->getBalance();
	if (((rd == 'l') && (sBF <= 0)) || ((rd == 'r') && (sBF < 0))) sd = 'l';

	//r->clearBalance();
	//s->clearBalance();
	if (rd == sd)
	{
		if (rd == 'l') return rotateRight(rn); // Left Left
		else return rotateLeft(rn); // Right Right
	}
	if (rd == 'l') return rotateLeftRight(rn); // Left Right
	//{
	//	r->left = rotateLeft(sn);
	//	return rotateRight(rn);
	//}
	return rotateRightLeft(rn);// Right Left
	//r->right = rotateRight(sn);
	//return rotateLeft(rn);
}

template<typename T>
pAVLNode AVL<T>::getNode(const UINT ndIdx)
{
	if (ndIdx == INIT_UINT) return NULL;
	return &(NEX.node_store[ndIdx]);
} // use pointer before a call to setNode below
  // might resize the vector

template<typename T>
UINT AVL<T>::setNode(const AVLNode n) 
{
	UINT nSz = NEX.node_store.size();
	NEX.node_store.push_back(n);
	return nSz;
}

template<typename T>
void AVL<T>::getAll(::std::vector<T> & res)
{ 
	res.clear();
	getAtNode(res, getRoot());
}

template<typename T>
void AVL<T>::getAtNode(::std::vector<T> & res, UINT nIdx)
{
	pAVLNode node = getNode(nIdx);
	if (node == NULL) return;
	getAtNode(res, node->left);
	res.push_back(*getData(node->data));
	getAtNode(res, node->right);
}

template<typename T>
void AVL<T>::getAllWith(::std::vector<T> & ts, ::std::vector<UINT> & nds)
{ 
	ts.clear();
	nds.clear();
	getAtNodeWith(ts, nds, getRoot());
}

template<typename T>
void AVL<T>::getAtNodeWith(::std::vector<T> & ts, ::std::vector<UINT> & nds, UINT nIdx)
{
	pAVLNode node = getNode(nIdx);
	if (node == NULL) return;
	getAtNodeWith(ts, nds, node->left);
	ts.push_back(*getData(node->data));
	nds.push_back(nIdx);
	getAtNodeWith(ts, nds, node->right);
}

template<typename T>
UINT AVL<T>::getNodeCount(void)
{
	UINT totalNodes = 0, root = getRoot();
	countNode(totalNodes, root);
	return totalNodes;
}

template<typename T>
void AVL<T>::countNode(UINT & count, UINT nIdx)
{
	pAVLNode node = getNode(nIdx);
	if (node == NULL) return;
	++count;
	countNode(count, node->left);
	countNode(count, node->right);

}

template<typename T>
T * AVL<T>::getData(UINT dIdx)
{
	UINT dSz = NEX.data_store.size();
	if (dIdx < dSz) return &(NEX.data_store[dIdx]);
	return NULL;
}

template<typename T>
UINT AVL<T>::setData(const T & d)
{
	UINT dSz = NEX.data_store.size();
	NEX.data_store.push_back(d);
	return dSz;
}
		
template<typename T>
void AVL<T>::Delete(void) // deletes node at sPath[pathTop]
{ // usually requires operator= for class T
	if (sPath.size() == 0) return; // no path
	BYTE top = 0, pathTop = sPath.size() - 1;
	pAVLPathElement pth = &sPath[pathTop], qth = NULL;
	pAVLNode p = getNode(pth->node), q = NULL;
	UINT pIdx = pth->node, qIdx = INIT_UINT, dataIdx = INIT_UINT;
	if (p == NULL) return;  // node not found
	char d = pth->direction;
	if (d != ' ') return; // a successful Find, the prerequisite, will have this in final path element
	//bool delRoot = pIdx == getRoot();
	// standard BST delete:
	if ((p->left == INIT_UINT) && (p->right == INIT_UINT))
	{
		if (pathTop == 0)
		{ // delete last node, p is root
			p->setDelFlag();
			setRoot(INIT_UINT);
			DeletePath();
			return;
		}
		top = pathTop - 1;
		pth = &sPath[top];
		d = pth->direction;
		qIdx = pth->node;
		q = getNode(qIdx);
		if (d == 'r') q->right = INIT_UINT;
		else q->left = INIT_UINT;
		p->setDelFlag();
	}
	else
	{
		if (p->left == INIT_UINT) // 1 child on right
		{
			if (pathTop == 0)
			{ // the balancing routines guarantee p->right has no subtrees
				setRoot(p->right);
				DeletePath();
				return;
			}
			else
			{
				top = pathTop - 1;
				pth = &sPath[top];
				d = pth->direction;
				qIdx = pth->node;
				q = getNode(qIdx);
				if (d == 'r') 
				{
					q->right = p->right;
				}
				else q->left = p->right;
			}
			p->setDelFlag();
		}
		else
		{
			if (p->right == INIT_UINT) // 1 child on left
			{
				if (pathTop == 0)
				{ // the balancing routines guarantee p->left has no subtrees
					setRoot(p->left);
					DeletePath();
					return; 
				}
				else
				{
					top = pathTop - 1;
					pth = &sPath[top];
					d = pth->direction;
					qIdx = pth->node;
					q = getNode(qIdx);
					if (d == 'r') 
					{
						q->right = p->left;
					}
					else q->left = p->left;
				}
				p->setDelFlag();
			}
			else // has both children
			{
				qIdx = p->right;
				q = getNode(qIdx);
				pth->direction = 'r';
				sPath.push(AVLPathElement(qIdx, ' '));
				qth = &sPath[++pathTop];
				
				do
				{ // getting the left-most node of the subtree on the right:
					qIdx = q->left;
					if (qIdx != INIT_UINT)
					{
						qth->direction = 'l';
						sPath.push(AVLPathElement(qIdx, ' '));
						qth = &sPath[++pathTop];
						q = getNode(qIdx);
					}
				} while (qIdx != INIT_UINT);
				dataIdx = p->data;
				p->data = q->data; // exchanging data_store indices
				q->data = dataIdx;
				top = pathTop - 1; // pathTop >= 1 pathTop is top index not size of sPath
				qth = &sPath[top];
				d = qth->direction;
				qIdx = qth->node;
				if (d == 'r') 
				{ // this will only happen if p's right subtree has only one node
					p->right = q->right;
				}
				else getNode(qIdx)->left = q->right;
				q->setDelFlag();
			}
		}
	} //finish of standard BST delete
	// now to balance from top up:
	top = pathTop; // our highest path element (should now refer to a deleted node)
	if (top == 0) throw(AVLX::path_empty());
	do
	{
		--top;
		pth = &sPath[top];
		p = getNode(pth->node);
		int bf = p->getBalance(), bfY = 0;
		d = pth->direction;
		if((d != 'l') && (d != 'r')) throw(AVLX::path_dir());
		if (d == 'r') --bf;
		else ++bf;
		if ((bf == -1) || (bf == 1))
		{ // height same
			if (bf == 1) p->setRightHeavy();
			else p->setLeftHeavy();
			DeletePath();
			return;
		}
		if (bf == 0) p->clearBalance(); // tree balanced but height changed
		if ((bf < -1) || (bf > 1))
		{
			AVLPathElement pe = AVLPathElement(pth->node, d == 'r'? 'l': 'r'); // this is path to sibling
			qth = &pe;
			pIdx = Rotate(qth, bfY);
			if (top == 0) setRoot(pIdx); // all paths start at the root
			else
			{
				qth = &sPath[top - 1];
				qIdx = qth->node;
				d = qth->direction;
				if((d != 'l') && (d != 'r')) throw(AVLX::path_dir());
				q = getNode(qIdx);
				if (q == NULL) throw(AVLX::path_node());
				if (d == 'r') q->right = pIdx;
				else q->left = pIdx;
			}
			p = getNode(pIdx);
			if (0 == bfY)
			{
				DeletePath();
				return;
			}
		}
	} while (top > 0);
	DeletePath();

}

template<typename T>
bool AVL<T>::findData(T & dataInOut, UINT & dataIdx)
{
	UINT ndIdx = Find(dataInOut);
	dataIdx = INIT_UINT;
	if (ndIdx == INIT_UINT) return false;
	dataIdx = getNode(ndIdx)->data;
	dataInOut = *getData(dataIdx);
	return true;
}

template<typename T>
UINT AVL<T>::Size(void)
{
	UINT bCt = 0;
	bCt += sPath.capacity() * sizeof(AVLPathElement);	
	return bCt;
}

template<typename T>
UINT AVL<T>::detailSizes(void)
{
	BYTE c = sPath.capacity();
	UINT 
		nTot = getNodeCount(), 
		total = sizeof(AVL<T>) + sizeof(AVLPathElement) * c + nTot * (sizeof(AVLNode) + sizeof(T)) + sizeof(UINT);
	std::string capStr = std::to_string(c);
	std::cout << "AVL[" << ID << "] is AVL<T> size: (" << sizeof(AVL<T>) << ") + path (" << sizeof(AVLPathElement) << " * " << capStr << ")" <<
		" + node count: (" << nTot << ") * ( AVLNode size: " << sizeof(AVLNode) << " + data size: " << sizeof(T) << ") "
		" + root index size: (" << sizeof(UINT) << ") = [" << total << "]\n";
	return total;
}

template<typename T>
UINT AVL<T>::getRoot(void) {return NEX.roots[ID];}

template<typename T>
void AVL<T>::setRoot(UINT r) {NEX.roots[ID] = r;}


template<typename T>
avlNexus<T>::~avlNexus(void)
{
	UINT aSz = avl_store.size();
	for (UINT i = 0; i < aSz; ++i)
	{
		delete(avl_store[i]);
	}
}

template<typename T>
void avlNexus<T>::wholeTrees(void)
{
	::std::vector<UINT> delVec;
	UINT aSz = avl_store.size();
	for (UINT i = 0; i < aSz; ++i)
	{
		AVL<T> avl = *avl_store[i];
		if (avl.AVLEmpty())
		{
			delVec.push_back(i);

		}
	}
	UINT dSz = delVec.size();
	if (dSz == 0) return;

	typename ::std::vector<class AVL<T> *>::iterator aIT = avl_store.begin();

	::std::vector<UINT>::iterator rIT = roots.begin();

	for (UINT i = dSz; i > 0; --i)
	{ 
	  	aIT = avl_store.begin();
		::std::advance(aIT, delVec[i - 1]); 
		avl_store.erase(aIT);
		rIT = roots.begin();
		::std::advance(rIT, delVec[i - 1]);
		roots.erase(rIT);
	}

	aSz = avl_store.size();
	for (UINT i = 0; i < aSz; ++i)
	{
		UINT * pID = const_cast<UINT *>(&avl_store[i]->ID);
		*pID = i;
	}

}

template<typename T>
void avlNexus<T>::Defrag(void)
{ // don't use this unless you must 
  // requires a loop thru the nodes (getting 4 sorted lists) 
  // and then thru the vector::erases (for all your deletes) in data_store and node_store
  // also requires a loop thru the roots (getting 1 more sorted list)
  // the sorted lists will be checked against the deleted nodes
  // so that adjustments to the tree nodes, data and roots can be made
	::std::vector<UINT> delVec, dataDelVec;
	avlNexus<AVLDefrag> nex; // using avl to get sorted vectors below
    enum avl_enum_type {NODE = 0, DATA = 1, DATADEL = 2}; //, ROOT = 3};
	UINT avls[] = {nex.getNext(), nex.getNext(), nex.getNext()}; //, nex.getNext()}; //nodeAVL, dataAVL, dataDelAVL, rootAVL;
	::std::vector<AVLDefrag> nodeVec, dataVec, dataDelVec0, rootVec; // all to be sorted by AVL
	UINT nstrSz = node_store.size(), insIdx = 0, rSz = roots.size(), maxUINT = INIT_UINT; 
	AVLDefrag aDef;
	pAVLDefrag p = &aDef;
	for (UINT i = 0; i < nstrSz; ++i)
	{
		AVLNode & node = node_store[i];
		if (node.isDeleted()) 
		{
			delVec.push_back(i);
			aDef.sPtr = &node.data;
			if (!nex.avl_store[avls[DATADEL]]->Insert(p, insIdx)) throw(AVLX::avl_insert());
		}
		else 
		{
			if (node.left != INIT_UINT) 
			{
				aDef.sPtr = &node.left;
				if (!nex.avl_store[avls[NODE]]->Insert(p, insIdx)) throw(AVLX::avl_insert());
			}
			if (node.right != INIT_UINT) 
			{
				aDef.sPtr = &node.right;
				if (!nex.avl_store[avls[NODE]]->Insert(p, insIdx)) throw(AVLX::avl_insert());
			}
			aDef.sPtr = &node.data;
			if (!nex.avl_store[avls[DATA]]->Insert(p, insIdx)) throw(AVLX::avl_insert());
		}
	}

	for (UINT i = 0; i < rSz; ++i)
	{
		if (roots[i] != INIT_UINT) 
		{
			aDef.sPtr = &roots[i];
	//		if (!nex.avl_store[avls[ROOT]]->Insert(p, insIdx)) throw(AVLX::avl_insert());
			if (!nex.avl_store[avls[NODE]]->Insert(p, insIdx)) throw(AVLX::avl_insert());
		}
	}
	//nex.avl_store[avls[ROOT]]->getAll(rootVec);
	nex.avl_store[avls[NODE]]->getAll(nodeVec); // outputs sorted vectors
	nex.avl_store[avls[DATA]]->getAll(dataVec);
	nex.avl_store[avls[DATADEL]]->getAll(dataDelVec0);

	UINT i = 0, j = 0, k = 0, nSz = nodeVec.size(), dSz = dataVec.size(), 
		delSz = delVec.size(), ddSz = dataDelVec0.size(); //, dIdx = 0;
	if(dSz != nSz) throw(AVLX::data_node()); // one node per data for all trees right?
	if(delSz != ddSz) throw(AVLX::del_data_node()); // one deleted node per one deleted data
	if (delSz == 0) return; // no adjustment possible
	for (i = 0; i < ddSz; ++i) dataDelVec.push_back(*(dataDelVec0[i].sPtr));
	delVec.push_back(INIT_UINT); // greater or equal than all
	dataDelVec.push_back(INIT_UINT);
	// adjusts the root & left & right & data indices in nodeVec & dataVec
	for (i = 0; i < nSz; ++i)
	{
		UINT & 	e = *nodeVec[i].sPtr, // adjust this
				d = delVec[j]; // do not change
		if(e == d) throw(AVLX::node_del());
		while (d < e)
		{
			d = delVec[++j];
			if(e == d) throw(AVLX::node_del());
		}
		e -= j;

		UINT & f = *dataVec[i].sPtr, // adjust this
			 t = dataDelVec[k];  // do not change
		if(f == t) throw(AVLX::data_del());
		while (t < f)
		{
			t = dataDelVec[++k];
			if(f == t) throw(AVLX::data_del());
		}
		f -= k;
	}

	::std::vector<AVLNode>::iterator nIT = node_store.begin();

	typename ::std::vector<T>::iterator dIT = data_store.begin();

	for (i = delSz; i > 0; --i)
	{ // this will delete the vector elements and ensure proper alignement
	  // with the AVL tree node elements left right and data
	  	nIT = node_store.begin();
		::std::advance(nIT, delVec[i - 1]); 
		node_store.erase(nIT);
		//dIdx = dataDelVec[i - 1];
		dIT = data_store.begin();
		::std::advance(dIT, dataDelVec[i - 1]);
		data_store.erase(dIT);
	}
	wholeTrees();
}

template<typename T>
UINT avlNexus<T>::getNext(void)
{
	UINT lst_idx = avl_store.size();
	avl_store.push_back(new AVL<T>(*this, lst_idx));
	roots.push_back(INIT_UINT);
	if (avl_store.size() != roots.size()) throw(AVLX::avl_root());
	return lst_idx;
}

template<typename T>
UINT avlNexus<T>::Size(void)
{
	UINT bc = 0; 

	data_store.shrink_to_fit();
	node_store.shrink_to_fit();
	avl_store.shrink_to_fit();
	roots.shrink_to_fit();
	bc += roots.capacity() * sizeof(UINT);
	bc += data_store.capacity() * sizeof(T);
	bc += node_store.capacity() * sizeof(AVLNode);
	bc += avl_store.capacity() * sizeof(AVL<T>);
	UINT aSz = avl_store.size();
	for (UINT i = 0; i < aSz; ++i)
	{
		bc += avl_store[i]->Size();
	}

	return bc;

}

#pragma pack(pop)
