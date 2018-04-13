#include "wizardIntroduce.h"
#include "../Utility/macrodefined.h"

wizardIntroduce::wizardIntroduce(parsProblem* atn_problem, QWidget *parent) : QWizardPage(parent), _atn_problem(atn_problem){
    setTitle(_atn_problem->name);
    _atn_info_text = new QTextEdit(this);
	_atn_info_text->setReadOnly(true);
    _atn_image_label = new QLabel(this);

	initIntroduceLayout();
}

void wizardIntroduce::initIntroduceLayout() {
	_atn_info_text->setText(_atn_problem->info);
	QPixmap mPixmap = QPixmap(_atn_problem->pImage);
	if (mPixmap.width() > INTRODUCEIMAGEPIX)
		_atn_image_label->setPixmap(mPixmap.scaledToWidth(INTRODUCEIMAGEPIX));
	else
		_atn_image_label->setPixmap(mPixmap);

	_layout = new QHBoxLayout;
	_layout->addWidget(_atn_info_text);
	_layout->addWidget(_atn_image_label);
	_layout->setContentsMargins(2, 20, 10, 2);
	setLayout(_layout);
}

bool wizardIntroduce::isComplete() const {
	return true;
}

wizardIntroduce::~wizardIntroduce(){
	delete _layout;
	_layout = nullptr;
}
