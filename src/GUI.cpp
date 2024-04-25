#include "first.hpp"
#include "widgets.hpp"
#include "GUI.hpp"
#include "trie.hpp"
#include "avlExceptions.hpp"
#include "dictExceptions.hpp"

using namespace std;

GUI::GUI(void)
{
	pG = NULL;
	MainDlg = NULL;
	MessageBox = NULL;
	dictionaryDlgCloseButton = NULL;
	MessageCloseButton = NULL;
	dictionaryLabel = NULL;
	MessageLabel = NULL;
	patternEntry = NULL;
	resultsList = NULL;
	dictionaryTextView = NULL;
	statusLabel = NULL;
	//searchLabel = NULL;
	patternLabel = NULL;
}

GUI::~GUI(void)
{
	delete pTree;
}

void GUI::ResetGUI(pGEXT p_g)
{
	pG = p_g;
	pG->get("MainDlg", MainDlg);
	pG->get("dictionaryDlgCloseButton", dictionaryDlgCloseButton);
	pG->get("dictionaryLabel", dictionaryLabel);
	pG->get("patternEntry", patternEntry);
	pG->get("resultsList", resultsList);
	pG->get("dictionaryTextView", dictionaryTextView);
	pG->get("statusLabel", statusLabel);
	pG->get("MessageBox", MessageBox);
	pG->get("MessageCloseButton", MessageCloseButton);
	pG->get("MessageLabel", MessageLabel);
	//pG->get("searchLabel", searchLabel);
	pG->get("patternLabel", patternLabel);

	pTree = new TRIE::trieTree;
	statusLabel->set_text("");
	string pLabel = "Enter pattern below (wildcards are ";
	pLabel += TRIE::C_WC;
	pLabel += " and ";
	pLabel += TRIE::S_WC;
	pLabel += ')';
	patternLabel->set_text(pLabel);

	txtView = Gtk::TextBuffer::create();
	dictionaryTextView->set_buffer(txtView);
	matchView = Gtk::TextBuffer::create();
	resultsList->set_buffer(matchView);
	
	hlite = matchView->create_tag("hlite");
	hlite->property_background() = "blue";
	hlite->property_paragraph_background() = "blue";
	hlite->property_foreground() = "white";
	//hliteSet = false;
	hliteIdx = -1;

	gray = matchView->create_tag("grayTag");
	gray->property_background() = "lightgray";
	gray->property_paragraph_background() = "lightgray";
	//graySet = false;
	grayIdx = -1;

	rlArrow = Gdk::Cursor::create(Gdk::ARROW);
	//rlAlloc = resultsList->get_allocation();
	//rlWin.get()->set_cursor(rlArrow);
	//gdk_window_set_cursor(rlWin.get()->gobj(), rlArrow.get()->gobj());
	//Glib::RefPtr<Gdk::Cursor> rlCur = gdk_cursor_new_from_name(rlDsp, "default");
	//rlWin->set_cursor(rlArrow);
}

/*bool GUI::HandlemainWindow_key_release_event(GdkEventKey* event)
{
	static const char 
		* allowables = " *\'-0123456789?ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char c = ' ';
	Glib::ustring txt = patternEntry->get_text(), buff = "";
	int pSz = txt.size(),
		pos = patternEntry->get_position(); // in characters which this procedure makes ASCII
	bool deleteMe = false, updateMe = false, atEnd = pos == pSz;
	string lst = txt.substr(pos - 1, 1);
	if (lst.size() > 1) deleteMe = true;
	else
	{
		c = lst[0];
		char C = toupper(c);
		if (c != C) 
		{
			updateMe = true;
			c = C;
		}
		else
		{
			if (1 != strspn(lst.c_str(), allowables)) deleteMe = true;
		}
	}
	if (deleteMe || updateMe) 
	{
		buff = txt.substr(0, pos - 1);
		if (updateMe) buff += c;
		if (!atEnd) buff += txt.substr(pos + 1);
		patternEntry->set_text(buff);
		if (pos == pSz) patternEntry->set_position(-1);
		else patternEntry->set_position(pos);
	}
	return false;

}*/
//void GUI::HandleMainDlgRealize(void)
//{
	//rlWin = resultsList->get_window(Gtk::TEXT_WINDOW_TEXT);
//}

//void GUI::HandleMainDlgSize(Gtk::Allocation & a)
//{
//	rlAlloc = resultsList->get_allocation();
//}

void GUI::HandledictionaryDlgCloseButton(void)
{
	//cout << "GUI::HandledictionaryDlgCloseButton called.\n";
	MainDlg->close();
	delete MainDlg;

}

void GUI::HandleMessageBoxCloseButton(void)
{

	MainDlg->close();
	delete MainDlg;

}


void GUI::HandlepatternEntry(void)
{
	//cout << "GUI::HandlepatternEntry called.\n";
	try
	{
		txtView->set_text("");
		Glib::ustring ptt = patternEntry->get_text();
		int pos = patternEntry->get_position();
		bool atEnd = pos == ptt.size();
		if (atEnd) pos = -1;
		string r = pTree->debug_pattern(ptt);
		patternEntry->set_text(r);
		if (r.size() != ptt.size()) pos = -1;
		patternEntry->set_position(pos);
		pTree->pattern = r;
		statusLabel->set_text("searching dictionary ...");
		//unload_listbox();
		while(gtk_events_pending()) {gtk_main_iteration_do(false);}	
		pTree->search();
		statusLabel->set_text("loading results list ...");
		while(gtk_events_pending()) {gtk_main_iteration_do(false);}	
		//vector<string> & wrds = pTree->wrdvec;
		//UINT wSz = wrds.size();
		/*for (UINT i = 0; i < wSz; ++i)
		{
			LBRow * lbr = new LBRow;
			string wrd = wrds[i];
			lbr->WORD = new Gtk::Label(wrd.c_str(), Gtk::ALIGN_START);
			lbr->add_m_all();
			lbRows.push_back(lbr);
			resultsList->append(*lbr);
			resultsList->show_all();
		}*/
		UINT rSz = pTree->results.size();
		if (rSz > 0) 
		{
			Glib::ustring resStr = pTree->resultString;
			matchView->set_text(resStr);
			grayIdx = -1;
			hliteIdx = -1;
			statusLabel->set_text("select a row to read the entry");
			rlWin = resultsList->get_window(Gtk::TEXT_WINDOW_TEXT);
			//Glib::RefPtr<Gdk::Cursor> rlArrow = Gdk::Cursor::create(Gdk::ARROW);
			rlWin->set_cursor(rlArrow);
		}
		else statusLabel->set_text("");
	}
	catch (DICX::dictexception & e)
	{
		MessageLabel->set_text(e.what());
		int m = MessageBox->run();
	}
	catch (AVLX::avlexception & e)
	{
		MessageLabel->set_text(e.what());
		int m = MessageBox->run();

	}
	
}
//void GUI::HandleresultsList(Gtk::ListBoxRow * row)
//{
//	UINT idx = row->get_index();
//	string s = "you have selected row " + to_string(idx + 1);
//	statusLabel->set_text(s);
//	Glib::ustring entry = pTree->getEntry(idx);
	
//	txtView->set_text(entry);

//}
bool GUI::HandleresultsListClick(GdkEventButton * b)
{
	int xBuff = 0, yBuff = 0, lineTop = 0, charCt = 0, rSz = pTree->results.size();
	if (rSz == 0) return true;
	resultsList->window_to_buffer_coords(Gtk::TEXT_WINDOW_TEXT, b->x, b->y, xBuff, yBuff);
	Gtk::TextIter tITb, tITe;
	resultsList->get_line_at_y(tITb, yBuff, lineTop);
	int idx = tITb.get_line();
	if (hliteIdx >= 0) matchView->remove_tag(hlite, hliteITb, hliteITe);
	hliteIdx = -1;
	if (idx >= rSz) return true;
	charCt = tITb.get_chars_in_line();
	tITe = tITb;
	tITe.forward_chars(charCt);
	hliteITb = tITb;
	hliteITe = tITe;
	if (idx == grayIdx) 
	{
		matchView->remove_tag(gray, hliteITb, hliteITe);
		grayIdx = -1;
	}
	matchView->apply_tag(hlite, hliteITb, hliteITe);
	hliteIdx = idx;
	string s = "you have selected \"" + pTree->getWord(idx) + "\" at row " + to_string(idx + 1);
	statusLabel->set_text(s);
	Glib::ustring entry = pTree->getEntry(idx);
	
	txtView->set_text(entry);
	rlWin = resultsList->get_window(Gtk::TEXT_WINDOW_TEXT);
	rlWin->set_cursor(rlArrow);

	return true;

}

bool GUI::HandleresultsListMouse(GdkEventMotion * m)
{

	int xBuff = 0, yBuff = 0, lineTop = 0, charCt = 0, rSz = pTree->results.size();
	if (rSz == 0) return true;
	resultsList->window_to_buffer_coords(Gtk::TEXT_WINDOW_TEXT, m->x, m->y, xBuff, yBuff);
	Gtk::TextIter tITb, tITe;
	resultsList->get_line_at_y(tITb, yBuff, lineTop);
	int idx = tITb.get_line();
	if (grayIdx >= 0) matchView->remove_tag(gray, grayITb, grayITe);
	grayIdx = -1;
	//bool ooWin = (m->x < 0.0) || (m->y < 0.0) || (m->x > rlAlloc.get_width()) || (m->y > rlAlloc.get_height());
	if ((idx >= rSz) || (idx == hliteIdx)) return true;
	charCt = tITb.get_chars_in_line();
	tITe = tITb;
	tITe.forward_chars(charCt);
	grayITb = tITb;
	grayITe = tITe;
	matchView->apply_tag(gray, grayITb, grayITe);
	grayIdx = idx;
	//Glib::RefPtr<Gdk::Window> rlWin = resultsList->get_window(Gtk::TEXT_WINDOW_TEXT);
	//Glib::RefPtr<Gdk::Cursor> rlArrow = Gdk::Cursor::create(Gdk::ARROW);
	//rlWin->set_cursor(rlArrow);

	return true;

}

bool GUI::HandleresultsListLeave(GdkEventCrossing * c)
{
	//std::cout << "HandleresultsListLeave\n";
	if (grayIdx >= 0)
	{
		matchView->remove_tag(gray, grayITb, grayITe);
		grayIdx = -1;
	}
	return true;
}

//void GUI::HandleresultsListRealize(void)
//{
//	std::cout << "HandleresultsListRealize\n";
//}
//void GUI::unload_listbox(void)
//{
//    vector<Gtk::Widget*> lbrows = resultsList->get_children();
//    for (vector<Gtk::Widget*>::iterator wIT = lbrows.begin(); wIT != lbrows.end(); ++wIT)
//    {
//        resultsList->remove(**wIT);
//    }

//    for (vector<LBRow*>::iterator rIT = lbRows.begin(); rIT != lbRows.end(); ++rIT)
//    {
//        delete *rIT;
//    }

//    lbRows.clear();
//}

//bool GUI::HandledictionaryTextViewClick(GdkEventButton * b)
//{
	//int xBuff = 0, yBuff = 0, lineTop = 0, charCt = 0;
	//dictionaryTextView->window_to_buffer_coords(Gtk::TEXT_WINDOW_TEXT, b->x, b->y, xBuff, yBuff);
	//Gtk::TextIter tITb, tITe;
	//dictionaryTextView->get_line_at_y(tITb, yBuff, lineTop);
	//charCt = tITb.get_chars_in_line();
	//tITe = tITb;
	//tITb.forward_chars(charCt);
	//txtView->select_range(tITb, tITe);
	//Glib::RefPtr<Gdk::Window> rlWin = resultsList->get_window(Gtk::TEXT_WINDOW_TEXT);
	//Glib::RefPtr<Gdk::Cursor> rlArrow = Gdk::Cursor::create(Gdk::ARROW);
	//rlWin->set_cursor(rlArrow);
//	return true;
//}

//void GUI::HandledictionaryTextViewRealize(void)
//{
//	std::cout << "HandledictionaryTextViewRealize\n";
//}

//GUI::LBRow::LBColumns::LBColumns(void):Gtk::Box(Gtk::ORIENTATION_HORIZONTAL)
//{
//	WORD = NULL;
//}

//GUI::LBRow::LBColumns::~LBColumns(void)
//{
//	if (WORD != NULL) delete WORD;
//}

//GUI::LBRow::LBRow(void)
//{
	//LBCols = new LBColumns;
//	WORD = NULL;
//	rIdx = 0;
//}

//GUI::LBRow::~LBRow(void)
//{
//	if (WORD != NULL) delete WORD; //LBCols;
//}

//void GUI::LBRow::add_m_all(void)
//{
//	WORD->set_width_chars(35); //LBCols->pack_start(*LBCols->WORD, Gtk::PACK_SHRINK);
//	add(*WORD);
//}





