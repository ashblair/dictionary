#pragma once

/*CONCEPTUAL INCLUDES FOR THIS PROJECT:
#include "stdafx.h" which should contain
#include <gtkmm.h>
#include <assert.h>
#include <iostream>
#include <string>
#include <map>

*/
/*
	usage: (in main start with:) GEXT::argc = argc; GEXT::argv = argv;
	from the glade file, an extract:
	MAINS:
		MainDlg, a GtkDialog


	STYLES:
		dictionaryDlgCloseBox, a GtkButtonBox
		dictionaryMainBox, a GtkBox


	CONTAINERS:
		patternBox, a GtkBox
		lookupBox, a GtkBox


	CONTROLS:
		dictionaryDlgCloseButton, a GtkButton
		dictionaryLabel, a GtkLabel
		patternEntry, a GtkEntry
		resultsList, a GtkTextView
		dictionaryTextView, a GtkTextView
		statusLabel, a GtkLabel
*/
typedef class GUI * pGUI;

class glade_info
{
public:
	std::string classType, id;
	Gtk::Widget * pW;

	glade_info(void):classType{}, id{}, pW{NULL}{}
	glade_info(const glade_info &a):classType{a.classType}, id{a.id}, pW{a.pW}{}
	~glade_info(void){}

	Gtk::Widget * getWidget(void) {return pW;}
	std::string getName() {return id;}
	std::string getClassType() {return classType;}
};
typedef class glade_extract GEXT;
typedef GEXT * pGEXT;

class glade_extract
{
enum wdg_enum{MainDlg = 0, dictionaryDlgCloseBox = 1, dictionaryMainBox = 2, patternBox = 3, MessageBox = 4, lookupBox = 5, dictionaryDlgCloseButton = 6, dictionaryLabel = 7, patternEntry = 8, resultsList = 9, dictionaryTextView = 10, statusLabel = 11, MessageCloseButton = 12, MessageLabel = 13, MessageVBox = 14, patternLabel = 15, searchLabel = 16};
	Gtk::Widget * get(const std::string &);
public:
	void get(const std::string & s, Gtk::Box*&gtk){gtk=dynamic_cast<Gtk::Box*>(get(s));}
	void get(const std::string & s, Gtk::Button*&gtk){gtk=dynamic_cast<Gtk::Button*>(get(s));}
	void get(const std::string & s, Gtk::ButtonBox*&gtk){gtk=dynamic_cast<Gtk::ButtonBox*>(get(s));}
	void get(const std::string & s, Gtk::Dialog*&gtk){gtk=dynamic_cast<Gtk::Dialog*>(get(s));}
	void get(const std::string & s, Gtk::Entry*&gtk){gtk=dynamic_cast<Gtk::Entry*>(get(s));}
	void get(const std::string & s, Gtk::Label*&gtk){gtk=dynamic_cast<Gtk::Label*>(get(s));}
	void get(const std::string & s, Gtk::ListBox*&gtk){gtk=dynamic_cast<Gtk::ListBox*>(get(s));}
	void get(const std::string & s, Gtk::TextView*&gtk){gtk=dynamic_cast<Gtk::TextView*>(get(s));}
	bool on_MainDlg_delete_event(GdkEventAny*);
	void on_MainDlg_size_allocate(Gtk::Allocation&);

	//void on_MainDlg_realize(void);
	//bool on_MainDlg_key_release_event(GdkEventKey*);	
	void on_dictionaryDlgCloseButton_clicked(void);
	void on_MessageCloseButton_clicked(void);
	void on_patternEntry_activate(void);
	//void on_resultsList_row_activated(Gtk::ListBoxRow*);
	bool on_resultsList_button_release_event(GdkEventButton*);
	//bool CXN[152] = viewDraw->signal_motion_notify_event().connect(sigc::mem_fun<GdkEventMotion *, bool>(*this, &glade_extract::on_viewDraw_motion_notify_event));
	bool on_resultsList_motion_notify_event(GdkEventMotion*);
	bool on_resultsList_leave_notify_event(GdkEventCrossing*);
	//void on_resultsList_realize(void);
	//bool on_dictionaryTextView_button_release_event(GdkEventButton*);
	//void on_dictionaryTextView_realize(void);
	static Gtk::Dialog* CMW;
	static int argc;
	static char ** argv;
	static Glib::RefPtr<Gtk::Application> the_app;
	static Glib::RefPtr<Gtk::Builder> refBuilder;
	static Glib::RefPtr<Gtk::CssProvider> provider;
	static Glib::RefPtr<Gdk::Display> display;
	static Glib::RefPtr<Gdk::Screen> screen;
	static pGEXT getInst(void);
	std::map<std::string, glade_info *> mains, styles, containers, controls;
	pGUI pCI;
	~glade_extract(void);
private:
	glade_info GI[17];
	glade_extract(void);
	static pGEXT pGEXTInstance;
};
