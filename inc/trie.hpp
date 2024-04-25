#pragma once
#include "AVL_c.hpp"
// these are offsets in the embedded dictionary text file:
#define OFF_LO 0x33b
#define OFF_HI 0x1b98c89
#define LEN_LO_BIT 25


namespace TRIE
{

    typedef class entryStr
    {
    public:
        entryStr(void);
        entryStr(const entryStr&);
        entryStr(const UINT &, const UINT &);
        entryStr & operator = (const entryStr &);
        bool IsEQ(const entryStr &) const;
        friend bool operator == (const entryStr & a, const entryStr & b) {return a.IsEQ(b);}

        ::std::string toStr(void);
        ::std::string allToStr(void);

        UINT getPivot(const entryStr &) const;
        UINT find(const ::std::string &);

        UINT start, len;

    } * pentryStr;

    typedef class trieStr
    {
    public:
        trieStr(void);
        trieStr(const char &);
        trieStr(const trieStr &);
        UINT start(void) const;
        UINT len(void) const;
        UINT getData(const UINT &, const UINT &) const;
        trieStr(const UINT &, const UINT &);

        trieStr & operator =(const trieStr &);
        bool IsEQ(const trieStr &) const;
        friend bool operator == (const trieStr & a, const trieStr & b) {return a.IsEQ(b);}
        bool IsLT(const trieStr &) const;
        friend bool operator < (const trieStr & a, const trieStr & b) {return a.IsLT(b);}
        ::std::string toStr(void);
        ::std::string toLStr(void);
        ::std::string allToStr(void);

        UINT getPivot(const trieStr &) const;
        UINT getPivot(const char *&, const UINT) const;
        UINT find(const char *&, const UINT);
        UINT find(const ::std::string &);
        void advance(const UINT &);
        char getChar(const UINT &) const;

        UINT data;

    } * ptrieStr;

    typedef struct offsetLookupType
    {
        offsetLookupType(void);
        offsetLookupType(const offsetLookupType &);
        offsetLookupType(const UINT &);
        offsetLookupType(const UINT &, const vector_c<UINT, UCHAR> &);
        offsetLookupType & operator = (const offsetLookupType &);
        bool IsEQ(const offsetLookupType &) const;
        friend bool operator == (const offsetLookupType & a, const offsetLookupType & b) {return a.IsEQ(b);}
        bool IsLT(const offsetLookupType &) const;
        friend bool operator < (const offsetLookupType & a, const offsetLookupType & b) {return a.IsLT(b);}

        UINT offset;
        vector_c<UINT, UCHAR> indices;
        //::std::vector<UINT> indices;

    } offsetLU, * poffsetLU;

    static ::std::vector<trieStr> WORDS;
    static ::std::vector<entryStr> ENTRIES;
    //static ::std::map<UINT, ::std::vector<UINT>> OFFS; // key=word offsets, value = indices into WORDS & ENTRIES of all duplicates
    static avlNexus<offsetLU> * OFFS = NULL;

    static const char END_CHR = '\0', C_WC = '?', S_WC = '*';
    static UINT edgeCount = 0, nodeCount = 0, reptCount = 0, offsetAVLidx = 0;

    static AVL<offsetLU> & getOFFS(void);

    void parseText(void);

    typedef class trieNode * ptrieNode;

    typedef class trieEdge
    {
    public:
        trieEdge(void);
        trieEdge(const trieEdge &);
        trieEdge(const trieStr &);
        trieEdge(const char &);
        trieEdge(const trieStr &, const UINT &);
        //trieEdge(const trieStr &, const trieNode &);
        ~trieEdge(void);

        trieEdge & operator =(const trieEdge &);

        bool IsEQ(const trieEdge &) const;
        friend bool operator == (const trieEdge & a, const trieEdge & b) {return a.IsEQ(b);}
        bool IsLT(const trieEdge &) const;
        friend bool operator < (const trieEdge & a, const trieEdge & b) {return a.IsLT(b);}

        bool findChar(char);
        bool findAny(::std::vector<trieEdge> &);
        bool findAny1(::std::vector<trieEdge>&);
        bool findEnd(void);
        bool atEnd(void);
        bool actualized(void) const;
        void add_idxs(::std::vector<vector_c<UINT, UCHAR>>&);
        void add_tree(::std::vector<vector_c<UINT, UCHAR>>&);
        void search(const trieEdge &, ::std::vector<vector_c<UINT, UCHAR>> &, const char *&, const unsigned char, unsigned char, unsigned char);

        trieStr pfx;
        UINT sfx; // index into TRIE_NODES

    } * ptrieEdge;

    static avlNexus<trieEdge> * EDGE_NEXUS = NULL;
    static AVL<trieEdge> & getEdgeAVL(const UINT &);

    class trieNode
    {
        //::std::map<char, trieEdge> * edges;
        //pAVL<trieEdge> edges;
        UINT avlID;
    public:
        trieNode(void);
        trieNode(const trieNode &);
        ~trieNode(void);

        //void delEdges(void);
        bool getEdge(trieEdge &, UINT &);
        ::std::vector<trieEdge> getEdges(void);
        ::std::vector<trieEdge> getEdges1(void);
        void add_idx(const UINT &, const UINT &);
        void insert(trieEdge);
        //void insert(const trieStr &, const UINT &);
        UINT getAVLsize(void);

    };

    static ::std::vector<trieNode> TRIE_NODES;

    class trieTree
    {
    public:
        trieTree(void);
        ~trieTree(void);

        void getData(void);
        ::std::string debug_pattern(const ::Glib::ustring &);
        void search(void);
        ::Glib::ustring getEntry(const int &);
        ::std::string getWord(const int &);
        void showSize(ptrieNode);
        void showSizes(void);

        UINT root;
        trieEdge srchEdge;
        ::std::string pattern, resultString;
        ::std::vector<vector_c<UINT, UCHAR>> results;
        //::std::vector<::std::string> wrdvec;
    };
};