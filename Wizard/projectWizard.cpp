#pragma execution_character_set("utf-8")
#include "../Utility/macrodefined.h"
#include "../Utility/parseJson.h"
#include "projectWizard.h"

projectWizard::projectWizard(parsProblem* antenna_problem, QWidget *parent) : QWizard(parent),
_antenna_problem(antenna_problem){
    setWindowTitle(_antenna_problem->name + " 工程向导");
    //remove help menu
    this->setWindowFlags(windowFlags() &~ Qt::WindowContextHelpButtonHint);
    setButtonText(QWizard::NextButton, "下一步>");
    _introduce = new wizardIntroduce(_antenna_problem, this);
    _add_setting = new wizardAddSetting(this);
    // need improve

    //confManage = new config();
    addPage(_introduce);
    addPage(_add_setting);        
	setMinimumSize(SUBWINDOW_WIDTH, SUBWINDOW_HEIGHT);
}

void projectWizard::accept() {
	//在addSetting类中已经注册参数，获取相关值并同步到全局参数
	QString project_path = field("project_path").toString();
	QString project_name = field("project_name").toString();
	QString working_path = QString("%1/%2").arg(project_path).arg(project_name);
	if (field("is_default_path").toBool()) {
		dataPool::global::setGDefaultProjectPath(project_path);
		parseJson::updateConfigFile("DEFAULT_PATH", project_path);
		qInfo("default path change to '%s'", qUtf8Printable(project_path));
	}
	dataPool::global::setGProjectName(project_name);
	dataPool::global::setGWorkingProjectPath(working_path);
	qInfo("project name is '%s'", qUtf8Printable(project_name));
	qInfo("working path is '%s'", qUtf8Printable(working_path));

	QDialog::accept();
}

projectWizard::~projectWizard() {
	delete _introduce;
	_introduce = nullptr;
	delete _add_setting;
	_add_setting = nullptr;
}