#pragma execution_character_set("utf-8")
#include <quuid.h>
#include "../Utility/macrodefined.h"
#include "../Utility/parseJson.h"
#include "problemManage.h"

problemManage::problemManage(QWidget *parent) : QDialog(parent) {
	//remove help menu
	setMinimumSize(SUBWINDOW_WIDTH / 2, SUBWINDOW_HEIGHT);
	this->setWindowFlags(windowFlags() &~Qt::WindowContextHelpButtonHint);

	//_exist_problem_label = new QLabel("已经存在的天线问题", this);
	//_remove_problem_label = new QLabel("已经移除的天线问题", this);
	_exist_problem_table = new tableTemplate();
	_remove_problem_table = new tableTemplate();
	_exist_problem_table->setFocusPolicy(Qt::NoFocus);
	_remove_problem_table->setFocusPolicy(Qt::NoFocus);
	_exist_problem_table->verticalHeader()->setVisible(false);
	_remove_problem_table->verticalHeader()->setVisible(false);
	_exist_problem_table->setSelectionMode(QAbstractItemView::SingleSelection);
	_remove_problem_table->setSelectionMode(QAbstractItemView::SingleSelection);
	_exist_problem_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	_remove_problem_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	_exist_problem_table->setColumnCount(1);
	_remove_problem_table->setColumnCount(1);

	_add_button = new QPushButton("添加新问题>>", this);
	_add_button->setProperty("add", 0);
	_remove_button = new QPushButton("移除", this);
	_remove_button->setEnabled(false);
	_recovery_button = new QPushButton("恢复", this);
	_recovery_button->setEnabled(false);
	_delete_button = new QPushButton("删除", this);
	_delete_button->setEnabled(false);
	_confirm_button = new QPushButton("确定", this);
	_cancel_button = new QPushButton("取消", this);

	_readme_text = new QTextBrowser(this);
	_readme_text->setFixedHeight(SUBWINDOW_HEIGHT / 2);
	_readme_text->setOpenExternalLinks(true);
	_select_directory_label = new QLabel("选择天线问题：", this);
	_select_directory_text = new QLineEdit(this);
	_select_directory_text->setReadOnly(true);
	_select_directory_button = new QPushButton("浏览", this);

	initExistProblem();
	initRemoveProblem();
	initLayout();

	connect(_exist_problem_table, SIGNAL(itemSelectionChanged()), this, SLOT(slot_existSelectChanged()));
	connect(_remove_problem_table, SIGNAL(itemSelectionChanged()), this, SLOT(slot_removeSelectChanged()));
	connect(_add_button, SIGNAL(clicked(bool)), this, SLOT(slot_add(bool)));
	connect(_remove_button, SIGNAL(clicked(bool)), this, SLOT(slot_remove(bool)));
	connect(_recovery_button, SIGNAL(clicked(bool)), this, SLOT(slot_reconvery(bool)));
	connect(_delete_button, SIGNAL(clicked(bool)), this, SLOT(slot_delete(bool)));
	connect(_select_directory_button, SIGNAL(clicked(bool)), this, SLOT(slot_select_Directory(bool)));
	connect(_confirm_button, SIGNAL(clicked(bool)), this, SLOT(slot_confirm(bool)));
	connect(_cancel_button, SIGNAL(clicked(bool)), this, SLOT(slot_cancel(bool)));
}

void problemManage::initExistProblem() {
	_exist_problem_table->clear();
	QStringList header1;
	header1 << "已经存在的天线问题";
	_exist_problem_table->setHorizontalHeaderLabels(header1);
	_exist_problem_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	QStringList problem_list;
	QVector<parsProblem>::iterator iter;
	for (iter = dataPool::global::g_problems.begin(); iter != dataPool::global::g_problems.end(); ++iter) {
		if (iter->oper == "i")
			problem_list.append(iter->name);
	}
	_exist_problem_table->setRowCount(problem_list.size());
	for (unsigned int m = 0; m < problem_list.size(); ++m) {
		_exist_problem_table->insert2table(m, 0, problem_list[m]);
	}
}

void problemManage::initRemoveProblem() {
	_remove_problem_table->clear();
	QStringList header2;
	header2 << "已经移除的天线问题";
	_remove_problem_table->setHorizontalHeaderLabels(header2);
	_remove_problem_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	QStringList problem_list;
	QVector<parsProblem>::iterator iter;
	for (iter = dataPool::global::g_problems.begin(); iter != dataPool::global::g_problems.end(); ++iter) {
		if (iter->oper == "d")
			problem_list.append(iter->name);
	}
	_remove_problem_table->setRowCount(problem_list.size());
	for (unsigned int m = 0; m < problem_list.size(); ++m) {
		_remove_problem_table->insert2table(m, 0, problem_list[m]);
	}
}

void problemManage::initLayout() {
	_layout = new QVBoxLayout;
	_manage_problem_layout = new QHBoxLayout;
	_manage_exist_problem_layout = new QVBoxLayout;
	_exist_problem_layout = new QHBoxLayout;
	_remove_problem_layout = new QHBoxLayout;
	_recovery_delete_button_layout = new QVBoxLayout;
	_add_button_layout = new QHBoxLayout;
	_confirm_cancel_button_layout = new QHBoxLayout;
	_add_new_problem_box = new QGroupBox;
	_add_new_problem_layout = new QVBoxLayout;
	_select_directory_layout = new QHBoxLayout;

	_exist_problem_layout->addWidget(_exist_problem_table);
	_exist_problem_layout->addWidget(_remove_button);	
	
	_recovery_delete_button_layout->addWidget(_recovery_button);
	_recovery_delete_button_layout->addWidget(_delete_button);
	_remove_problem_layout->addWidget(_remove_problem_table);
	_remove_problem_layout->addLayout(_recovery_delete_button_layout);
	
	_add_button_layout->addStretch();
	_add_button_layout->addWidget(_add_button);
	
	_confirm_cancel_button_layout->addStretch();
	_confirm_cancel_button_layout->addWidget(_confirm_button);
	_confirm_cancel_button_layout->addWidget(_cancel_button);

	//管理已有天线问题布局
	_manage_exist_problem_layout->addLayout(_exist_problem_layout);
	_manage_exist_problem_layout->addSpacing(20);
	_manage_exist_problem_layout->addLayout(_remove_problem_layout);
	_manage_exist_problem_layout->addSpacing(20);
	_manage_exist_problem_layout->addLayout(_add_button_layout);

	//新增天线问题布局	
	_select_directory_layout->addWidget(_select_directory_label);
	_select_directory_layout->addWidget(_select_directory_text);
	_select_directory_layout->addWidget(_select_directory_button);
	_add_new_problem_layout->addWidget(_readme_text);
	_add_new_problem_layout->addStretch();
	_add_new_problem_layout->addLayout(_select_directory_layout);
	_add_new_problem_layout->addStretch();
	_add_new_problem_box->setLayout(_add_new_problem_layout);

	_manage_problem_layout->addLayout(_manage_exist_problem_layout);
	_layout->addLayout(_manage_problem_layout);
	_layout->addSpacing(20);
	_layout->addLayout(_confirm_cancel_button_layout);
	_layout->setContentsMargins(10, 20, 10, 20);
	setLayout(_layout);
}

void problemManage::initReadMeText() {
	_readme_text->clear();
	QFont font;
	font.setPixelSize(14);
	_readme_text->setFont(font);
	_readme_text->setStyleSheet("border: none;");
	QDir hfss_template_dir("./DEA4AD/trunk/problem/wifiAntenna/");
	QString hfss_absolute_path = hfss_template_dir.absolutePath();
	QDir feko_template_dir("./DEA4AD/trunk/problem/GP_WiFiAntenna/");
	QString feko_absolute_path = feko_template_dir.absolutePath();
	_readme_text->append("<h1 >添加新天线问题</h1>");
	_readme_text->append("  在DEA4AD软件框架内，一个天线问题保存在一个文件夹内，问题文件夹内包含以下文件：");
	_readme_text->append("<h2>ANSOFT HFSS问题（<a href="+ hfss_absolute_path +" >参考模板</a>）：</h2>");
	_readme_text->append("<ol>1. ANSOFT HFSS 模型文件（wifiAntenna.hfss）；</ol>");
	_readme_text->append("<ol>2. ANSOFT HFSS vbs脚本文件（wifiAntenna.vbs）；</ol>");
	_readme_text->append("<ol>3. 天线问题编码文件（wifiAntenna.py）；</ul>");
	_readme_text->append("<ol>4. 天线问题配置参数文件（wifiAntenna_conf.py）；</ol>");
	_readme_text->append("<ol>5. 天线问题数据参数存储文件（wifiAntenna_conf.json）；</ol>");
	_readme_text->append("<h2>FEKO问题（<a href="+ feko_absolute_path +" >参考模板</a>）：</h2>");
	_readme_text->append("<ol>1. FEKO 模型文件（GP_WiFiAntenna.cfx）；</ol>");
	_readme_text->append("<ol>2. 天线问题编码文件（GP_WiFiAntenna.py）；</ol>");
	_readme_text->append("<ol>3. 天线问题配置参数文件（GP_WiFiAntenna_conf.py）；</ol>");
	_readme_text->append("<ol>4. 天线问题数据参数存储文件（GP_WiFiAntenna_conf.json）；</ol>");
	_readme_text->append("<h2>注意事项</h2>");
	_readme_text->append("  问题文件的命名规则，<font color = \"red\">要求文件夹名即为天线问题名</font>，文件夹内文件命名请参考模板问题。");
}

bool problemManage::isCorrectProblemFormat(const QDir dir) {
	QString antena_name = dir.dirName();
	QStringList files_list = dir.entryList();
	bool is_correct = files_list.contains(QString("%1.py").arg(antena_name)) &&
		files_list.contains(QString("%1_conf.py").arg(antena_name)) &&
		files_list.contains(QString("%1_conf.json").arg(antena_name));
	if (is_correct && files_list.contains(QString("%1.vbs").arg(antena_name)) &&
		files_list.contains(QString("%1.hfss").arg(antena_name))) {
		//ANSOFT HFSS problem
		_problem_type = 1;
		return true;
	}
	else if (is_correct && files_list.contains(QString("%1.cfx").arg(antena_name))) {
		//FEKO problem
		_problem_type = 2;
		return true;
	}
	return false;
}

void problemManage::add_antenna_problem(const QDir dir) {
	//添加天线问题，1. 拷贝问题文件到DEA4AD（判重）；2.插入新数据到problems.json
	QString antenna_name = dir.dirName();
	QDir DEA4AD_problem_dir(QString("%1/problem/").arg(dataPool::global::getGDEA4ADPath()));
	if (!DEA4AD_problem_dir.exists()) {
		QMessageBox::critical(0, QString("警告"), QString("DEA4AD路径被更改，请将DEA4AD添加到程序exe文件同层目录下！"));
		qCritical("DEA4AD路径被更改，请将DEA4AD添加到程序exe文件同层目录下！");
		return;
	}
	QString DEA4AD_problem_path = DEA4AD_problem_dir.absolutePath();
	QString new_problem_dir_path = QString("%1/%2/").arg(DEA4AD_problem_path).arg(antenna_name);
	QDir new_problem_dir(new_problem_dir_path);
	if (new_problem_dir.exists()) {
		QMessageBox::StandardButton rb = QMessageBox::question(NULL, "提示", "天线问题在框架内已存在，是否替换？", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (rb == QMessageBox::No)
			return;
		else
			DEA4AD_problem_dir.rmdir(antenna_name);
	}
	DEA4AD_problem_dir.mkdir(antenna_name);

	parsProblem new_problem;
	new_problem.name = antenna_name;
	new_problem.info = antenna_name;
	new_problem.oper = "i";
	new_problem.path = new_problem_dir_path;
	new_problem.type = _problem_type;
	//设置默认频率为最大频点（如果有）
	/*QJsonObject problem_obj = parseJson::getJsonObj(QString("%1/%2_conf.json").arg(tmp.path).arg(tmp.name));
	QJsonObject frequency_obj = parseJson::getSubJsonObj(problem_obj, "FreSetting");
	QStringList fre_end_list = dataPool::str2list(frequency_obj.value("FreEnd").toString().trimmed());
	double fre_end = 0.01;
	for (int i = 0; i < fre_end_list.size(); ++i) {
		if (fre_end < fre_end_list[i].toDouble())
			fre_end = fre_end_list[i].toDouble();
	}
	tmp.max_frequency = fre_end;*/

	
}

//slots
void problemManage::slot_existSelectChanged() {
	int select_number = _exist_problem_table->selectedItems().size();
	if (select_number == 0) {
		_remove_button->setEnabled(false);
		_exist_problem_table->clearSelection();
	}
	else {
		_remove_button->setEnabled(true);
	}
}
void problemManage::slot_removeSelectChanged() {
	int select_number = _remove_problem_table->selectedItems().size();
	if (select_number == 0) {
		_recovery_button->setEnabled(false);
		_delete_button->setEnabled(false);
		_remove_problem_table->clearSelection();
	}
	else {
		_recovery_button->setEnabled(true);
		_delete_button->setEnabled(true);
	}
}
void problemManage::slot_add(bool) {
	int add_flag = _add_button->property("add").toInt();
	if (add_flag == 0) {		
		initReadMeText();
		_select_directory_text->clear();
		_manage_problem_layout->addWidget(_add_new_problem_box);
		_add_button->setText("<<添加新问题");
		_add_button->setProperty("add", 1);
		setMinimumSize(SUBWINDOW_WIDTH, SUBWINDOW_HEIGHT);
	}
	else if (add_flag == 1) {
		QString antenna_path = _select_directory_text->text();
		if (!antenna_path.isEmpty()) {
			QDir dir(antenna_path);
			if (!isCorrectProblemFormat(dir)) {
				QMessageBox::critical(0, QString("警告"), QString("新增天线问题部分文件缺失，请参照模板文件检测后重新添加！"));
				qCritical("新增天线问题部分文件缺失");
				return;
			}
			//do some check
			add_antenna_problem(dir);
		}
		setMinimumSize(SUBWINDOW_WIDTH / 2, SUBWINDOW_HEIGHT);
		_add_new_problem_box->setParent(nullptr);
		_manage_problem_layout->removeWidget(_add_new_problem_box);
		_add_button->setText("添加新问题>>");
		resize(QSize(SUBWINDOW_WIDTH / 2, SUBWINDOW_HEIGHT));
		_add_button->setProperty("add", 0);
	}
}

void problemManage::slot_remove(bool) {
	int curr_row = _exist_problem_table->currentRow();
	if (curr_row != -1) {
		QString select_item = _exist_problem_table->item(curr_row, 0)->text().trimmed();
		int new_row_index = _remove_problem_table->rowCount();
		_remove_problem_table->insertRow(new_row_index);
		_remove_problem_table->insert2table(new_row_index, 0, select_item);
		_exist_problem_table->removeRow(curr_row);
		_exist_problem_table->clearSelection();
		qInfo("remove antenna problem : '%s'", qUtf8Printable(select_item));
	}
}
void problemManage::slot_reconvery(bool) {
	int curr_row = _remove_problem_table->currentRow();
	if (curr_row != -1) {
		QString select_item = _remove_problem_table->item(curr_row, 0)->text().trimmed();
		int new_row_index = _exist_problem_table->rowCount();
		_exist_problem_table->insertRow(new_row_index);
		_exist_problem_table->insert2table(new_row_index, 0, select_item);
		_remove_problem_table->removeRow(curr_row);
		_remove_problem_table->clearSelection();
		qInfo("recovery antenna problem : '%s'", qUtf8Printable(select_item));
	}
}
void problemManage::slot_delete(bool) {
	//删除天线问题->1.删除DEA4AD中的问题文件；2.删除problems.json中问题记录；3.删除associates.json中的相关性；
	int curr_row = _remove_problem_table->currentRow();
	if (curr_row != -1) {
		QString select_item = _remove_problem_table->item(curr_row, 0)->text().trimmed();
		//删除选中行
		QMessageBox::StandardButton rb = QMessageBox::question(NULL, "删除", "删除选中行?（注意不可恢复）", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (rb == QMessageBox::Yes) {	
			_remove_problem_table->removeRow(curr_row);
			_remove_problem_table->clearSelection();
			qInfo("delete antenna problem : '%s'", qUtf8Printable(select_item));
		}
	}
}

void problemManage::slot_select_Directory(bool) {
	QString dir = QFileDialog::getExistingDirectory(this, "打开文件夹", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
		QFileDialog::DontResolveSymlinks);
	_select_directory_text->setText(dir);
}

void problemManage::slot_confirm(bool) {

}
void problemManage::slot_cancel(bool) {
	this->close();
}

problemManage::~problemManage(){
	delete _layout;
	_layout = nullptr;
	/*delete _manage_problem_layout;
	_manage_problem_layout = nullptr;
	delete _manage_exist_problem_layout;
	_manage_exist_problem_layout = nullptr;
	delete _exist_problem_layout;
	_exist_problem_layout = nullptr;
	delete _remove_problem_layout;
	_remove_problem_layout = nullptr;
	delete _recovery_delete_button_layout;
	_recovery_delete_button_layout = nullptr;
	delete _add_button_layout;
	_add_button_layout = nullptr;
	delete _confirm_cancel_button_layout;
	_confirm_cancel_button_layout = nullptr;
	delete _add_new_problem_box;
	_add_new_problem_box = nullptr;
	delete _add_new_problem_layout;
	_add_new_problem_layout = nullptr;
	delete _select_directory_layout;
	_select_directory_layout = nullptr;
	delete _exist_problem_table;
	_exist_problem_table = nullptr;
	delete _remove_problem_table;
	_remove_problem_table = nullptr;*/
}