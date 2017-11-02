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
    _hint = new QLabel("请输入工程名称。", this);
    _always_path->setChecked(true);

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

    connect(_select_path_btn, SIGNAL(clicked()), this, SLOT(slot_selectPath()));
}

void wizardAddSetting::initSettingLayout() {
	QVBoxLayout *vlayout = new QVBoxLayout(this);
	QHBoxLayout *hlayout1 = new QHBoxLayout(this); 
	QHBoxLayout *hlayout2 = new QHBoxLayout(this);
	QHBoxLayout *hlayout3 = new QHBoxLayout(this);	
	hlayout1->addWidget(_project_name_label);
	hlayout1->addWidget(_project_name_edit);
	vlayout->addLayout(hlayout1);
	
	hlayout2->addWidget(_project_path_label);
	hlayout2->addWidget(_project_path_edit);
	hlayout2->addWidget(_select_path_btn);
	vlayout->addLayout(hlayout2);

	vlayout->addWidget(_always_path);

	hlayout3->addSpacerItem(&QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
	hlayout3->addWidget(_hint);
	vlayout->addSpacerItem(&QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
	vlayout->addLayout(hlayout3);

	setLayout(vlayout);
}

bool wizardAddSetting::isComplete() const{	
	QDir dir;
	QString full_path = QString("%1/%2").arg(_project_path_edit->text().trimmed())
		.arg(_project_name_edit->text().trimmed());
    if (dir.exists(full_path)){
        _hint->setText("当前文件已存在" + full_path);
		qCritical("current file: '%s' already exist.", qUtf8Printable(full_path));
		return false;
	}
	qInfo("project setting success.");
	return true;
}

// slots functions
void wizardAddSetting::slot_selectPath(){
    QFileDialog* open_file_path = new QFileDialog();
    QString home_dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString select_dir = open_file_path->getExistingDirectory(this, tr("选择文件夹"), home_dir,
                                                           QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	_project_path_edit->setText(select_dir);
    _always_path->setChecked(false);
	delete open_file_path;
	open_file_path = nullptr;
}

wizardAddSetting::~wizardAddSetting(){}