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

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(_atn_info_text);
	layout->addWidget(_atn_image_label);
	setLayout(layout);
}

bool wizardIntroduce::isComplete() const {
	return true;
}

wizardIntroduce::~wizardIntroduce(){}
