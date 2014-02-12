#include "stdafx.h"
#include "HostGameForm.h"

HostGameForm::HostGameForm(wxWindow *parent) : wxWindow(parent,
                                                        wxID_ANY) {
  sizer = new wxBoxSizer(wxVERTICAL);
  portBox = new wxTextCtrl(this, wxID_ANY);
  sizer->Add(portBox);
  SetSizer(sizer);
  Layout();
}