#pragma execution_character_set("utf-8")
#include "treeView.h"
#include "../Utility/macrodefined.h"
#include "../Utility/parseJson.h"
#include "../Antenna/goRun.h"

treeModel::treeModel(QWidget* parent) : QTreeView(parent), _model_info(nullptr), _is_running(false){
	_pro_tree = new QTreeView(this);
	_project_menu = new QMenu(this);
	_atn_design_menu = new QMenu(this);
	_performance_menu = new QMenu(this);
	_item_performance_menu = new QMenu(this);
	_act_design_run = nullptr;
	_act_design_stop = nullptr;
	_curr_item_index = new QModelIndex();
	
	initMenu();
	initIcon();

	connect(_pro_tree, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(slot_customContextMenuRequested(const QPoint&)));
	connect(_pro_tree, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(slot_doubleClicked(const QModelIndex&)));
	connect(_pro_tree, SIGNAL(clicked(const QModelIndex&)), this, SLOT(slot_clicked(const QModelIndex&)));
}

void treeModel::initIcon() {
	_icon_map[QStringLiteral("treeNode")] = QIcon(QStringLiteral("./images/node.png"));
	_icon_map[QStringLiteral("treeItem")] = QIcon(QStringLiteral("./images/item.png"));
}

QTreeView* treeModel::getTreeWidget() {
	return _pro_tree;
}

bool treeModel::writeXMLFile(const QString &file_name, parsProblem* atn_problem) {
	_atn_problem = atn_problem;
	QFile file(file_name);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return false;
	QTextStream out(&file);
	QDomDocument doc;
	QDomElement root, element, design_element, performance_element;
	QDomText text;
	QDomProcessingInstruction instruction = doc.createProcessingInstruction("xml", "version = \'1.0\' encoding = \'UTF-8\'");
	doc.appendChild(instruction);

	//project
	root = doc.createElement("project");
	root.setAttribute("name", _atn_problem->name);
	doc.appendChild(root);

	element = doc.createElement("item");
	text = doc.createTextNode("关于天线");
	element.appendChild(text);
	element.setAttribute("flag", "viewOnly");
	root.appendChild(element);

	design_element = doc.createElement("node");
	design_element.setAttribute("name", "设计");
	design_element.setAttribute("flag", "design");
	root.appendChild(design_element);

	performance_element = doc.createElement("node");
	performance_element.setAttribute("name", "性能参数");
	performance_element.setAttribute("flag", "performance");
	design_element.appendChild(performance_element);

	element = doc.createElement("item");
	text = doc.createTextNode("几何参数");
	element.setAttribute("flag", "geometry");
	element.appendChild(text);
	design_element.appendChild(element);

	element = doc.createElement("item");
	text = doc.createTextNode("算法参数");
	element.setAttribute("flag", "algorithm");
	element.appendChild(text);
	design_element.appendChild(element);

	QJsonObject problem_obj = parseJson::getJsonObj(QString("%1/%2_conf.json").arg(_atn_problem->path).arg(_atn_problem->name));
	QJsonObject frequency_obj = parseJson::getSubJsonObj(problem_obj, "FreSetting");
	QStringList str_list = dataPool::str2list(frequency_obj.value("FreStart").toString().trimmed());
	for (int i = 0; i < str_list.size(); ++i) {
		element = doc.createElement("item");
		text = doc.createTextNode(QString("频段%1").arg(i+1));
		element.appendChild(text);
		performance_element.appendChild(element);
	}

	out.setCodec("UTF-8");
	doc.save(out, 4);   //4 spaces
	qInfo("generate xml file.");
	return true;
}

//外部接口
bool treeModel::updateXMLFile(const QString &file_name, const QStandardItem *item, const QStandardItem *child) {
	//!get xml root node
	QFile in_file(file_name);
	if (!in_file.open(QFile::ReadOnly | QFile::Text)) {
		qCritical("cannot read file: '%s'.", qUtf8Printable(file_name));
		return false;
	}
	QDomDocument doc;
	QString error;
	int row, column;
	if (!doc.setContent(&in_file, false, &error, &row, &column)) {
		qCritical("error parse xml file: '%s' at row-%d, column-%d: %s.", qUtf8Printable(file_name), row, column, qUtf8Printable(error));
		return false;
	}
	in_file.close();
	QDomElement xml_root = doc.documentElement();
	if ("project" != xml_root.tagName()) {
		qCritical("its not a project root node.");
		return false;
	}
	//!
	
	QDomNode root_child = xml_root.firstChild();
	QDomElement root_child_element;
	while (!root_child.isNull()) {
		root_child_element = root_child.toElement();
		if (("node" == root_child_element.tagName()) && root_child_element.hasAttribute("flag")) {
			if (MARK_NODE_DESIGN == item->data(ROLE_MARK_NODE) && "performance" == root_child_element.attribute("flag")) {
				QDomElement design_element = doc.createElement("item");
				QDomText design_text = doc.createTextNode(child->text());
				design_element.appendChild(design_text);
				root_child_element.appendChild(design_element);
				break;
			}
		}
		root_child = root_child.nextSibling();
	}


	QFile out_file(file_name);
	if (!out_file.open(QIODevice::WriteOnly))
		return false;
	QTextStream out(&out_file);
	out.setCodec("utf-8");
	doc.save(out, 4);
	qInfo("update something in xml file.");
	return true;
}

//外部接口
bool treeModel::parseXML(const QString &file_name, parsProblem* atn_problem) {
	_atn_problem = atn_problem;
	QFile file(file_name);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		qCritical("cannot read file: '%s'.", qUtf8Printable(file_name));
		return false;
	}
	QDomDocument doc;
	QString error;
	int row, column;
	if (!doc.setContent(&file, false, &error, &row, &column)) {
		qCritical("error parse xml file: '%s' at row-%d, column-%d: %s.", qUtf8Printable(file_name), row, column, qUtf8Printable(error));
		return false;
	}
	file.close();
	QDomElement xml_root = doc.documentElement();
	if ("project" != xml_root.tagName()) {
		qCritical("its not a project root node.");
		return false;
	}

	QStandardItemModel* tree_model = new QStandardItemModel(_pro_tree);
	QStringList header;
	header << xml_root.attribute("name");
	tree_model->setHorizontalHeaderLabels(header);

	QString project_name = dataPool::global::getGProjectName();
	QStandardItem* tree_root = new QStandardItem(_icon_map[QStringLiteral("treeNode")], project_name);
	tree_root->setData(MARK_PROJECT, ROLE_MARK);
	tree_model->appendRow(tree_root);
	parseProjectElement(xml_root, tree_root);

	_pro_tree->setModel(tree_model);
	_pro_tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
	_pro_tree->setContextMenuPolicy(Qt::CustomContextMenu);        //设置右键菜单
	_pro_tree->expandAll();
	return true;
}

void treeModel::parseProjectElement(const QDomElement &element, QStandardItem* parent) {
	QDomNode child = element.firstChild();
	QDomElement child_element;
	while (!child.isNull()) {
		child_element = child.toElement();
		if ("node" == child_element.tagName()) {
			parseNodeElement(child_element, parent);
		}
		else if ("item" == child_element.tagName()) {
			parseItemElement(child_element, parent);
		}
		child = child.nextSibling();
	}
}

void treeModel::parseNodeElement(const QDomElement &element, QStandardItem *parent) {
	QStandardItem *item = new QStandardItem(_icon_map[QStringLiteral("treeNode")], element.attribute("name"));
	if (element.hasAttribute("flag") && "design" == element.attribute("flag")) {
		item->setData(MARK_NODE, ROLE_MARK);
		item->setData(MARK_NODE_DESIGN, ROLE_MARK_NODE);
	}
	else if (element.hasAttribute("flag") && "performance" == element.attribute("flag")) {
		item->setData(MARK_NODE, ROLE_MARK);
		item->setData(MARK_NODE_PERFORMANCE, ROLE_MARK_NODE);
	}
	parent->appendRow(item);

	//traverse node
	QDomNode child = element.firstChild();
	QDomElement child_element;
	while (!child.isNull()) {
		child_element = child.toElement();
		if ("node" == child_element.tagName()) {
			parseNodeElement(child_element, item);
		}
		else if ("item" == child_element.tagName()) {
			parseItemElement(child_element, item);
		}
		child = child.nextSibling();
	}
}

void treeModel::parseItemElement(const QDomElement &element, QStandardItem *parent) {
	QStandardItem *item = new QStandardItem(_icon_map[QStringLiteral("treeItem")], element.text());
	if (element.hasAttribute("flag") && "viewOnly" == element.attribute("flag")) {
		item->setData(MARK_ITEM, ROLE_MARK);
		item->setData(MARK_ITEM_OPENFILE, ROLE_MARK_ITEM);
	}
	else if (element.hasAttribute("flag") && "geometry" == element.attribute("flag")) {
		item->setData(MARK_ITEM, ROLE_MARK);
		item->setData(MARK_ITEM_GEOMETRYDESIGN, ROLE_MARK_ITEM);
	}
	else if (element.hasAttribute("flag") && "algorithm" == element.attribute("flag")) {
		item->setData(MARK_ITEM, ROLE_MARK);
		item->setData(MARK_ITEM_ALGORITHMDESIGN, ROLE_MARK_ITEM);
	}
	else if (MARK_NODE_PERFORMANCE == parent->data(ROLE_MARK_NODE)) {
		item->setData(MARK_ITEM, ROLE_MARK);
		item->setData(MARK_ITEM_PERFORMANCEDESIGN, ROLE_MARK_ITEM);
	}
	parent->appendRow(item);
}

void treeModel::initMenu() {
	QAction* act_close = new QAction("关闭", _pro_tree);
	act_close->setEnabled(false);
	QAction* act_del = new QAction("删除", _pro_tree);
	act_del->setEnabled(false);
	//connect(act_del, &QAction::triggered, this, &treeModel::slot_del);

	QAction* act_design_copy = new QAction("复制", _pro_tree);
	act_design_copy->setEnabled(false);
	_act_design_run = new QAction("运行", _pro_tree);	
	connect(_act_design_run, &QAction::triggered, this, &treeModel::slot_run);
	_act_design_stop = new QAction("停止", _pro_tree);
	_act_design_stop->setEnabled(false);
	connect(_act_design_stop, &QAction::triggered, this, &treeModel::slot_stopRun);
	QAction* act_design_del = new QAction("删除", _pro_tree);
	act_design_del->setEnabled(false);

	QAction* act_performance_add = new QAction("新增", _pro_tree);
	act_performance_add->setEnabled(false);

	QAction* act_performance_item_del = new QAction("删除", _pro_tree);

	_project_menu->addAction(act_close);
	_project_menu->addAction(act_del);

	_atn_design_menu->addAction(act_design_copy);
	_atn_design_menu->addAction(_act_design_run);
	_atn_design_menu->addAction(_act_design_stop);
	_atn_design_menu->addSeparator();
	_atn_design_menu->addAction(act_design_del);

	_performance_menu->addAction(act_performance_add);
	_item_performance_menu->addAction(act_performance_item_del);
}

void treeModel::openFile() {
	_model_info = new modelInfo(_atn_problem, this);
	//mf->setModal(true);
	_model_info->exec();
}

void treeModel::modifyGeometryVariables() {
	geometryModel* geometry_model = new geometryModel(_atn_problem, _is_running);
	//connect(this, SIGNAL(signal_calculate(bool)), geometry_model, SLOT(slot_checkRunning(bool)));	
	geometry_model->exec();
}

void treeModel::modefyAlgorithmParameters() {
	QJsonObject global_obj = parseJson::getJsonObj(dataPool::global::getGCurrentGlobalJsonPath());
	if (global_obj.isEmpty()) {
		qCritical("get global json object field.");
		QMessageBox::critical(0, QString("警告"), QString("读取全局配置文件失败！"));
		return;
	}
	parsAlgorithm* palgorithm = dataPool::global::getAlgorithmByName(global_obj.value("ALGORITHM_NAME").toString().trimmed());
	algorithmModel* algorithm_model = new algorithmModel(_atn_problem, palgorithm, global_obj, _is_running);
	algorithm_model->exec();
}

void treeModel::modifyPerformanceParameters(unsigned int index) {
	performanceTab* performance_tab = new performanceTab(_atn_problem, index, _is_running);
	performance_tab->exec();
}

// slot function
//create project tree by xml file
void treeModel::slot_creatProTreeByXML(QString& path, parsProblem* atn_problem) {
	//该槽函数是treeView与AntennaLibrary的唯一接口
	writeXMLFile(path, atn_problem);
	parseXML(path, atn_problem);
	qInfo("create project tree.");
}

//mouse right click
void treeModel::slot_customContextMenuRequested(const QPoint &pos) {
	_curr_item_index = &_pro_tree->indexAt(pos);
	//qDebug() << currentIndex.data().toString();
	QVariant var_node = _curr_item_index->data(ROLE_MARK);
	QVariant var_folder = _curr_item_index->data(ROLE_MARK_NODE);
	QVariant var_item = _curr_item_index->data(ROLE_MARK_ITEM);
	int var_int = var_node.toInt();
	int var_item_int = var_item.toInt();
	if (var_node.isValid()) {
		if (var_int == MARK_PROJECT)
			_project_menu->exec(QCursor::pos()); //弹出右键菜单，菜单位置为光标位置
		else if (var_folder.isValid() && var_int == MARK_NODE) {
			if (var_folder.toInt() == MARK_NODE_DESIGN)
				_atn_design_menu->exec(QCursor::pos());
			else if (var_folder.toInt() == MARK_NODE_PERFORMANCE)
				_performance_menu->exec(QCursor::pos());
		}
		else if (var_item.isValid() && var_int == MARK_ITEM) {
			if (var_item_int == MARK_ITEM_PERFORMANCEDESIGN)
				_item_performance_menu->exec(QCursor::pos());
		}
	}
}

void treeModel::slot_showAll() {
	_pro_tree->expandAll();
}

void treeModel::slot_hideAll() {
	_pro_tree->collapseAll();
}

void treeModel::slot_doubleClicked(const QModelIndex& item_index) {
	QVariant var_item = item_index.data(ROLE_MARK_ITEM);
	int index = item_index.row();
	if (var_item.isValid()) {
		int item_int = var_item.toInt();
		if (item_int == MARK_ITEM_GEOMETRYDESIGN) {
			//编辑几何结构参数
			modifyGeometryVariables();
		}
		else if (item_int == MARK_ITEM_ALGORITHMDESIGN) {
			//编辑算法参数
			modefyAlgorithmParameters();
		}
		else if (item_int == MARK_ITEM_PERFORMANCEDESIGN) {
			//编辑性能参数（频段）
			if(index >= 0)
				modifyPerformanceParameters(index);
		}
		else if (item_int == MARK_ITEM_OPENFILE) {
			openFile();
		}
	}
}

void treeModel::slot_clicked(const QModelIndex& item_index) {
	_curr_item_index = const_cast<QModelIndex*>(&item_index);
}

void treeModel::slot_run() {
	optRunProcess = new QProcess();
	connect(optRunProcess, SIGNAL(readyRead()), this, SLOT(slot_readyRead()));
	_act_design_run->setEnabled(false);
	_act_design_stop->setEnabled(true);
	_is_running = true;
	emit signal_calculate(true);
	goRun *oRun = new goRun(optRunProcess);
	oRun->start();
}

void treeModel::slot_stopRun() {
	//optRunProcess->close();
	//delete optRunProcess;
	//optRunProcess = nullptr;
	_act_design_run->setEnabled(true);
	_act_design_stop->setEnabled(false);
	_is_running = false;
	emit signal_calculate(false);
	QDir dir(QDir::currentPath());
	QString ostopPath = QString("%1/DEA4AD/trunk/end.bat").arg(dir.path());
	QProcess* p = new QProcess();
	//"/c" mean close cmd window after execute .bat file.
	p->start("cmd.exe", QStringList() << "/c" << ostopPath);
	p->waitForFinished();
}

//optimize run process to read pipe
void treeModel::slot_readyRead() {
	QString text = optRunProcess->readAll();
	//outputDock->show();
	//QTextEdit* output = outputDock->widget()->findChild<QTextEdit *>();
	emit signal_outputMessage(text);
	//qDebug() << text;
}