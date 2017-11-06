#pragma execution_character_set("utf-8")
#include "wizardAddSetting.h"
#include "../Utility/global.h"

wizardAddSetting::wizardAddSetting(QWidget *parent) : QWizardPage(parent){
	setTitle("基本设置");
    _project_name_label = new QLabel("工程名称：", this);
    _project_name_edit = new QLineEdit(this);
    _project_path_label = new QLabel("工程路径：", this);
    _project_path_edit = new QLineEdit(this);
    _select_path_btn = new QPushButton("浏览", this);
    _always_path = new QRadioButton("设为默认路径", this);
    _always_path->setChecked(true);
	_hint = new QLabel(this);
	QFont font;
	font.setPixelSize(20);
	_hint->setFont(font);
	
	//register data field, with asterisk(*) means its a mandatory field
	registerField("project_name*", _project_name_edit);
	registerField("project_path*", _project_path_edit);
	registerField("is_default_path", _always_path);

	//读取默认路径
	QString default_path = dataPool::global::getGDefaultProjectPath();
    if(default_path.isNull() || default_path.isEmpty())
		default_path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    _project_path_edit->setText(default_path);
    _project_path_edit->setReadOnly(true);

	initSettingLayout();
    connect(_select_path_btn, SIGNAL(clicked()), this, SLOT(slot_selectPath()));
}

void wizardAddSetting::initSettingLayout() {
	QVBoxLayout *vlayout = new QVBoxLayout;
	QHBoxLayout *hlayout1 = new QHBoxLayout; 
	QHBoxLayout *hlayout2 = new QHBoxLayout;
	QHBoxLayout *hlayout3 = new QHBoxLayout;	
	hlayout1->addWidget(_project_name_label);
	hlayout1->addWidget(_project_name_edit);
	vlayout->addLayout(hlayout1);
	
	hlayout2->addWidget(_project_path_label);
	hlayout2->addWidget(_project_path_edit);
	hlayout2->addWidget(_select_path_btn);
	vlayout->addLayout(hlayout2);

	vlayout->addWidget(_always_path);

	hlayout3->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
	hlayout3->addWidget(_hint);
	hlayout3->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
	vlayout->addLayout(hlayout3);

	setLayout(vlayout);
}

bool wizardAddSetting::isComplete() const{	
	_hint->clear();
	if (_project_name_edit->text().isEmpty() || _project_name_edit->text().isNull())
		return false;
	QDir dir;
	QString full_path = QString("%1/%2").arg(_project_path_edit->text().trimmed())
		.arg(_project_name_edit->text().trimmed());
	if (dir.exists(full_path)) {
		_hint->setText("当前工程已存在，请更改工程名或工程路径。");
		return false;
	}
	return true;
}

// slots functions
void wizardAddSetting::slot_selectPath(){
    QFileDialog* open_file_path = new QFileDialog();
    QString home_dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString select_dir = open_file_path->getExistingDirectory(this, "选择文件夹", home_dir,
                                                           QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	_project_path_edit->setText(select_dir);
    _always_path->setChecked(false);
	delete open_file_path;
	open_file_path = nullptr;
}

wizardAddSetting::~wizardAddSetting(){}