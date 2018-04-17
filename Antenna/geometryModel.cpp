#pragma execution_character_set("utf-8")
#include "../Utility/macrodefined.h"
#include "../Utility/parseJson.h"
#include "../Utility/commonStyle.h"
#include "geometryModel.h"

geometryModel::geometryModel(parsProblem* atn_problem, bool is_running, QWidget *parent) : QDialog(parent), 
_atn_problem(atn_problem), _is_running(is_running) {
	//remove help menu
	this->setWindowFlags(windowFlags() &~Qt::WindowContextHelpButtonHint);
	setMinimumSize(SUBWINDOW_WIDTH, SUBWINDOW_HEIGHT);
	//setStyleSheet("background-color: white");
	_problem_obj = parseJson::getJsonObj(QString("%1/%2_conf.json").arg(_atn_problem->path).arg(_atn_problem->name));
	if (!_problem_obj.isEmpty()) {
		_variables_widget = new variablesTemplate(_atn_problem, &_problem_obj);

		_confirm_button = new QPushButton(QString("确定"), this);
		_cancel_button = new QPushButton(QString("取消"), this);
		_hint = new QLabel(this);
		commonStyle::setHintStyle(_hint);

		connect(_confirm_button, SIGNAL(clicked(bool)), this, SLOT(slot_confirmButton(bool)));
		connect(_cancel_button, SIGNAL(clicked(bool)), this, SLOT(slot_cancelButton(bool)));
		initLayout();
	}
}

void geometryModel::initLayout() {
	_layout = new QVBoxLayout;
	QLayout* varlayout = _variables_widget->getLayout();
	QHBoxLayout* hlayout = new QHBoxLayout;
	//在按钮左侧添加伸缩，让按钮居右
	hlayout->addWidget(_hint);
	hlayout->addStretch();
	hlayout->addWidget(_confirm_button);
	hlayout->addWidget(_cancel_button);	
	_layout->addLayout(varlayout);
	_layout->addSpacing(20);
	_layout->addLayout(hlayout);
	_layout->setContentsMargins(10, 20, 10, 10);

	_variables_widget->traversalWidgets(_variables_widget->children(), !_is_running);
	_confirm_button->setEnabled(!_is_running);
	setLayout(_layout);
}

void geometryModel::slot_confirmButton(bool) {
	QList<iTemplate*> templates{ _variables_widget };

	_hint->clear();
	for (iTemplate* iter : templates) {
		if (!iter->checkInputValid()) {
			_hint->setText(iter->checkInfo->message);
			return;
		}
	}
	for (iTemplate* iter : templates)
		iter->updateJObj();

	if (parseJson::write(QString("%1/%2_conf.json").arg(_atn_problem->path).arg(_atn_problem->name), &_problem_obj))
		this->close();
	else {
		qCritical("save failed in geometry variables tabWidget.");
		QMessageBox::critical(0, QString("Error"), QString("save failed."));
	}
}

void geometryModel::slot_cancelButton(bool) {
	this->close();
}

geometryModel::~geometryModel() {
	delete _layout;
	_layout = nullptr;
}