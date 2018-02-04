#pragma execution_character_set("utf-8")
#include "../Utility/macrodefined.h"
#include "../Utility/parseJson.h"
#include "../Utility/commonStyle.h"
#include "algorithmModel.h"

algorithmModel::algorithmModel(parsProblem* atn_problem, parsAlgorithm* algorithm, QJsonObject global_obj, QWidget *parent)
	: QDialog(parent), _atn_problem(atn_problem), _algorithm(algorithm), _global_obj(global_obj) {
	//remove help menu
	this->setWindowFlags(windowFlags() &~Qt::WindowContextHelpButtonHint);
	setMinimumSize(SUBWINDOW_WIDTH, SUBWINDOW_HEIGHT);

	_algorithm_obj = nullptr;
	_algorithm_widget = new algorithmTemplate(_atn_problem, &_global_obj, &_algorithm, &_algorithm_obj);

	_save_all_button = new QPushButton(QString("±£´æËùÓÐ"), this);
	_hint = new QLabel(this);
	commonStyle::setHintStyle(_hint);

	initLayout();
	connect(_save_all_button, SIGNAL(clicked(bool)), this, SLOT(slot_saveAllButton(bool)));	
}

void algorithmModel::initLayout() {
	QVBoxLayout* vlayout = new QVBoxLayout;
	QLayout* varlayout = _algorithm_widget->getLayout();
	QHBoxLayout* hlayout = new QHBoxLayout;
	//ÔÚ°´Å¥×ó²àÌí¼ÓÉìËõ£¬ÈÃ°´Å¥¾ÓÓÒ
	hlayout->addWidget(_hint);
	hlayout->addStretch();
	hlayout->addWidget(_save_all_button);
	vlayout->addLayout(varlayout);
	vlayout->addLayout(hlayout);
	vlayout->setContentsMargins(10, 20, 10, 2);
	setLayout(vlayout);
}

void algorithmModel::slot_saveAllButton(bool) {
	QList<iTemplate*> templates{ _algorithm_widget };

	_hint->clear();
	for (iTemplate* iter : templates) {
		if (!iter->checkInputValid()) {
			_hint->setText(iter->checkInfo->message);
			return;
		}
	}
	for (iTemplate* iter : templates)
		iter->updateJObj();

	if (parseJson::write(QString("%1/%2_conf.json").arg(_algorithm->path).arg(_algorithm->name), _algorithm_obj) 
		&& parseJson::write(dataPool::global::getGCurrentGlobalJsonPath(), &_global_obj))
		this->close();
	else {
		qCritical("save failed in algorithm tabWidget.");
		QMessageBox::critical(0, QString("Error"), QString("save failed."));
	}
}

algorithmModel::~algorithmModel() {}