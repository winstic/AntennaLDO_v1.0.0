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
	design_element.setAttribute("name", dataPool::global::getGCurrentSpecName());
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

	QJsonObject problem_obj = parseJson::getJsonObj(QString("%1/%2/%3_conf.json").arg(dataPool::global::getGWorkingProjectPath()).arg(dataPool::global::getGCurrentSpecName()).arg(_atn_problem->name));
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
bool treeModel::updateXMLFile(const QString &file_name, const QString &node_attribute, const QStandardItem *item, const QString &oper) {
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
	QDomElement target;
	findXMLNodeElement(xml_root, target, node_attribute);
	if (target.isNull()) {
		qCritical("没有找到待插入的node结点（结点属性值不对应，请检查xml文件中具体的flag值）。");
		return false;
	}
	if (oper == "i") {
		QDomElement add_element = doc.createElement("item");
		QDomText add_text = doc.createTextNode(item->text());
		add_element.appendChild(add_text);
		target.appendChild(add_element);
	}
	if (oper == "d") {
		target.removeChild(target.lastChild());
	}
	

	QFile out_file(file_name);
	if (!out_file.open(QIODevice::WriteOnly))
		return false;
	QTextStream out(&out_file);
	out.setCodec("utf-8");
	doc.save(out, 4);
	return true;
}
void treeModel::findXMLNodeElement(QDomElement &element, QDomElement &target, const QString &node_attribute){	
	//traverse node
	QDomNode child = element.firstChild();
	QDomElement child_element;
	while (!child.isNull()) {
		child_element = child.toElement();
		QString test1 = child_element.tagName();
		if ((child_element.tagName().compare("node") == 0) && child_element.hasAttribute("flag")) {
			QString test_flag = child_element.attribute("flag");
			if(child_element.attribute("flag").compare(node_attribute) == 0)
				target = child_element;
			else
				findXMLNodeElement(child_element, target, node_attribute);
		}
		child = child.nextSibling();
	}
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
	connect(act_performance_add, &QAction::triggered, this, &treeModel::slot_addPerFormanceSetting);

	QAction* act_performance_item_del = new QAction("删除", _pro_tree);
	connect(act_performance_item_del, &QAction::triggered, this, &treeModel::slot_delPerFormanceSetting);

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
	delete _model_info;
	_model_info = nullptr;
}

void treeModel::modifyGeometryVariables() {
	geometryModel* geometry_model = new geometryModel(_atn_problem, _is_running);
	//connect(this, SIGNAL(signal_calculate(bool)), geometry_model, SLOT(slot_checkRunning(bool)));	
	geometry_model->exec();
	delete geometry_model;
	geometry_model = nullptr;
}

void treeModel::modefyAlgorithmParameters() {
	QJsonObject global_obj = parseJson::getJsonObj(QString("%1/%2/global_conf.json").arg(dataPool::global::getGWorkingProjectPath()).arg(dataPool::global::getGCurrentSpecName()));
	if (global_obj.isEmpty()) {
		return;
	}
	parsAlgorithm* palgorithm = dataPool::global::getAlgorithmByName(global_obj.value("ALGORITHM_NAME").toString().trimmed());
	algorithmModel* algorithm_model = new algorithmModel(_atn_problem, palgorithm, global_obj, _is_running);
	algorithm_model->exec();
	delete algorithm_model;
	algorithm_model = nullptr;
}

int treeModel::modifyPerformanceParameters(unsigned int index) {
	performanceTab* performance_tab = new performanceTab(_atn_problem, index, _is_running);
	int exe_id = performance_tab->exec();
	delete performance_tab;
	performance_tab = nullptr;
	return exe_id;
}

bool treeModel::addDelAntennaPerformanceCore(QJsonObject& obj, const QStringList fre_end_list, const int flag, const QString oper, const int index) {
	//用flag标记选择解析设置参数方法
	unsigned int length = fre_end_list.size();
	if (length < 1) {
		qCritical("没有设置频段，(至少要设置一个频段).");
		return false;
	}
	double new_fre_start = _atn_problem->max_frequency + 1;
	for (QJsonObject::iterator iter = obj.begin(); iter != obj.end(); ++iter) {
		QStringList tmp_list;
		if (flag == 0) {
			tmp_list = dataPool::str2list(iter.value().toString().simplified());
		}
		else if (flag == 1) {
			tmp_list = dataPool::stack2list(iter.value().toString().simplified());
		}
		else {
			qCritical("无效的flag参数, 请仔细核对。");
			return false;
		}
		if (tmp_list.size() != length) {
			qCritical("问题json文件性能参数数据设置不对应, 请仔细核对。");
			return false;
		}
		if (oper == "i") {
			//添加新频段
			if (iter.key().simplified().compare("FreStart") == 0)
				tmp_list.append(QString::number(new_fre_start));
			else if (iter.key().simplified().compare("FreEnd") == 0)
				tmp_list.append(QString::number(new_fre_start + 100));
			else
				tmp_list.append(tmp_list.at(length - 1));
		}
		else if (oper == "d") {
			//删除编号为index的频段
			if (index < 0 || index >= length) {
				qCritical("index参数越界, 请仔细核对。");
				return false;
			}
			tmp_list.removeAt(index);
		}
		else {
			qCritical("无效的oper参数, 请仔细核对。");
			return false;
		}
		obj.insert(iter.key(), QString("[%1]").arg(tmp_list.join(",")));
	}
	return true;
}
bool treeModel::addDelAntennaPerformance(const QString oper, const int index) {
	QJsonObject problem_obj = parseJson::getJsonObj(QString("%1/%2/%3_conf.json").arg(dataPool::global::getGWorkingProjectPath()).arg(dataPool::global::getGCurrentSpecName()).arg(_atn_problem->name));
	if (problem_obj.isEmpty()) return false;
	QJsonObject frequency_obj = parseJson::getSubJsonObj(problem_obj, "FreSetting");
	QJsonObject far_field_obj = parseJson::getSubJsonObj(problem_obj, "ThetaPhiStep");
	QJsonObject gain_obj = parseJson::getSubJsonObj(problem_obj, "GainSetting");
	QJsonObject axial_obj = parseJson::getSubJsonObj(problem_obj, "AxialratioSetting");
	QJsonObject loss_obj = parseJson::getSubJsonObj(problem_obj, "VSWRSetting");
	if (frequency_obj.isEmpty() || far_field_obj.isEmpty() || gain_obj.isEmpty() || axial_obj.isEmpty() || loss_obj.isEmpty()) 
		return false;
	QStringList fre_end_list = dataPool::str2list(frequency_obj.value("FreEnd").toString().simplified());	

	bool is_update = true;
	is_update &= addDelAntennaPerformanceCore(frequency_obj, fre_end_list, 0, oper, index);
	is_update &= addDelAntennaPerformanceCore(far_field_obj, fre_end_list, 0, oper, index);
	is_update &= addDelAntennaPerformanceCore(gain_obj, fre_end_list, 1, oper, index);
	is_update &= addDelAntennaPerformanceCore(axial_obj, fre_end_list, 1, oper, index);
	is_update &= addDelAntennaPerformanceCore(loss_obj, fre_end_list, 1, oper, index);
	if (is_update) {
		problem_obj.insert("FreSetting", frequency_obj);
		problem_obj.insert("ThetaPhiStep", far_field_obj);
		problem_obj.insert("GainSetting", gain_obj);
		problem_obj.insert("AxialratioSetting", axial_obj);
		problem_obj.insert("VSWRSetting", loss_obj);
		if (!parseJson::write(QString("%1/%2/%3_conf.json").arg(dataPool::global::getGWorkingProjectPath()).arg(dataPool::global::getGCurrentSpecName()).arg(_atn_problem->name), &problem_obj)){
			qCritical("无法保存性能参数信息到问题json文件。");
			//QMessageBox::critical(0, QString("警告"), QString("无法保存性能参数信息到问题json文件，详情查阅日志。"));
			return false;
		}
		//更新最大频率
		QStringList fre_start_list = dataPool::str2list(frequency_obj.value("FreStart").toString().simplified());
		QStringList fre_end_list = dataPool::str2list(frequency_obj.value("FreEnd").toString().simplified());
		int length = fre_start_list.size();
		if (fre_end_list.size() != length) {
			qCritical("%s天线问题json文件频率数据设置有误, 请仔细核对。", qUtf8Printable(_atn_problem->name));
			return false;
		}
		QVector<freRange> tfr;
		_atn_problem->frequencyRanges.swap(tfr);
		for (int i = 0; i < length; ++i) {
			freRange tmpfr(fre_start_list[i].toDouble(), fre_end_list[i].toDouble());
			_atn_problem->frequencyRanges.append(tmpfr);
		}
		_atn_problem->fillMaxFrequency();
		qInfo("最大频率: '%s'.", qUtf8Printable(QString::number(_atn_problem->max_frequency)));
	}
	return is_update;
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

void treeModel::slot_addPerFormanceSetting() {
	QStandardItemModel *itemModel = const_cast<QStandardItemModel *>(
		static_cast<const QStandardItemModel *>(_curr_item_index->model()));
	QStandardItem *item = itemModel->itemFromIndex(*_curr_item_index);
	if (!addDelAntennaPerformance("i")) { 
		QMessageBox::critical(0, QString("警告"), QString("无法添加新频段，详情查阅日志。"));
		return; 
	}
	bool is_add = false;
	int curr_index = item->rowCount();
	if ((modifyPerformanceParameters(curr_index) == QDialog::Accepted)) {
		QVariant var_node = (*_curr_item_index).data(ROLE_MARK_NODE);
		if (var_node.isValid()) {
			if (var_node.toInt() == MARK_NODE_PERFORMANCE) {
				QString performance_name = QString("频段%1").arg(curr_index + 1);
				QStandardItem *child = new QStandardItem(_icon_map[QStringLiteral("treeItem")], performance_name);
				child->setData(MARK_ITEM, ROLE_MARK);
				child->setData(MARK_ITEM_PERFORMANCEDESIGN, ROLE_MARK_ITEM);				
				if (updateXMLFile(QString("%1/%2.xml").arg(dataPool::global::getGWorkingProjectPath()).arg(dataPool::global::getGProjectName()), "performance", child, "i")) {
					item->appendRow(child);
					is_add = true;
					qInfo("增加频段 : %s", qUtf8Printable(QString("[%1,%2]").arg(_atn_problem->frequencyRanges[curr_index].first)
						.arg(_atn_problem->frequencyRanges[curr_index].second)));
				}
				else {
					qCritical("更新工程目录结构到xml文件失败。");		
					is_add = false;
					QMessageBox::critical(0, QString("警告"), QString("无法添加新频段，详情查阅日志。"));
				}
			}
		}
	}	
	if (!is_add) {
		addDelAntennaPerformance("d", curr_index);
	}
}

void treeModel::slot_delPerFormanceSetting() {
	QStandardItemModel *itemModel = const_cast<QStandardItemModel *>(
		static_cast<const QStandardItemModel *>(_curr_item_index->model()));
	QStandardItem *item = itemModel->itemFromIndex(*_curr_item_index);
	QStandardItem *del_parent = item->parent();
	int counts = del_parent->rowCount();
	int del_row = _curr_item_index->row();
	if (counts <= 1) {
		qWarning("必须设置频段，只有一个频段参数不能删除");
		return;
	}
	double delete_pre_start = _atn_problem->frequencyRanges[del_row].first;
	double delete_pre_end = _atn_problem->frequencyRanges[del_row].second;
	QMessageBox::StandardButton rb = QMessageBox::question(NULL, "删除", QString("删除频段[%1,%2]?").arg(delete_pre_start).arg(delete_pre_end), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (rb == QMessageBox::Yes) {
		if (updateXMLFile(QString("%1/%2.xml").arg(dataPool::global::getGWorkingProjectPath()).arg(dataPool::global::getGProjectName()), "performance", item, "d")) {
			if (!addDelAntennaPerformance("d", del_row)) {
				QMessageBox::critical(0, QString("警告"), QString("无法删除该频段，详情查阅日志。"));
				return;
			}
			qInfo("删除频段 : %s", qUtf8Printable(QString("[%1,%2]").arg(delete_pre_start).arg(delete_pre_end)));
			//删除时每次将最后的一个编号删除，以此来同步工程树结构下的编号
			del_parent->removeRow(counts - 1);
		}
		else {
			qCritical("更新工程目录结构到xml文件失败。");
			QMessageBox::critical(0, QString("警告"), QString("无法删除该频段，详情查阅日志。"));
			return;
		}
	}
}

void treeModel::slot_run() {
	QString current_spec_path = QString("%1/%2").arg(dataPool::global::getGWorkingProjectPath()).arg(dataPool::global::getGCurrentSpecName());
	//校验global_conf.json
	QJsonObject global_obj = parseJson::getJsonObj(QString("%1/global_conf.json").arg(current_spec_path));
	if (global_obj.isEmpty() || global_obj.value("ALGORITHM_NAME").toString().isEmpty()) {
		qCritical("请先设置算法。");
		QMessageBox::critical(0, QString("警告"), QString("请先设置算法。"));
		return;
	}
	//更新global_conf.json
	if (!dataPool::copyFile(QString("%1/global_conf.json").arg(current_spec_path), QString("%1/global_conf.json").arg(dataPool::global::getGDEA4ADPath()))) {
		qCritical("update global_json file failed.");
		return;
	}
	//更新proble json
	if (!dataPool::copyFile(QString("%1/%2_conf.json").arg(current_spec_path).arg(_atn_problem->name),
		QString("%1/%2_conf.json").arg(_atn_problem->path).arg(_atn_problem->name))) {
		qCritical("update global_json file failed.");
		return;
	}
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