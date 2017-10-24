#pragma once

#include <QWizardPage>
#include <QtWidgets>
#include <fstream>
#include <QRegExp>
#include <QTextStream>

class wizardAddSetting :public QWizardPage{
	Q_OBJECT
public:
    wizardAddSetting(QWidget *parent = 0);
	~wizardAddSetting();

protected:
	//validate before click Next/Finish
	bool isComplete() const override;

private:
	void initSettingLayout();

public slots:
	void slot_selectPath();

private:  
    QLabel* _project_name_label;
    QLineEdit* _project_name_edit;
    QLabel* _project_path_label;
    QLineEdit* _project_path_edit;
    QPushButton* _select_path_btn;
    QRadioButton* _always_path;
	QLabel* _hint;
};