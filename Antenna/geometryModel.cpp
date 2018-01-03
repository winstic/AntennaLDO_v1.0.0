#include "../Utility/macrodefined.h"
#include "../Utility/parseJson.h"
#include "geometryModel.h"

geometryModel::geometryModel(parsProblem* atn_problem, QJsonObject* problem_obj, QWidget *parent) : QDialog(parent), 
_atn_problem(atn_problem), _problem_obj(problem_obj) {
	//remove help menu
	this->setWindowFlags(windowFlags() &~Qt::WindowContextHelpButtonHint);
	setMinimumSize(SUBWINDOW_WIDTH, SUBWINDOW_HEIGHT);
	_variables_widget = new variablesTemplate(_atn_problem, _problem_obj);

	_save_all_button = new QPushButton(QString("±£´æËùÓÐ"), this);
	_hint = new QLabel(this);
	QFont font;
	font.setPixelSize(20);
	_hint->setFont(font);

	connect(_save_all_button, SIGNAL(clicked(bool)), this, SLOT(slot_saveAllButton(bool)));
}

void geometryModel::initLayout() {
	QVBoxLayout* vlayout = new QVBoxLayout;
	QLayout* varlayout = _variables_widget->getLayout();
	QHBoxLayout* hlayout = new QHBoxLayout;
	//ÔÚ°´Å¥×ó²àÌí¼ÓÉìËõ£¬ÈÃ°´Å¥¾ÓÓÒ
	hlayout->addWidget(_hint);
	hlayout->addStretch();
	hlayout->addWidget(_save_all_button);
	vlayout->addLayout(varlayout);
	vlayout->addLayout(hlayout);
	vlayout->setContentsMargins(10, 20, 10, 2);
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

	if (parseJson::write(QString("%1/%2_conf.json").arg(dataPool::global::getGWorkingProjectPath()).arg(_atn_problem->name), _problem_obj))
		this->close();
	else {
		qCritical("save failed in design tabWidget.");
		QMessageBox::critical(0, QString("Error"), QString("save failed."));
	}
}

geometryModel::~geometryModel() {}