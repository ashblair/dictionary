#include "first.hpp"
#include "trie.hpp"
#include "dictExceptions.hpp"

using namespace std;

// these statics pertain to The Project Gutenberg eBook of Webster's Unabridged Dictionary
// embedded in the executable from TXT to TXT_END as defined below
//const char * TRIE::TXT = _binary_rsc_Webster_txt_start, * TRIE::TXT_END = _binary_rsc_Webster_txt_end;
//UINT TRIE::OFF_LO = 0x33b, TRIE::OFF_HI = 0x1b98c89;
//vector<TRIE::trieStr> TRIE::WORDS, TRIE::ENTRIES;
//map<UINT, UINT> TRIE::OFFS; // key=word offsets, value = index into WORDS & ENTRIES
extern char TXT_START   asm("_binary_rsc_Webster_txt_start");
extern char TXT_END     asm("_binary_rsc_Webster_txt_end");

extern char DATA_START  asm("_binary_rsc_parse_dat_start");
extern char DATA_END    asm("_binary_rsc_parse_dat_end");

TRIE::entryStr::entryStr(void): start(INIT_UINT), len(0) {}
TRIE::entryStr::entryStr(const entryStr & a): start(a.start), len(a.len) {}
TRIE::entryStr::entryStr(const UINT & s, const UINT & l): start(s), len(l) {/*assert (l <= 0xffff);*/} // limit length to 65535 chars

TRIE::entryStr & TRIE::entryStr::operator= (const entryStr & a)
{
    start = a.start;
    len = a.len;
    return *this;
}

bool TRIE::entryStr::IsEQ(const entryStr & a) const
{
    UINT len0 = len, len1 = a.len;
    if (len0 == len1) return getPivot(a) == len0;
    return false;
}

UINT TRIE::entryStr::getPivot(const entryStr & a) const
{
    UINT i = 0, start0 = start, start1 = a.start,
        len0 = len, len1 = a.len;
    if ((start0== INIT_DATA) || (start1 == INIT_DATA)) return 0;
    char * c0 = &TXT_START + start0, * c1 = &TXT_START + start1;
    for (; i < _MIN(len0, len1); ++i)
    {
        if (*(c0++) != *(c1++)) return i;
    }
    return i;
}

string TRIE::entryStr::toStr(void)
{
    return string(&TXT_START + start, len);
}

string TRIE::entryStr::allToStr(void)
{
    char sBuff[20];
    int cCt = sprintf(sBuff, "%X", start);
    string s = "start at [" + string(sBuff, cCt) + "]";
    cCt = sprintf(sBuff, "%X", len);
    s += " length [" + string(sBuff, cCt) + "]";
    s += " string is [" + toStr() + "]";
    return s;
}


UINT TRIE::entryStr::find(const string & s)
{
    UINT sLen = s.size();
    if (sLen == 0) return len;
    const char * c0 = &TXT_START + start;
    char first = s[0];
    for (UINT i = 0; i < len; ++i)
    {
        if (*c0 == first)
        {
            const char * c2 = c0;
            ++c0;
            UINT j = 1;
            for (; j < _MIN(sLen, len - i); ++j)
            {
                if (*(c0++) != s[j]) break;
            }
            if (j == sLen) return i;
            c0 = c2;

        }
        ++c0;
    }
    return len;
}



TRIE::trieStr::trieStr(void): data(INIT_DATA) {}
TRIE::trieStr::trieStr(const char & c): data((c << LEN_LO_BIT) | CHAR_DATA) {/*assert (c < 0x80);*/}
TRIE::trieStr::trieStr(const trieStr & a): data(a.data) {}

UINT TRIE::trieStr::start(void) const 
{
    UINT s = 0x1ffffff & data;
    return s;
}
UINT TRIE::trieStr::len(void) const 
{
    UINT l = data >> LEN_LO_BIT;
    return l;
}
UINT TRIE::trieStr::getData(const UINT & start, const UINT & len) const 
{
    UINT d = (len << LEN_LO_BIT) | start;
    return d;
}

TRIE::trieStr::trieStr(const UINT & s, const UINT & l): data(getData(s, l)) 
{
    //assert (l < 0x80); // limit length to 127 chars
}

TRIE::trieStr & TRIE::trieStr::operator = (const trieStr & a)
{
    data = a.data;
    return *this;
}

bool TRIE::trieStr::IsEQ(const trieStr & a) const
{
    UINT len0 = len(), len1 = a.len(),
        start0 = start(), start1 = a.start();
    if ((start0 == CHAR_DATA) || (start1 == CHAR_DATA))
    {
        char c0 = static_cast<char>(len0), c1 = static_cast<char>(len1);
        if ((start0 == CHAR_DATA) && (start1 == CHAR_DATA)) return c0 == c1;
        if (start0 == CHAR_DATA) return c0 == a.getChar(0);
        return getChar(0) == c1;
    }
    if (len0 == len1) return getPivot(a) == len0;
    return false;
}

bool TRIE::trieStr::IsLT(const trieStr & a) const
{
    UINT i = 0, len0 = len(), len1 = a.len(), m = _MIN(len0, len1),
        start0 = start(), start1 = a.start();
    if ((start0 == CHAR_DATA) || (start1 == CHAR_DATA))
    {
        char c0 = static_cast<char>(len0), c1 = static_cast<char>(len1);
        if ((start0 == CHAR_DATA) && (start1 == CHAR_DATA)) return c0 < c1;
        if (start0 == CHAR_DATA) return c0 < a.getChar(0);
        return getChar(0) < c1;
    }
    char * pc0 = &TXT_START + start0, * pc1 = &TXT_START + start1;
    for (; i < m; ++i)
    {
        if (*pc0 != *pc1) break;
        ++pc0;
        ++pc1;
    }
    if (i == m) return len0 < len1;
    return *pc0 < *pc1;
}

string TRIE::trieStr::toStr(void)
{
    UINT start0 = start(), len0 = len();
    if (start0 == INIT_UINT) return "string not actualized";
    if (start0 == CHAR_DATA) return string(1, static_cast<char>(len0));
    return string(&TXT_START + start0, len0);
}

string TRIE::trieStr::toLStr(void)
{
    string s = toStr(), r = "";
    for (UINT i = 0; i < s.size(); ++i)
    {
        char c = s[i];
        r += tolower(c);
    }
    return r;
}

string TRIE::trieStr::allToStr(void)
{
    char sBuff[20];
    int cCt = sprintf(sBuff, "%X", start());
    string s = "start at [" + string(sBuff, cCt) + "]";
    cCt = sprintf(sBuff, "%X", len());
    s += " length [" + string(sBuff, cCt) + "]";
    s += " string is [" + toStr() + "]";
    return s;
}

UINT TRIE::trieStr::getPivot(const trieStr & a) const
{
    UINT i = 0, start0 = start(), start1 = a.start(),
        len0 = len(), len1 = a.len();
    if ((start0== INIT_DATA) || (start1 == INIT_DATA)) return 0;
    char * c0 = &TXT_START + start0, * c1 = &TXT_START + start1;
    for (; i < _MIN(len0, len1); ++i)
    {
        if (*(c0++) != *(c1++)) return i;
    }
    return i;
}
        
UINT TRIE::trieStr::getPivot(const char *& a, const UINT strLen) const
{
    UINT i = 0, start0 = start(), len0 = len();
    if (start0 == INIT_DATA) return 0;
    const char * c0 = &TXT_START + start0, * c1 = a;
    for (; i < _MIN(len0, strLen); ++i)
    {
        if (*(c0++) != *(c1++)) return i;
    }
    return i;
}

UINT TRIE::trieStr::find(const char *& a, const UINT slen) 
{
    UINT start0 = start(), len0 = len();
    const char * c0 = &TXT_START + start0, * c1 = a;
    for (UINT i = 0; i < len0; ++i)
    {
        if (*c0 == *c1)
        {
            const char * c2 = c0;
            ++c0; 
            ++c1;
            UINT j = 1;
            for (; j < _MIN(slen, len0 - i); ++j)
            {
                if (*(c0++) != *(c1++)) break;
            }
            if (j == slen) return i;
            c0 = c2;
            c1 = a;
        }
        ++c0;
    }
    return len0;
}

UINT TRIE::trieStr::find(const string & s)
{
    UINT sLen = s.size(), start0 = start(), len0 = len();
    if (sLen == 0) return len0;
    const char * c0 = &TXT_START + start0;
    char first = s[0];
    for (UINT i = 0; i < len0; ++i)
    {
        if (*c0 == first)
        {
            const char * c2 = c0;
            ++c0;
            UINT j = 1;
            for (; j < _MIN(sLen, len0 - i); ++j)
            {
                if (*(c0++) != s[j]) break;
            }
            if (j == sLen) return i;
            c0 = c2;

        }
        ++c0;
    }
    return len0;
}

void TRIE::trieStr::advance(const UINT & i)
{ // alters this trieStr
    UINT start0 = start(), len0 = len();
    if (len0 == 0) throw(DICX::advance());
    start0 += i;
    len0 -= i;
    data = getData(start0, len0);
}

char TRIE::trieStr::getChar(const UINT & i) const 
{
    UINT start0 = start(), len0 = len();
    if (len0 == 0) return END_CHR;
    if (i >= len0) throw(DICX::get_char());
    return *(&TXT_START + start0 + i);
}

TRIE::offsetLookupType::offsetLookupType(void): offset(INIT_UINT) {}
TRIE::offsetLookupType::offsetLookupType(const offsetLookupType & a): offset(a.offset), indices(a.indices) {}
TRIE::offsetLookupType::offsetLookupType(const UINT & o): offset(o) {}
TRIE::offsetLookupType::offsetLookupType(const UINT & o, const vector_c<UINT, UCHAR> & i): offset(o), indices(i) {}
TRIE::offsetLookupType & TRIE::offsetLookupType::operator = (const offsetLookupType & a)
{
    offset = a.offset;
    indices = a.indices;
    return *this;
}
bool TRIE::offsetLookupType::IsEQ(const offsetLookupType & a) const {return offset == a.offset;}
bool TRIE::offsetLookupType::IsLT(const offsetLookupType & a) const {return offset < a.offset;}



void TRIE::parseText(void)
{ // outputs file parse.dat
  // w/ lo-endian UINT count of entries,
  // entry: UINT offset, BYTE length, BYTE wordsperline, BYTE wordIDXperline

    WORDS.clear();
    std::cout << "parsing embedded Webster.txt file\n";
    char * txt = &TXT_START + OFF_LO, * EOLN = txt, * BOLN = txt;
    const char * EOLNCHARS = "\r\n", * ALPHAS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        * WCHARS = " \'-0123456789;ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    UINT vecIdx = 0u, offset = OFF_LO, dSz = 0u;
    bool foundWord = false;
    entryStr lineWord;
    string wrd, matchMe = "---";
    vector<UINT> indices, wpl, ipl; //, ring;
    //vector<string> dups; // @ 2 string gets pushed here
    //map<string, vector<UINT>> wLU; // word lookup
    //map<string, vector<UINT>>::iterator wIT = wLU.begin();

    // going thru text file:
    while (offset < OFF_HI)
    {
        EOLN = strstr(txt, EOLNCHARS);
        BOLN = EOLN + 2;
        entryStr line = entryStr(offset, EOLN - txt);
        wrd = line.toStr();
        foundWord = (line.len > 0) 
                && (line.len == strspn(wrd.c_str(), WCHARS))
                && (strspn(wrd.c_str(), ALPHAS) > 0)
                && !(lineWord == line)
                && (line.find(matchMe) == line.len);
        if (foundWord) 
        {
            lineWord = line;
            while (wrd[wrd.size() - 1] == ';')
            { // mulltiple words per line on more than 1 line like BRAHMANIC & friends:
                EOLN = strstr(BOLN, EOLNCHARS);
                trieStr line2 = trieStr(offset + wrd.size(), EOLN - BOLN);
                BOLN = EOLN + 2;
                string wrd2 = line2.toStr();
                wrd += wrd2;
            }
            size_t wIdx0 = 0, wIdx1 = wrd.find(';');
            UINT wCount = 1;
            vector<trieStr> wrdsperline;
            while (wIdx1 != string::npos)
            {
                wrdsperline.push_back(trieStr(wIdx0 + offset, wIdx1 - wIdx0));
                if (wrd[wIdx1 + 1] == ' ') wIdx0 = wIdx1 + 2;
                else 
                {
                    if (wrd[wIdx1 + 1] != '\r') throw(DICX::last_char());
                    wIdx0 = wIdx1 + 3;
                }
                wIdx1 = wrd.find(';', wIdx0);
                ++wCount;
            }
            if (wCount > 0xff) throw (DICX::words_per_line());
            if (wCount == 1) wrdsperline.push_back(trieStr(line.start, line.len));
            else
            {
                wrdsperline.push_back(trieStr(wIdx0 + offset, wrd.size() - wIdx0));
            }
            //UINT vecTop = vecIdx;
            for (UINT i = 0; i < wCount; ++i)
            {
                trieStr innerword = wrdsperline[i];
                WORDS.push_back(innerword);
                //ring.push_back(vecIdx);  // initialize as if unique
                wrd = innerword.toStr();
                //wIT = wLU.find(wrd);
                //dSz = 1;
                //if (wIT == wLU.end()) wLU[wrd] = {vecIdx};
                //else
                //{
                //    wIT->second.push_back(vecIdx);
                //    dSz = wIT->second.size();
                //    if (dSz == 2) dups.push_back(wrd);
                //}
                //string dupStr = "";
                //if (dSz > 1) dupStr = " dup#[" + to_string(dSz) + "]";
                std::cout << "WORD[" << vecIdx << "]: " << wrd; // << dupStr;
                if (wCount > 1)
                {
                    wpl.push_back(wCount);
                    ipl.push_back(i + 1);
                    indices.push_back(vecIdx);
                    std::cout << " (" << (i + 1) << " / " << wCount << " on line)";
                }
                std::cout << "\n";
                ++vecIdx;
            }
        }
        offset += BOLN - txt;
        txt = BOLN;

    }
    // fix rings for duplicates:
    //dSz = dups.size();
    //for (UINT i = 0; i < dSz; ++i)
    //{
    //    wrd = dups[i];
    //    std::cout << "dup " << i << " " << wrd;
    //    wIT = wLU.find(wrd);
    //    assert (wIT != wLU.end());
    //    vector<UINT> & dupIdx = wIT->second; // because of the way they were added, the elements here will be in order LO to HI
    //    UINT d = dupIdx.size();
    //    std::cout << " w/ " << d << " equal words @< ";
    //    assert (d > 1);
    //    for (UINT j = (d - 1); j > 0; --j)
    //    {
    //        std::cout << dupIdx[j] << " ";
    //        ring[dupIdx[j]] = dupIdx[j - 1];
    //    }
    //    std::cout << dupIdx[0] << " >\n";
    //    ring[dupIdx[0]] = dupIdx[d - 1]; // setting primary to point to highest
    //}
    //cout << "duplicate sets processed: " << dSz << "\n";

    // stream to file:
    bool L2H = lo2hi();
    fstream fs;
    string fNme = "parse.dat";
    fs.open(fNme, fstream::out | fstream::binary);
    if (sizeof(UINT) != 4) throw(DICX::integer());
    UINT i = 0, j = 0, k = 0;
    char * fw = reinterpret_cast<char *>(&vecIdx); if (!L2H) fw += 3;
    for (k = 0; k < 4; ++k) 
    { // 4 byte UINT
        fs.write(fw, 1); 
        if (L2H) ++fw;
        else --fw;
    }

    for (i = 0; i < vecIdx; ++i)
    {
        trieStr s = WORDS[i];
        UINT start0 = s.start(), len0 = s.len();
        fw = reinterpret_cast<char *>(&start0); if (!L2H) fw += 3;
        for (k = 0; k < 4; ++k) 
        { // 4 byte UINT
            fs.write(fw, 1);
            if (L2H) ++fw;
            else --fw;
        }
        fw = reinterpret_cast<char *>(&len0); if (!L2H) fw += 3;
        fs.write(fw, 1);

        if ((j < indices.size()) && (indices[j] == i))
        { // single bytes
            fw = reinterpret_cast<char *>(&wpl[j]); if (!L2H) fw += 3;
            fs.write(fw, 1);
            fw = reinterpret_cast<char *>(&ipl[j]); if (!L2H) fw += 3;
            fs.write(fw, 1);
            ++j;
        }
        else
        { // single bytes
            k = 1;
            fw = reinterpret_cast<char *>(&k); if (!L2H) fw += 3;
            fs.write(fw, 1);
            fs.write(fw, 1);
        }
        //fw = reinterpret_cast<char *>(&ring[i]); if (!L2H) fw += 3;
        //for (k = 0; k < 4; ++k) 
        //{ // 4 byte UINT
        //    fs.write(fw, 1);
        //    if (L2H) ++fw;
        //    else --fw;
        //}

    }
    fs.close();
    std::cout << fNme << " written to working directory.\nMove to resource directory & re-run program normally.\n";
}


TRIE::trieEdge::trieEdge(void): sfx(INIT_UINT) {}
TRIE::trieEdge::trieEdge(const trieEdge & a): pfx(a.pfx), sfx(a.sfx) {}
TRIE::trieEdge::trieEdge(const char & c): pfx(trieStr(c)), sfx(INIT_UINT) {}
TRIE::trieEdge::trieEdge(const trieStr & s): pfx(s), sfx(INIT_UINT) {}
TRIE::trieEdge::trieEdge(const trieStr & p, const UINT & s): pfx(p), sfx(s) {}
//TRIE::trieEdge::trieEdge(const trieStr & p, const trieNode & s): pfx(p), sfx(TRIE_NODES.size()) {TRIE_NODES.push_back(s);}
TRIE::trieEdge::~trieEdge(void) {}
TRIE::trieEdge & TRIE::trieEdge::operator =(const trieEdge & a)
{
    pfx = a.pfx;
    sfx = a.sfx;
    return *this;
}
bool TRIE::trieEdge::IsEQ(const trieEdge & a) const {return pfx == a.pfx;}
bool TRIE::trieEdge::IsLT(const trieEdge & a) const {return pfx < a.pfx;}
bool TRIE::trieEdge::findChar(char c)
{
    if (pfx.len() > 0)
    {
        if (pfx.getChar(0) == c)
        {
            pfx.advance(1);
            return true;
        }
        return false;
    }
    if (sfx == INIT_UINT) return false;
    trieEdge e = trieEdge(c);
    UINT dIdx = 0;
    if (TRIE_NODES[sfx].getEdge(e, dIdx))
    {
        *this = e;
        pfx.advance(1);
        return true;
    }
    return false;
}

bool TRIE::trieEdge::findAny(vector<trieEdge> & eVec)
{
    eVec.clear();
    if (pfx.len() > 0)
    {
        eVec.push_back(*this);
        return true;
    }
    else
    {
        if (sfx == INIT_UINT) return false;
        else
        {
            eVec = TRIE_NODES[sfx].getEdges();
            return eVec.size() > 0;
        }
    }
}

bool TRIE::trieEdge::findAny1(vector<trieEdge> & eVec)
{
    eVec.clear();
    if (pfx.len() > 0)
    {
        trieEdge e = *this;
        e.pfx.advance(1);
        if (pfx.len() > 0)
        {
            eVec.push_back(e);
            return true;
        }
        else
        {
            if (sfx == INIT_UINT) return false;
            else
            {
                eVec = TRIE_NODES[sfx].getEdges();
                return eVec.size() > 0;
            }
        }
    }
    else
    {
        if (sfx == INIT_UINT) return false;
        else
        {
            eVec = TRIE_NODES[sfx].getEdges1();
            return eVec.size() > 0;
        }
    }

}

bool TRIE::trieEdge::findEnd(void)
{ // sets this edge to the appropriate edge for offset searching if end is found
    if (pfx.len() > 0) return false;
    if (sfx != INIT_UINT)
    {
        trieEdge e = trieEdge(END_CHR);
        UINT dIdx = 0;
        if (!TRIE_NODES[sfx].getEdge(e, dIdx)) return false;
        *this = e;
    }
    return true;
}

bool TRIE::trieEdge::atEnd(void)
{
    return (pfx.len() == 0) && (sfx == INIT_UINT);
}

bool TRIE::trieEdge::actualized(void) const
{
    return pfx.start() != INIT_DATA;
}

void TRIE::trieEdge::add_idxs(vector<vector_c<UINT, UCHAR>> & res)
{
    //map<UINT, vector<UINT>>::iterator 
    //    iIT = TRIE::OFFS.lower_bound(pfx.start());
    offsetLU oLU = offsetLU(pfx.start());
    UINT dIdx = 0;
    bool foundLE = getOFFS().GetLE(oLU, dIdx);
    if (!foundLE) throw(DICX::offset_not_found());
    vector_c<UINT, UCHAR>
        & iVec = oLU.indices; // iIT->second;
    
    res.push_back(iVec);

}

void TRIE::trieEdge::add_tree(vector<vector_c<UINT, UCHAR>> & res)
{
    if (pfx.len() > 0) pfx.advance(pfx.len());
    if (sfx == INIT_UINT) add_idxs(res);
    else
    {
        vector<trieEdge> any;
        findAny(any);
        for (UINT i = 0; i < any.size(); ++i)
        {
            any[i].add_tree(res);
        }
    }

}

void TRIE::trieEdge::search(
        const trieEdge & swcPTR,
        vector<vector_c<UINT, UCHAR>> & res, 
        const char *& patt, 
        const unsigned char pattLEN, 
        unsigned char pattIDX, 
        unsigned char wcIDX)
{
    if (pattLEN == 0) return;
    vector<trieEdge> any;
    trieEdge a;
    UINT i;
    char c = patt[pattIDX];

    if (pattLEN == pattIDX)
    {
        if (patt[pattLEN - 1] == S_WC) add_tree(res);
        else if (findEnd()) add_idxs(res);
    }
    else
    {
        if (findChar(c))
        {
            search(swcPTR, res, patt, pattLEN, pattIDX + 1, wcIDX);
        }
        else if (c == C_WC)
        {
            if (findAny1(any))
            {
                for (i = 0; i < any.size(); ++i)
                {
                    *this = any[i];
                    search(swcPTR, res, patt, pattLEN, pattIDX + 1, wcIDX);
                }
            }
        }
        else if (c == S_WC)
        {
            if (findAny(any))
            {
                for (i = 0; i < any.size(); ++i)
                {
                    *this = any[i];
                    search(any[i], res, patt, pattLEN, pattIDX + 1, pattIDX);
                }
            }
        }
        else if (swcPTR.actualized())
        {
            *this = swcPTR;
            if (findAny1(any))
            {
                for (i = 0; i < any.size(); ++i)
                {
                    *this = any[i];
                    search(any[i], res, patt, pattLEN, wcIDX + 1, wcIDX);
                }
            }
        }
        else return;
    }
}


TRIE::trieNode::trieNode(void): avlID(INIT_UINT) {}
TRIE::trieNode::trieNode(const trieNode & a): avlID(a.avlID) {} // pointer copy only
TRIE::trieNode::~trieNode(void) {}

/*void TRIE::trieNode::delEdges(void)
{
    if (edges != NULL)
    {
        vector<trieEdge> eVec;
        edges->getAll(eVec);
        UINT eSz = eVec.size();
        for (UINT i = 0; i < eSz; ++i)
        {
            trieEdge e = eVec[i];
            if (e.sfx != NULL) e.sfx->delEdges();
        }
        //edges->clear();
        delete edges;
        edges = NULL;
    }
}*/

bool TRIE::trieNode:: getEdge(trieEdge & e, UINT & dIdx)
{
    if (avlID == INIT_UINT) return false;
    return getEdgeAVL(avlID).findData(e, dIdx);

}

vector<TRIE::trieEdge> TRIE::trieNode::getEdges(void)
{
    vector<trieEdge> r;
    if (avlID == INIT_UINT) return r;
    getEdgeAVL(avlID).getAll(r);
    //for (map<char, trieEdge>::iterator eIT = edges->begin(); eIT != edges->end(); ++eIT) r.push_back(eIT->second);
    return r;
}

vector<TRIE::trieEdge> TRIE::trieNode::getEdges1(void)
{
    vector<trieEdge> eVec0, eVec1;
    if (avlID == INIT_UINT) return eVec0;
    getEdgeAVL(avlID).getAll(eVec0);
    UINT eSz = eVec0.size();
    for (UINT i = 0; i < eSz; ++i)
    {
        trieEdge e = eVec0[i];
        if (!e.atEnd()) 
        {
            e.pfx.advance(1); // will this update the vector element?
            eVec1.push_back(e);
        }
    }
    return eVec1;
}

void TRIE::trieNode::add_idx(const UINT & primary, const UINT & dup)
{ // primary and dup are offsets

    //map<UINT, vector<UINT>>::iterator 
    //    pIT = TRIE::OFFS.lower_bound(primary),
    //    dIT = TRIE::OFFS.lower_bound(dup);
    offsetLU oLUp = offsetLU(primary), oLUd = offsetLU(dup);
    UINT dIDXp = 0, dIDXd = 0;
    bool foundPrimary = getOFFS().GetLE(oLUp, dIDXp), foundDup = getOFFS().GetLE(oLUd, dIDXd);
    if ((!foundPrimary) || (!foundDup)) throw(DICX::offset_not_found());
    vector_c<UINT, UCHAR>
        pVec = oLUp.indices, //pIT->second,
        dVec = oLUd.indices; //dIT->second;
    if (dVec.size() > 1) throw(DICX::duplicate_indices());  // duplicate should only ever have it's own index in the vector
                                // the only time it will be looked up is once here
    UINT dIdx = dVec[0];
    
    pVec.push(dIdx);
    OFFS->data_store[dIDXp].indices = pVec;
    ++reptCount;
    //UINT * start = OFFS2.GetLE(primary);
    //std::cout << "in add_idx AVL start[" << *start << "] map start[" << WORDS[pVec[0]].start() << "]\n";
    //static UINT dupCt = 0;
    //if (pVec.size() == 2)
    //{
    //    std::cout << "[" << dupCt << "] duplicates found for " << TRIE::WORDS[pVec[0]].toStr() << "\n";
    //    ++dupCt;
    //}
}

void TRIE::trieNode::insert(trieEdge eIn)
{
    trieEdge eBuff; // = trieEdge(s);
    ptrieEdge p = NULL;
    //ptrieNode nd = NULL;
    UINT pvt = 0u, nodeIdx = 0u, dataIdx = 0u;
    char f = ' ';
    bool iChk = false;

    //std::cout << "|" << avlID << " " << eIn.pfx.toStr() << "|";
    if (eIn.pfx.len() == 0)
    {
        eBuff = trieEdge(END_CHR);
        if (getEdge(eBuff, dataIdx)) add_idx(eBuff.pfx.start(), eIn.pfx.start()); // duplicate string detected
        else
        { // inserting end of string:
            if (avlID == INIT_UINT) avlID = EDGE_NEXUS->getNext(); //edges = new AVL<trieEdge, UCHAR>; //map<char, trieEdge>;
            p = &eIn;
            iChk = getEdgeAVL(avlID).Insert(p, dataIdx); 
            if (!iChk) throw(DICX::insert());
            ++edgeCount;
            //std::cout << "l";
        }
    }
    else
    {
        f = eIn.pfx.getChar(0);
        eBuff = trieEdge(f);
        if (getEdge(eBuff, dataIdx))
        { // eBuff is the edge beginning with f so parse eIn.pfx with eBuff.pfx:
            pvt = eIn.pfx.getPivot(eBuff.pfx);
            if (pvt == 0) throw(DICX::pivot());
            if ((eBuff.sfx == INIT_UINT) || (pvt < eBuff.pfx.len()))
            { // prefix needs to be divided:
                nodeIdx = TRIE_NODES.size(); TRIE_NODES.push_back(trieNode()); ++nodeCount;

                // new node inserts:
                eBuff.pfx.advance(pvt); eIn.pfx.advance(pvt);
                TRIE_NODES[nodeIdx].insert(eBuff); //trieStr(e.pfx.start() + pvt, e.pfx.len() - pvt), e.sfx);
                TRIE_NODES[nodeIdx].insert(eIn); //trieStr(s.start() + pvt, s.len() - pvt));

                // old node cleanup:
                eBuff.pfx.data = eBuff.pfx.getData(eBuff.pfx.start() - pvt, pvt);
                eBuff.sfx = nodeIdx;
                EDGE_NEXUS->data_store[dataIdx] = eBuff;
            }
            else
            { // pvt should equal pfx.len which is probably 1 here
                if (pvt != eBuff.pfx.len()) throw(DICX::pivot());
                eIn.pfx.advance(pvt);
                TRIE_NODES[eBuff.sfx].insert(eIn); //trieStr(s.start() + pvt, s.len() - pvt));
            }
        }
        else
        { // no edge begins with f so add it:
            if (avlID == INIT_UINT) avlID = EDGE_NEXUS->getNext(); //edges = new AVL<trieEdge, UCHAR>; //map<char, trieEdge>;
            //(*edges)[f] = trieEdge(s);
            p = &eIn; //e = trieEdge(s); // e initialized w/ trieStr s above
            iChk = getEdgeAVL(avlID).Insert(p, dataIdx); 
            if (!iChk) throw(DICX::insert());
            ++edgeCount;
            //std::cout << "+";
        }
    }
}

/*void TRIE::trieNode::insert(const trieStr & s, const UINT & n)
{
    if (avlID == INIT_UINT) avlID = EDGE_NEXUS->getNext(); //edges = new AVL<trieEdge, UCHAR>; //map<char, trieEdge>;
    //char f = s.getChar(0);
    //if (s.len() == 0) f = END_CHR;

    trieEdge e = trieEdge(s, n);
    ptrieEdge p = &e;
    bool iChk = getEdgeAVL(avlID).Insert(p);
    assert(iChk);
    
    //pair<map<char, trieEdge>::iterator, bool> ret =
    //    edges->insert( pair<char, trieEdge>(f, trieEdge(s)));

    //assert (ret.second);
    //ret.first->second.sfx = n;
    ++edgeCount;
}*/

UINT TRIE::trieNode::getAVLsize(void)
{
    if (avlID == INIT_UINT) return 0;
    return getEdgeAVL(avlID).Size();
}

AVL<TRIE::offsetLU> & TRIE::getOFFS(void)
{
    return *OFFS->avl_store[offsetAVLidx];
   
}

AVL<TRIE::trieEdge> & TRIE::getEdgeAVL(const UINT & i)
{
    return *EDGE_NEXUS->avl_store[i];
}

TRIE::trieTree::trieTree(void)
{
    nodeCount = 0;
    edgeCount = 0;
    reptCount = 0;
    root = 0;
    TRIE_NODES.clear();
    TRIE_NODES.push_back(trieNode());
    ++nodeCount;
    getData();
}

TRIE::trieTree::~trieTree(void) 
{
    WORDS.clear();
    ENTRIES.clear();
    
    if (OFFS != NULL) delete OFFS;
    if (EDGE_NEXUS != NULL) delete EDGE_NEXUS;

    TRIE_NODES.clear();
}

void TRIE::trieTree::getData(void)
{ // uses data from parse.dat to initialize data structures root, WORDS, ENTRIES & OFFS
  // w/ lo-endian UINT count of entries,
  // entry: UINT offset, USHRT length, BYTE wordsperline, BYTE wordIDXperline
  // for multi word lines the last 2 entries for N words will be N 1 .. N 2 ... N 3 ... ... N N

    WORDS.clear();
    ENTRIES.clear();
    //OFFS.clear();
    if (OFFS != NULL) throw (DICX::offsetAVL());
    OFFS = new avlNexus<offsetLU>;
    UINT oIdx = OFFS->getNext();
    //assert (oIdx == 0);
    AVL<offsetLU> & offs = *OFFS->avl_store[oIdx];
    offsetAVLidx = oIdx;

    EDGE_NEXUS = new avlNexus<trieEdge>;

    //trieNode & rNode = TRIE_NODES[root];
    bool L2H = lo2hi();
    //std::cout << "computer is " << (L2H? "little": "big") << "endian\n";
    UINT wrdCount = 0, start = 0, len = 0, wpl = 0, ipl = 0, wpl0 = 0, ipl0 = 0, //ring = 0,
        i = 0, j = 0, k = 0, maxIdx = 0, maxLen = 0, entryLen = 0, dIdx = 0;
    offsetLU oLU;
    oLU.indices.push(0);
    poffsetLU poLU = &oLU;
    //UINT * pStart = &start;
    //string matchMe = "--";
    //vector<UINT> dashers;
    char * dat = &DATA_START, * rd = reinterpret_cast<char *>(&wrdCount),
        * rd_start = reinterpret_cast<char *>(&start),
        * rd_len = reinterpret_cast<char *>(&len),
        * rd_wpl = reinterpret_cast<char *>(&wpl),
        * rd_ipl = reinterpret_cast<char *>(&ipl);
        //* rd_rng = reinterpret_cast<char *>(&ring);
    if (!L2H)
    {
        rd += 3; rd_start += 3; rd_len += 3; rd_wpl += 3; rd_ipl += 3; //rd_rng += 3;
    }
    for (k = 0; k < 4; ++k) 
    {
        *rd = *(dat++);
        if (L2H) ++rd;
        else --rd;
    }
    //std::cout << "word count " << wrdCount << "\n";
    //map<UINT, UINT> dupLU;
    //map<UINT, UINT>::iterator dIT = dupLU.begin();
    //map<string, vector<UINT>> wrdoffsets;
    //vector<string> wrdprblms;
    for (i = 0; i < wrdCount; ++i)
    {
        rd = rd_start; 
        for(k = 0; k < 4; ++k) 
        {
            *rd = *(dat++);
            if (L2H) ++rd;
            else --rd;
        }
        
        rd = rd_len; *rd = *(dat++);
        //for (k = 0; k < 2; ++k) 
        //{
        //    *rd = *(dat++);
        //    if (L2H) ++rd;
        //    else --rd;
        //}

        rd = rd_wpl; *rd = *(dat++);
        rd = rd_ipl; *rd = *(dat++);

        //rd = rd_rng; 
        //for(k = 0; k < 4; ++k) 
        //{
        //    *rd = *(dat++);
        //    if (L2H) ++rd;
        //    else --rd;
        //}

        //if (ring == i) OFFS[start + len] = {i}; // this word is unique
        //else dupLU[i] = ring;
        //OFFS[start + len] = {i};
        oLU.offset = start; oLU.indices[0] = i;
        if (!offs.Insert(poLU, dIdx)) std::cout << "Error: offset [" << start << "] already in OFFS.\n";
        trieStr s = trieStr(start, len);
        WORDS.push_back(s);
        //if (s.find(matchMe) < len) dashers.push_back(i);
        if (i == 0) maxLen = len;
        else
        {
            if (len > maxLen)
            {
                maxIdx = i;
                maxLen = len;
            }
        }

        //std::cout << "DATA[" << i << "]: " << WORDS[i].allToStr() << "\n";
        //pair<map<string, vector<UINT>>::iterator, bool> ret = wrdoffsets.insert(pair<string, vector<UINT>>(s.toStr(), {start}));
        //if (!ret.second)
        //{
        //    ret.first->second.push_back(start);
        //    if (ret.first->second.size() == 2) wrdprblms.push_back(ret.first->first);
        //}
        //if (ring >= i) root->insert(s); // uniques and primaries only
        TRIE_NODES[root].insert(s);
        //std::cout << "\n";
        ENTRIES.push_back(entryStr(WORDS[i - ipl + 1].start(), 0)); // initialized to empty string
        if ((i > 0) && (ipl0 == wpl0))
        {
            entryLen = start - ENTRIES[i - 1].start;
            for (k = 0; k < wpl0; ++k) 
            {
                ENTRIES[i - 1 - k].len = entryLen;
                //std::cout << "entry[" << (i - 1 - k) << "] = {start[" << ENTRIES[i - 1 - k].start << "] len[" << ENTRIES[i - 1 - k].len << "]}\n";
            }
        }

        wpl0 = wpl;
        ipl0 = ipl;
    }
    if (i > 0)
    { // this will fix the last entry:
        entryLen = OFF_HI - ENTRIES[i - 1].start;
        for (k = 0; k < wpl0; ++k) ENTRIES[i - 1 - k].len = entryLen;
    }
    //std::cout << "maximum word:[" << maxIdx << "] (" << WORDS[maxIdx].toStr() << ") w/ length{" << maxLen << "}\n";
    //std::cout << "nodes[" << nodeCount << "] edges[" << edgeCount << "] reps [" << reptCount << "]\n";
    //std::cout << "map sizes approximately:\n";
    //std::cout << "1) OFFS: map<UINT, vector<UINT>> " << sizeof(map<UINT, vector<UINT>>) << " + " << OFFS.size() 
    //    << " * (" << sizeof(typename map<UINT, vector<UINT>>::node_type) << " + " << sizeof(pair<UINT, vector<UINT>>) << ")\n";
    //std::cout << "2) edges: map<char, trieEdge> " << sizeof(map<char, trieEdge>) << " + factor * (" 
    //    << sizeof(typename map<char, trieEdge>::node_type) << " + " << sizeof(pair<char, trieEdge>) << ")\n";
    //char nullStr[100];
    //int cCt = sprintf(nullStr, "UCHAR null? 0x%X UINT null? 0x%X", static_cast<UCHAR>(-1), static_cast<UINT>(-1));
    //string nullS = string(nullStr, cCt);
    //std::cout << nullS << "\n";
    //showSizes();
    //std::cout << "AVL<trieEdge, UCHAR> sizes: <=41 * node[" << sizeof(AVLNode<UCHAR>) << " ->vec " << sizeof(vector<AVLNode<UCHAR>>) 
    //    << "] + <=6 * path[" << sizeof(AVLPath<UCHAR>) << " ->vec " << sizeof(vector<AVLPath<UCHAR>>) 
    //    << "] + <=41 * data [" << sizeof(trieEdge) << " ->vec " << sizeof(vector<trieEdge>) << "] + 1 for the AVL root\n";
    //showSizes();
    //std::cout << dashers.size() << " words matching " << matchMe << "\n";
    //for (i = 0; i < dashers.size(); ++i) std::cout << "[" << i << " - " << dashers[i] << "] " << WORDS[dashers[i]].toStr() << "\n";
    /*std::cout << "found " << dupLU.size() << " duplicates.\n";
    UINT primaryCount = 0;
    while (dupLU.size() > 0)
    { // this will add the indexes to OFFS map
      // which s/b useful to get complete entries from the dictionary
        dIT = dupLU.begin();
        UINT primary = dIT->first, oth = dIT->second;
        ++primaryCount;
        dupLU.erase(dIT);
        vector<UINT> dupIdxs = {primary};
        while(oth != primary)
        {
            dupIdxs.push_back(oth);
            dIT = dupLU.find(oth);
            assert (dIT != dupLU.end());
            oth = dIT->second;
            dupLU.erase(dIT);
        }
        sort(dupIdxs.begin(), dupIdxs.end());
        OFFS[primary] = dupIdxs;
    }
    std::cout << "with " << primaryCount << " primaries.\n";*/
    //for (i = 0; i < wrdprblms.size(); ++i)
    //{
    //    string s = wrdprblms[i];
    //    map<string, vector<UINT>>::iterator oIT = wrdoffsets.find(s);
    //    assert (oIT != wrdoffsets.end());
    //    vector<UINT> & offs = oIT->second;
    //    std::cout << "dup[" << i << "] " << s << " offsets <";
    //    for (j = 0; j < offs.size(); ++j) 
    //    {
    //        std::cout << offs[j]; 
    //        if (j < (offs.size() - 1)) std::cout << ", ";
    //    }
    //    std::cout << ">\n";
    //}
}

string TRIE::trieTree::debug_pattern(const Glib::ustring & sIn)
{ // replaces strings of string wildcase characters with one
  // and makes all characters upper case
  // E.G. HI*******there**** will become HI*THERE* if the string wildcard character is *
    
    string rebuild = "";
    UINT i = 0, pSz = sIn.size(), j = 0;
    while (i < pSz)
    {
        char c = toupper(sIn[i]); // all words in the dictionary are upper case
        if (c == S_WC)
        {
            while((i < pSz) && (sIn[i + 1] == S_WC)) ++i;
        }
        rebuild += c;
        ++i;
    }
    return rebuild;
}

void TRIE::trieTree::search(void)
{
    trieEdge e, swcPOS;
    const char * test = pattern.c_str();
    unsigned char ptt_len = pattern.size(), ptt_idx = 0, swc_pos = 0;
    results.clear();
    //wrdvec.clear();
    resultString = "";
    e.sfx = root;
    e.search(swcPOS, results, test, ptt_len, ptt_idx, swc_pos);

    UINT rSz = results.size();
    //std::cout << rSz << " search results w/ pattern[" << test << "]\n";
    //e.sfx = NULL;
    //swcPOS.sfx = NULL;
    for (UINT i = 0; i < results.size(); ++i)
    {
        vector_c<UINT, UCHAR> & idxs = results[i];
        string s = WORDS[idxs[0]].toStr();
        //wrdvec.push_back(s);
        resultString += s;
        resultString += "\r\n";
    }
    //std::cout << "done string.\n";
    //for (UINT i = 0; i < results.size(); ++i)
    //{
    //    vector<UINT> & idxs = results[i];
    //    for (UINT j = 0; j < idxs.size(); ++j)
    //    {
    //        std::cout << "[" << i << "-" << j << "] " << WORDS[idxs[j]].toStr() << " {" << ENTRIES[idxs[j]].toStr() << "}\n";
    //    }
    //}
}

Glib::ustring TRIE::trieTree::getEntry(const int & idx)
{
    Glib::ustring r = "";
    UINT rSz = results.size();
    if ((idx < 0) || (idx >= rSz)) return r;
    vector_c<UINT, UCHAR> & idxs = results[idx];
    for (UCHAR j = 0; j < idxs.size(); ++j)
    {
        entryStr es = ENTRIES[idxs[j]];
        r += es.toStr();
        //std::cout << es.allToStr() << "\n";
    }
    return r;
}

string TRIE::trieTree::getWord(const int & idx)
{
    return WORDS[results[idx][0]].toLStr();
}

void TRIE::trieTree::showSize(ptrieNode nd)
{
    UINT bCt = 0, nCt = 0;
    UINT ctOfByte = nd->getAVLsize();
    bCt += ctOfByte;
    cout << "node [" << nCt++ << "] AVL size [" << ctOfByte << "] total [" << bCt << "]\n";
}

void TRIE::trieTree::showSizes(void)
{
    UINT bCt = 0, iCt = 0;
    std::cout << "sizes:\n";
    iCt = WORDS.capacity() * sizeof(trieStr) + sizeof(WORDS);
    bCt += iCt;
    std::cout << "WORDS is " << WORDS.capacity() << " * sizeof(trieStr) " << sizeof(trieStr) << 
        " + sizeof vector " << sizeof(WORDS) << " = " << iCt << "\n";
    iCt = ENTRIES.capacity() * sizeof(entryStr) + sizeof(ENTRIES);
    bCt += iCt;
    std::cout << "ENTRIES is " << ENTRIES.capacity() << " * sizeof(entryStr) " << sizeof(entryStr) <<
        " + sizeof vector " << sizeof(ENTRIES) << " = " << iCt << "\n";
    iCt = TRIE_NODES.capacity() * sizeof(trieNode) + sizeof(TRIE_NODES);
    bCt += iCt;
    std::cout << "TRIE_NODES is " << TRIE_NODES.capacity() << " * sizeof(trieNode) " << sizeof(trieNode) <<
        " + sizeof vector " << sizeof(TRIE_NODES) << " = " << iCt << "\n";
    //iCt = sizeof(OFFS) + sizeof(*OFFS) + OFFS->Size();
    iCt = OFFS->Size();
    //bCt += iCt;
    std::cout << "OFFS is ptr size " << sizeof(OFFS) << " + avlNexus size " << sizeof(*OFFS) << 
        " + sum of vectors w/in " << iCt << " = " << (iCt + sizeof(OFFS) + sizeof(*OFFS)) << "\n";
    iCt += sizeof(OFFS) + sizeof(*OFFS);
    bCt += iCt;
    std::cout << "detail:\n";
    getOFFS().detailSizes();
    std::cout << "\n\n";
    iCt = EDGE_NEXUS->Size();
    std::cout << "EDGE_NEXUS is ptr size " << sizeof(EDGE_NEXUS) << " + avlNexus size " << sizeof(*EDGE_NEXUS) << 
        " + sum of vectors w/in " << iCt << " = " << (iCt + sizeof(EDGE_NEXUS) + sizeof(*EDGE_NEXUS)) << "\n";
    iCt += sizeof(EDGE_NEXUS) + sizeof(*EDGE_NEXUS);
    bCt += iCt;
    std::cout << "Total: " << bCt << "\n";
    //std::cout << "details:\n";
    //UINT eSz = EDGE_NEXUS->avl_store.size(), tot = 0;
    //for (UINT i = 0; i < eSz; ++i)
    //{
    //    tot += getEdgeAVL(i).detailSizes();
    //}
    //std::cout << "total edge AVL: " << tot << "\n";

}
