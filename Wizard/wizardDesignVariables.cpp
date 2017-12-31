#pragma execution_character_set("utf-8")
#include "../Utility/macrodefined.h"
#include "../Utility/commonStyle.h"
#include "wizardDesignVariables.h"


wizardDesignVariables::wizardDesignVariables(parsProblem* atn_problem, QJsonObject* obj, QWidget *parent): QWizardPage(parent), 
_atn_problem(atn_problem), _obj(obj){

	setTitle("几何结构模型设置");
	_vars_value_widget = new varsDefaultValueTemplate(atn_problem, _obj);

	_hint = new QLabel(this);
	commonStyle::setHintStyle(_hint);

    //layout
	QVBoxLayout* vlayout = new QVBoxLayout;
	QLayout* layout = _vars_value_widget->getLayout();
	vlayout->addLayout(layout);
	//vlayout->addStretch();
	vlayout->addWidget(_hint);
	vlayout->setContentsMargins(2, 20, 10, 2);
	setLayout(vlayout);
	connect(_vars_value_widget, SIGNAL(signal_checkValid()), this, SIGNAL(completeChanged()));
}

bool wizardDesignVariables::isComplete() const {
	_hint->clear();
	QList<iTemplate*> templates = getTemplatesWidget();
	for (iTemplate* iter : templates) {
		if (!iter->checkInputValid()) {
			_hint->setText(iter->checkInfo->message);
			return false;
		}
	}
	return true;
}

QList<iTemplate*> wizardDesignVariables::getTemplatesWidget() const {
	return QList<iTemplate*> {_vars_value_widget};
}

/*wizardDesignVariables::~wizardDesignVariables(){
	delete _vars_value_widget;
	_vars_value_widget = nullptr;
}*/