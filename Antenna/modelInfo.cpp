#include "../Utility/macrodefined.h"
#include "modeInfo.h"

modelInfo::modelInfo(parsProblem* atn_problem, QWidget *parent) : QDialog(parent), _atn_problem(atn_problem) {
	//remove help menu
	this->setWindowFlags(windowFlags() &~Qt::WindowContextHelpButtonHint);
	setMinimumSize(SUBWINDOW_WIDTH, SUBWINDOW_HEIGHT);
	
	_atn_info = new QTextEdit(this);
	_atn_info->setReadOnly(true);
	_atn_info->setText(_atn_problem->info);

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(_atn_info);
	setLayout(layout);
}

modelInfo::~modelInfo() {}