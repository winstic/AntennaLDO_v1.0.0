#pragma execution_character_set("utf-8")
#include "../Utility/macrodefined.h"
#include "../Utility/parseJson.h"
#include "../Utility/commonStyle.h"
#include "geometryModel.h"

geometryModel::geometryModel(parsProblem* atn_problem, QWidget *parent) : QDialog(parent), 
_atn_problem(atn_problem) {
	//remove help menu
	this->setWindowFlags(windowFlags() &~Qt::WindowContextHelpButtonHint);
	setMinimumSize(SUBWINDOW_WIDTH, SUBWINDOW_HEIGHT);
	//setStyleSheet("background-color: white");
	_problem_obj = parseJson::getJsonObj(QString("%1/%2_conf.json").arg(_atn_problem->path).arg(_atn_problem->name));
	if (_problem_obj.isEmpty()) {
		qCritical("get problem json object field.");
		QMessageBox::critical(0, QString("警告"), QString("读取问题配置文件失败！"));
	}
	else {
		_variables_widget = new variablesTemplate(_atn_problem, &_problem_obj);

		_save_all_button = new QPushButton(QString("保存所有"), this);
		_hint = new QLabel(this);
		commonStyle::setHintStyle(_hint);

		connect(_save_all_button, SIGNAL(clicked(bool)), this, SLOT(slot_saveAllButton(bool)));
		initLayout();
	}
}

void geometryModel::initLayout() {
	QVBoxLayout* vlayout = new QVBoxLayout;
	QLayout* varlayout = _variables_widget->getLayout();
	QHBoxLayout* hlayout = new QHBoxLayout;
	//在按钮左侧添加伸缩，让按钮居右
	hlayout->addWidget(_hint);
	hlayout->addStretch();
	hlayout->addWidget(_save_all_button);
	vlayout->addLayout(varlayout);
	vlayout->addLayout(hlayout);
	vlayout->setContentsMargins(10, 20, 10, 2);
	setLayout(vlayout);
}

void geometryModel::slot_saveAllButton(bool) {
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

geometryModel::~geometryModel() {}