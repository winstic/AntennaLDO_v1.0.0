#pragma execution_character_set("utf-8")
#include "../Utility/macrodefined.h"
#include "../Utility/parseJson.h"
#include "../Utility/commonStyle.h"
#include "algorithmModel.h"

algorithmModel::algorithmModel(parsProblem* atn_problem, parsAlgorithm* algorithm, QJsonObject global_obj, bool is_running, QWidget *parent)
	: QDialog(parent), _atn_problem(atn_problem), _algorithm(algorithm), _global_obj(global_obj), _is_running(is_running) {
	//remove help menu
	this->setWindowFlags(windowFlags() &~Qt::WindowContextHelpButtonHint);
	setMinimumSize(SUBWINDOW_WIDTH, SUBWINDOW_HEIGHT);

	_algorithm_obj = nullptr;
	_algorithm_widget = new algorithmTemplate(_atn_problem, &_global_obj, &_algorithm, &_algorithm_obj);

	_confirm_button = new QPushButton(QString("ȷ��"), this);
	_cancel_button = new QPushButton(QString("ȡ��"), this);
	_hint = new QLabel(this);
	commonStyle::setHintStyle(_hint);

	initLayout();
	connect(_confirm_button, SIGNAL(clicked(bool)), this, SLOT(slot_confirmButton(bool)));
	connect(_cancel_button, SIGNAL(clicked(bool)), this, SLOT(slot_cancelButton(bool)));
}

void algorithmModel::initLayout() {
	_layout = new QVBoxLayout;
	QLayout* varlayout = _algorithm_widget->getLayout();
	QHBoxLayout* hlayout = new QHBoxLayout;
	//�ڰ�ť�������������ð�ť����
	hlayout->addWidget(_hint);
	hlayout->addStretch();
	hlayout->addWidget(_confirm_button);
	hlayout->addWidget(_cancel_button);
	_layout->addLayout(varlayout);
	_layout->addSpacing(20);
	_layout->addLayout(hlayout);
	_layout->setContentsMargins(10, 20, 10, 10);

	_algorithm_widget->traversalWidgets(_algorithm_widget->children(), !_is_running);
	_confirm_button->setEnabled(!_is_running);

	setLayout(_layout);
}

void algorithmModel::slot_confirmButton(bool) {
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
		&& parseJson::write(QString("%1/%2/global_conf.json").arg(dataPool::global::getGWorkingProjectPath()).arg(dataPool::global::getGCurrentSpecName()), &_global_obj))
		this->close();
	else {
		qCritical("save failed in algorithm tabWidget.");
		QMessageBox::critical(0, QString("Error"), QString("save failed."));
	}
}

void algorithmModel::slot_cancelButton(bool) {
	this->close();
}

/*void algorithmModel::closeEvent(QCloseEvent *event) {
	QMessageBox::StandardButton rb = QMessageBox::question(NULL, "ɾ��", "���������޸ĵ����ݣ�", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (rb == QMessageBox::Yes) {
		slot_saveAllButton(true);
	}
	else
		return;
}
*/

algorithmModel::~algorithmModel() {
	delete _layout;
	_layout = nullptr;
}