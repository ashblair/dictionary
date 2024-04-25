#pragma once

class glade_extract;
typedef class glade_extract GEXT;
typedef GEXT * pGEXT;

namespace TRIE
{
	class trieTree;
};

class GUI
{
public:
	GUI(void);
	~GUI(void);

	void ResetGUI(pGEXT);
	//void HandleMainDlgRealize(void);
	//void HandleMainDlgSize(Gtk::Allocation&);
	void HandledictionaryDlgCloseButton(void);
	void HandleMessageBoxCloseButton(void);
	void HandlepatternEntry(void);
	bool HandleresultsListClick(GdkEventButton *);
	bool HandleresultsListMouse(GdkEventMotion *);
	bool HandleresultsListLeave(GdkEventCrossing *);
	//void HandleresultsListRealize(void);
	//bool HandledictionaryTextViewClick(GdkEventButton *);
	//void HandledictionaryTextViewRealize(void);

	pGEXT pG;
	Gtk::Dialog * MainDlg, * MessageBox;
	Gtk::Button * dictionaryDlgCloseButton, *MessageCloseButton;
	Gtk::Label * dictionaryLabel, * MessageLabel, *patternLabel;
	Gtk::Entry * patternEntry;
	Gtk::TextView * dictionaryTextView, * resultsList;
	Gtk::Label * statusLabel;

	TRIE::trieTree * pTree;

	Glib::RefPtr<Gtk::TextBuffer> txtView, matchView;
	Glib::RefPtr<Gdk::Window> rlWin;
	Glib::RefPtr<Gdk::Cursor> rlArrow;
	Glib::RefPtr<Gtk::TextTag> gray, hlite;
	Gtk::TextIter grayITb, grayITe, hliteITb, hliteITe;
	//bool graySet, hliteSet;
	int grayIdx, hliteIdx;
	//Gtk::Allocation rlAlloc;
};
