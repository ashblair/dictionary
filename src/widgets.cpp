#include "first.hpp"
#include "GUI.hpp"
#include "widgets.hpp"

using namespace std;

Glib::RefPtr<Gtk::CssProvider> GEXT::provider;
Glib::RefPtr<Gdk::Display> GEXT::display;
Glib::RefPtr<Gdk::Screen> GEXT::screen;
Glib::RefPtr<Gtk::Application> GEXT::the_app;
Glib::RefPtr<Gtk::Builder> GEXT::refBuilder;
Gtk::Dialog* GEXT::CMW = NULL;
pGEXT GEXT::pGEXTInstance = NULL;
//initialize these next statics first in main before getInst():
int GEXT::argc = 0;
char ** GEXT::argv = NULL;

extern char glade_start		asm("_binary_rsc_dictionary_glade_start");
extern char glade_end		asm("_binary_rsc_dictionary_glade_end");
extern char style_start		asm("_binary_rsc_style_css_start");
extern char style_end		asm("_binary_rsc_style_css_end");

glade_extract::glade_extract(void)
{
	the_app = Gtk::Application::create(argc, argv, "from.the.glade");
	//Load the GtkBuilder file and instantiate its widgets:
	refBuilder = Gtk::Builder::create();
	/*
	try
	{
		refBuilder->add_from_file("rsc/dictionary.glade");
	}
	catch(const Glib::FileError& ex)
	{
		cout << "FileError: " << ex.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	catch(const Glib::MarkupError& ex)
	{
		cout << "MarkupError: " << ex.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	catch(const Gtk::BuilderError& ex)
	{
		cout << "BuilderError: " << ex.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	provider = Gtk::CssProvider::create();
	Gtk::CssProvider * pProvider = provider.get();
	display = Gdk::Display::get_default();
	Gdk::Display * pDisplay = display.get();
	screen = pDisplay->get_default_screen();
	Gtk::StyleContext::add_provider_for_screen(screen, provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	pProvider->load_from_path("rsc/style.css");
	*/

	char * g_s = &glade_start, * g_e = &glade_end;
	gsize g_z = g_e - g_s;
	refBuilder->add_from_string(g_s, g_z);
	provider = Gtk::CssProvider::create();
	display = Gdk::Display::get_default();
	screen = display->get_default_screen();
	Gtk::StyleContext::add_provider_for_screen(screen, provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	char * s_s = &style_start, * s_e = &style_end;
	string s_str(s_s, s_e);
	provider->load_from_data(s_str);

	GI[MainDlg].classType = "GtkDialog";
	GI[MainDlg].id = "MainDlg";
	refBuilder->get_widget("MainDlg", GI[MainDlg].pW);
	mains["MainDlg"] = &GI[MainDlg];
	((Gtk::Dialog*)GI[MainDlg].pW)->signal_delete_event().connect(sigc::mem_fun<GdkEventAny *, bool>(*this, &glade_extract::on_MainDlg_delete_event));
	((Gtk::Dialog*)GI[MainDlg].pW)->signal_size_allocate().connect(sigc::mem_fun<Gtk::Allocation&, void>(*this, &glade_extract::on_MainDlg_size_allocate));
	//((Gtk::Dialog*)GI[MainDlg].pW)->signal_key_release_event().connect(sigc::mem_fun<GdkEventKey*, bool>(*this, &glade_extract::on_MainDlg_key_release_event));
	//((Gtk::Dialog*)GI[MainDlg].pW)->signal_realize().connect(sigc::mem_fun(*this, &glade_extract::on_MainDlg_realize));
	
	CMW = ((Gtk::Dialog*)GI[MainDlg].pW);
	GI[dictionaryDlgCloseBox].classType = "GtkButtonBox";
	GI[dictionaryDlgCloseBox].id = "dictionaryDlgCloseBox";
	refBuilder->get_widget("dictionaryDlgCloseBox", GI[dictionaryDlgCloseBox].pW);
	styles["dictionaryDlgCloseBox"] = &GI[dictionaryDlgCloseBox];
	GI[dictionaryMainBox].classType = "GtkBox";
	GI[dictionaryMainBox].id = "dictionaryMainBox";
	refBuilder->get_widget("dictionaryMainBox", GI[dictionaryMainBox].pW);
	styles["dictionaryMainBox"] = &GI[dictionaryMainBox];
	GI[patternBox].classType = "GtkBox";
	GI[patternBox].id = "patternBox";
	refBuilder->get_widget("patternBox", GI[patternBox].pW);
	containers["patternBox"] = &GI[patternBox];
	/*GI[listView].classType = "GtkViewport";
	GI[listView].id = "listView";
	refBuilder->get_widget("listView", GI[listView].pW);
	containers["listView"] = &GI[listView];*/
	GI[lookupBox].classType = "GtkBox";
	GI[lookupBox].id = "lookupBox";
	refBuilder->get_widget("lookupBox", GI[lookupBox].pW);
	containers["lookupBox"] = &GI[lookupBox];
	GI[dictionaryDlgCloseButton].classType = "GtkButton";
	GI[dictionaryDlgCloseButton].id = "dictionaryDlgCloseButton";
	refBuilder->get_widget("dictionaryDlgCloseButton", GI[dictionaryDlgCloseButton].pW);
	controls["dictionaryDlgCloseButton"] = &GI[dictionaryDlgCloseButton];
	((Gtk::Button*)GI[dictionaryDlgCloseButton].pW)->signal_clicked().connect(sigc::mem_fun(*this, &glade_extract::on_dictionaryDlgCloseButton_clicked));
	GI[dictionaryLabel].classType = "GtkLabel";
	GI[dictionaryLabel].id = "dictionaryLabel";
	refBuilder->get_widget("dictionaryLabel", GI[dictionaryLabel].pW);
	controls["dictionaryLabel"] = &GI[dictionaryLabel];
	GI[patternEntry].classType = "GtkEntry";
	GI[patternEntry].id = "patternEntry";
	refBuilder->get_widget("patternEntry", GI[patternEntry].pW);
	controls["patternEntry"] = &GI[patternEntry];
	((Gtk::Entry*)GI[patternEntry].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_patternEntry_activate));
	GI[resultsList].classType = "GtkTextView"; //ListBox";
	GI[resultsList].id = "resultsList";
	refBuilder->get_widget("resultsList", GI[resultsList].pW);
	controls["resultsList"] = &GI[resultsList];
	((Gtk::TextView*)GI[resultsList].pW)->signal_button_release_event().connect(sigc::mem_fun<GdkEventButton *, bool>(*this, &glade_extract::on_resultsList_button_release_event));
	((Gtk::TextView*)GI[resultsList].pW)->signal_motion_notify_event().connect(sigc::mem_fun<GdkEventMotion *, bool>(*this, &glade_extract::on_resultsList_motion_notify_event));
	((Gtk::TextView*)GI[resultsList].pW)->signal_leave_notify_event().connect(sigc::mem_fun<GdkEventCrossing *, bool>(*this, &glade_extract::on_resultsList_leave_notify_event));
	//((Gtk::TextView*)GI[resultsList].pW)->signal_realize().connect(sigc::mem_fun(*this, &glade_extract::on_resultsList_realize));
	GI[dictionaryTextView].classType = "GtkTextView";
	GI[dictionaryTextView].id = "dictionaryTextView";
	refBuilder->get_widget("dictionaryTextView", GI[dictionaryTextView].pW);
	controls["dictionaryTextView"] = &GI[dictionaryTextView];
	//((Gtk::TextView*)GI[dictionaryTextView].pW)->signal_button_release_event().connect(sigc::mem_fun<GdkEventButton *, bool>(*this, &glade_extract::on_dictionaryTextView_button_release_event));
	//((Gtk::TextView*)GI[dictionaryTextView].pW)->signal_realize().connect(sigc::mem_fun(*this, &glade_extract::on_dictionaryTextView_realize));
	GI[statusLabel].classType = "GtkLabel";
	GI[statusLabel].id = "statusLabel";
	refBuilder->get_widget("statusLabel", GI[statusLabel].pW);
	controls["statusLabel"] = &GI[statusLabel];
	GI[MessageBox].classType = "GtkDialog";
	GI[MessageBox].id = "MessageBox";
	refBuilder->get_widget("MessageBox", GI[MessageBox].pW);
	controls["MessageBox"] = &GI[MessageBox];
	/*MessageCloseButton, MessageLabel*/
	GI[MessageCloseButton].classType = "GtkButton";
	GI[MessageCloseButton].id = "MessageCloseButton";
	refBuilder->get_widget("MessageCloseButton", GI[MessageCloseButton].pW);
	controls["MessageCloseButton"] = &GI[MessageCloseButton];
	((Gtk::Button*)GI[MessageCloseButton].pW)->signal_clicked().connect(sigc::mem_fun(*this, &glade_extract::on_MessageCloseButton_clicked));
	GI[MessageLabel].classType = "GtkLabel";
	GI[MessageLabel].id = "MessageLabel";
	refBuilder->get_widget("MessageLabel", GI[MessageLabel].pW);
	controls["MessageLabel"] = &GI[MessageLabel];
	GI[MessageVBox].classType = "GtkBox";
	GI[MessageVBox].id = "MessageVBox";
	refBuilder->get_widget("MessageVBox", GI[MessageVBox].pW);
	styles["MessageVBox"] = &GI[MessageVBox];
	GI[patternLabel].classType = "GtkLabel";
	GI[patternLabel].id = "patternLabel";
	refBuilder->get_widget("patternLabel", GI[patternLabel].pW);
	styles["patternLabel"] = &GI[patternLabel];
	//GI[searchLabel].classType = "GtkLabel";
	//GI[searchLabel].id = "searchLabel";
	//refBuilder->get_widget("searchLabel", GI[searchLabel].pW);
	//controls["searchLabel"] = &GI[searchLabel];

	//the_app->run(*CMW);
}
glade_extract::~glade_extract(void)
{	delete pCI;
	}
pGEXT glade_extract::getInst(void)
{
	if(argv == NULL)
	{
		cout << "Error: intitialize GEXT::argc & GEXT::argv before calling getInst\n";
		exit(EXIT_FAILURE);
	}

	if(pGEXTInstance == NULL) pGEXTInstance = new glade_extract;
	return pGEXTInstance;
}
Gtk::Widget * glade_extract::get(const string & wName)
{
	map<string, glade_info *>::iterator mit = controls.find(wName);
	if (mit != controls.end()) return (*mit).second->getWidget();
	mit = containers.find(wName);
	if (mit != containers.end()) return (*mit).second->getWidget();
	mit = mains.find(wName);
	if (mit != mains.end()) return (*mit).second->getWidget();
	mit = styles.find(wName);
	if (mit != styles.end()) return (*mit).second->getWidget();
	return NULL;
}
bool glade_extract::on_MainDlg_delete_event(GdkEventAny* any_event)
{
	//cout << "MainDlg delete connected\n";
	return false;
}
void glade_extract::on_MainDlg_size_allocate(Gtk::Allocation & allocation)
{
	//cout << "MainDlg size connected\n";
	//pCI->HandleMainDlgSize(allocation);
}
//bool glade_extract::on_MainDlg_key_release_event(GdkEventKey* key)
//{
//	return pCI->HandlemainWindow_key_release_event(key);
//}	
//void glade_extract::on_MainDlg_realize(void)
//{
//	std::cout << "on_MainDlg_realize\n";
//}
void glade_extract::on_dictionaryDlgCloseButton_clicked(void)
{
	pCI->HandledictionaryDlgCloseButton();
}
void glade_extract::on_patternEntry_activate(void)
{
	pCI->HandlepatternEntry();
}
/*void glade_extract::on_resultsList_row_activated(Gtk::ListBoxRow* row)
{
	pCI->HandleresultsList(row);
}*/
bool glade_extract::on_resultsList_button_release_event(GdkEventButton* b)
{
	return pCI->HandleresultsListClick(b);
}
bool glade_extract::on_resultsList_motion_notify_event(GdkEventMotion * m)
{
	return pCI->HandleresultsListMouse(m);
}
bool glade_extract::on_resultsList_leave_notify_event(GdkEventCrossing * c)
{
	return pCI->HandleresultsListLeave(c);
}
//void glade_extract::on_resultsList_realize(void)
//{

//}
//bool glade_extract::on_dictionaryTextView_button_release_event(GdkEventButton* b)
//{
//	return pCI->HandledictionaryTextViewClick(b);	
//}
//void glade_extract::on_dictionaryTextView_realize(void)
//{

//}
void glade_extract::on_MessageCloseButton_clicked(void)
{
	pCI->HandleMessageBoxCloseButton();
}