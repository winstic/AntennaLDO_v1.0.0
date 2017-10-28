#include "treeView.h"
#include "../Utility/macrodefined.h"
#include "../Utility/parseJson.h"
#include "../Wizard/designWizard.h"
#include "../Wizard/optimizeWizard.h"
#include "designtab.h"
#include "optimizetab.h"
#include "designRun.h"
#include "optimizeRun.h"

treeModel::treeModel(QWidget* parent) : QTreeView(parent), _atn_problem(nullptr), _model_info(nullptr){
	_pro_tree = new QTreeView(this);
	_project_menu = new QMenu(this);
	_atn_design_menu = new QMenu(this);
	_atn_optimize_menu = new QMenu(this);
	_item_design_menu = new QMenu(this);
	_item_optimize_menu = new QMenu(this);
	_item_view_menu = new QMenu(this);
	_result_menu = new QMenu(this);
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
	QDomElement root, element;
	QDomText text;
	QDomProcessingInstruction instruction = doc.createProcessingInstruction("xml", "version = \'1.0\' encoding = \'UTF-8\'");
	doc.appendChild(instruction);

	//project
	root = doc.createElement("project");
	root.setAttribute("name", _atn_problem->name);
	doc.appendChild(root);

	element = doc.createElement("item");
	text = doc.createTextNode("模型简介");
	element.appendChild(text);
	element.setAttribute("flag", "viewOnly");
	root.appendChild(element);

	element = doc.createElement("node");
	element.setAttribute("name", "设计");
	element.setAttribute("flag", "design");
	root.appendChild(element);

	element = doc.createElement("node");
	element.setAttribute("name", "优化");
	element.setAttribute("flag", "optimize");
	root.appendChild(element);

	element = doc.createElement("node");
	element.setAttribute("name", "结果查看");
	element.setAttribute("flag", "result");
	root.appendChild(element);

	out.setCodec("UTF-8");
	doc.save(out, 4);   //4 spaces
	qInfo("generate xml file.");
	return true;
}

bool treeModel::updateXMLFile(const QString &file_name, const QStandardItem *item, const QStandardItem *child) {
	//!get xml root node
	QFile in_file(file_name);
	if (!in_file.open(QFile::ReadOnly | QFile::Text)) {
		qCritical(dataPool::str2char(QString("cannot read file [%1].").arg(file_name)));
		return false;
	}
	QDomDocument doc;
	QString error;
	int row, column;
	if (!doc.setContent(&in_file, false, &error, &row, &column)) {
		qFatal(dataPool::str2char(QString("error parse xml [%1] at row-%2, column-%3: %4.").arg(file_name)
			.arg(row).arg(column).arg(error)));
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
			if ((MARK_NODE_DESIGN == item->data(ROLE_MARK_NODE) && "design" == root_child_element.attribute("flag")) ||
				(MARK_NODE_OPTIMIZE == item->data(ROLE_MARK_NODE) && "optimize" == root_child_element.attribute("flag"))) {
				QDomElement design_element = doc.createElement("item");
				QDomText design_text = doc.createTextNode(child->text());
				design_element.appendChild(design_text);
				design_element.setAttribute("id", root_child_element.childNodes().count() + 1);
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

bool treeModel::parseXML(const QString &file_name, parsProblem* atn_problem) {
	_atn_problem = atn_problem;
	QFile file(file_name);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		qCritical(dataPool::str2char(QString("cannot read file [%1].").arg(file_name)));
		return false;
	}
	QDomDocument doc;
	QString error;
	int row, column;
	if (!doc.setContent(&file, false, &error, &row, &column)) {
		qFatal(dataPool::str2char(QString("error parse xml [%1] at row-%2, column-%3: %4.").arg(file_name)
			.arg(row).arg(column).arg(error)));
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

	delete tree_root;
	tree_root = nullptr;
	delete tree_model;
	tree_model = nullptr;
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
	else if (element.hasAttribute("flag") && "optimize" == element.attribute("flag")) {
		item->setData(MARK_NODE, ROLE_MARK);
		item->setData(MARK_NODE_OPTIMIZE, ROLE_MARK_NODE);
	}
	else {
		item->setData(MARK_NODE, ROLE_MARK);
		item->setData(MARK_NODE_RESULT, ROLE_MARK_NODE);
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
	else if (MARK_NODE_DESIGN == parent->data(ROLE_MARK_NODE)) {
		item->setData(MARK_ITEM, ROLE_MARK);
		item->setData(MARK_ITEM_ATNDESIGN, ROLE_MARK_ITEM);
	}
	else if (MARK_NODE_OPTIMIZE == parent->data(ROLE_MARK_NODE)) {
		item->setData(MARK_ITEM, ROLE_MARK);
		item->setData(MARK_ITEM_ATNOPTIMIZE, ROLE_MARK_ITEM);
	}
	parent->appendRow(item);
}

void treeModel::initMenu() {
	QAction* act_close = new QAction(QStringLiteral("关闭"), _pro_tree);
	QAction* act_del = new QAction(QStringLiteral("删除"), _pro_tree);
	connect(act_del, &QAction::triggered, this, &treeModel::slot_del);
	QAction* act_hide_all = new QAction(QStringLiteral("全部折叠"), _pro_tree);
	connect(act_hide_all, &QAction::triggered, this, &treeModel::slot_hideAll);
	QAction* act_show_all = new QAction(QStringLiteral("全部展开"), _pro_tree);
	connect(act_show_all, &QAction::triggered, this, &treeModel::slot_showAll);
	QAction* act_add_design = new QAction(QStringLiteral("添加设计"), _pro_tree);
	connect(act_add_design, &QAction::triggered, this, &treeModel::slot_addDesign);
	QAction* act_add_optimize = new QAction(QStringLiteral("添加优化"), _pro_tree);
	connect(act_add_optimize, &QAction::triggered, this, &treeModel::slot_addOptimize);
	//design run
	QAction* act_design_run = new QAction(QStringLiteral("运行"), _pro_tree);
	connect(act_design_run, &QAction::triggered, this, &treeModel::slot_designRun);
	//optimize run
	QAction* act_optimize_run = new QAction(QStringLiteral("运行"), _pro_tree);
	connect(act_optimize_run, &QAction::triggered, this, &treeModel::slot_optimizeRun);
	QAction* act_interrupt = new QAction(QStringLiteral("暂停"), _pro_tree);
	connect(act_interrupt, &QAction::triggered, this, &treeModel::slot_interrupt);
	QAction* act_design_stop = new QAction(QStringLiteral("终止"), _pro_tree);
	connect(act_design_stop, &QAction::triggered, this, &treeModel::slot_designStop);
	QAction* act_optimize_stop = new QAction(QStringLiteral("终止"), _pro_tree);
	connect(act_optimize_stop, &QAction::triggered, this, &treeModel::slot_optimizeStop);
	QAction* act_open_file = new QAction(QStringLiteral("打开"), _pro_tree);
	connect(act_open_file, &QAction::triggered, this, &treeModel::slot_openFile);
	QAction* act_modify_design_var = new QAction(QStringLiteral("修改参数"), _pro_tree);
	connect(act_modify_design_var, &QAction::triggered, this, &treeModel::slot_modifyDesignVar);
	QAction* act_modify_optimize_var = new QAction(QStringLiteral("修改参数"), _pro_tree);
	connect(act_modify_optimize_var, &QAction::triggered, this, &treeModel::slot_modifyOptimizeVar);
	QAction* act_show_result = new QAction(QStringLiteral("结果查看"), _pro_tree);
	//actShowResult->setEnabled(false);
	connect(act_show_result, &QAction::triggered, this, &treeModel::slot_showResult);

	_project_menu->addAction(act_close);
	_project_menu->addAction(act_del);
	_project_menu->addSeparator();
	_project_menu->addAction(act_hide_all);
	_project_menu->addAction(act_show_all);

	_atn_design_menu->addAction(act_add_design);

	_atn_optimize_menu->addAction(act_add_optimize);

	_item_view_menu->addAction(act_open_file);

	_item_design_menu->addAction(act_modify_design_var);
	_item_design_menu->addSeparator();
	_item_design_menu->addAction(act_design_run);
	_item_design_menu->addAction(act_interrupt);
	_item_design_menu->addAction(act_design_stop);
	_item_design_menu->addSeparator();
	_item_design_menu->addAction(act_del);
	_item_design_menu->addAction(act_show_result);

	_item_optimize_menu->addAction(act_modify_optimize_var);
	_item_optimize_menu->addSeparator();
	_item_optimize_menu->addAction(act_optimize_run);
	_item_optimize_menu->addAction(act_interrupt);
	_item_optimize_menu->addAction(act_optimize_stop);
	_item_optimize_menu->addSeparator();
	_item_optimize_menu->addAction(act_del);
	_item_optimize_menu->addAction(act_show_result);
}

QList<QStandardItem*> treeModel::getRoots() {
	QList<QStandardItem*> roots;
	QStandardItemModel* tree_model = new QStandardItemModel(_pro_tree);
	for (int i = 0; i < tree_model->rowCount(); ++i) {
		roots.append(tree_model->item(i));
	}
	delete tree_model;
	tree_model = nullptr;
	return roots;
}

QStandardItem* treeModel::getProNode() {
	QList<QStandardItem*> roots = getRoots();
	for (auto i = roots.begin(); i != roots.end(); ++i) {
		if ((*i)->data(ROLE_MARK) == MARK_PROJECT) {
			return (*i);
		}
	}
	return nullptr;
}


QList<QStandardItem*> treeModel::getFolderNode() {
	QList<QStandardItem*> folder_nodes;
	QStandardItem* project = getProNode();
	if (project == nullptr) {
		folder_nodes.append(nullptr);
	}
	for (int i = 0; i < project->rowCount(); ++i) {
		QStandardItem* child = project->child(i);
		QVariant var = child->data(ROLE_MARK_NODE);
		if (!var.isValid()) {
			continue;
		}
		folder_nodes.append(child);
	}
	return folder_nodes;
}

// slot function
//create project tree by xml file
void treeModel::slot_creatProTreeByXML(QString& path, parsProblem* atn_problem) {
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
			else if (var_folder.toInt() == MARK_NODE_OPTIMIZE)
				_atn_optimize_menu->exec(QCursor::pos());
			else
				_result_menu->exec(QCursor::pos());
		}
		else if (var_item.isValid() && var_int == MARK_ITEM) {
			if (var_item_int == MARK_ITEM_OPENFILE)
				_item_view_menu->exec(QCursor::pos());
			else if (var_item_int == MARK_ITEM_ATNDESIGN) {
				int design_index = _curr_item_index->row() + 1;
				//update current design path
				QString current_design_path = QString("%1/design%2").arg(dataPool::global::getGWorkingProjectPath()).arg(design_index);
				dataPool::global::setGCurrentDesignPath(current_design_path);
				_item_design_menu->exec(QCursor::pos());
				qInfo(dataPool::str2char(QString("current design path change to [%1]").arg(current_design_path)));
			}
			else if (var_item_int == MARK_ITEM_ATNOPTIMIZE) {
				int optimize_index = _curr_item_index->row() + 1;
				//update current optimize path
				QString current_optimize_path = QString("%1/optimize%2").arg(dataPool::global::getGWorkingProjectPath()).arg(optimize_index);
				dataPool::global::setGCurrentOptimizePath(current_optimize_path);
				_item_optimize_menu->exec(QCursor::pos());
				qInfo(dataPool::str2char(QString("current optimize path change to [%1]").arg(current_optimize_path)));
			}
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
	int d_o_index = item_index.row() + 1;
	if (var_item.isValid()) {
		int item_int = var_item.toInt();
		if (item_int == MARK_ITEM_ATNDESIGN) {
			//update current design path
			QString current_design_path = QString("%1/design%2").arg(dataPool::global::getGWorkingProjectPath()).arg(d_o_index);
			dataPool::global::setGCurrentDesignPath(current_design_path);
			slot_modifyDesignVar();
			qInfo(dataPool::str2char(QString("current design path change to [%1]").arg(current_design_path)));
		}
		else if (item_int == MARK_ITEM_ATNOPTIMIZE) {
			//update current optimize path
			QString current_optimize_path = QString("%1/optimize%2").arg(dataPool::global::getGWorkingProjectPath()).arg(d_o_index);
			dataPool::global::setGCurrentOptimizePath(current_optimize_path);
			slot_modifyOptimizeVar();
			qInfo(dataPool::str2char(QString("current optimize path change to [%1]").arg(current_optimize_path)));
		}
		else if (item_int == MARK_ITEM_OPENFILE)
			slot_openFile();
	}
}

void treeModel::slot_clicked(const QModelIndex& item_index) {
	_curr_item_index = const_cast<QModelIndex*>(&item_index);
}

void treeModel::slot_addDesign() {	
	if (_curr_item_index == nullptr) {
		qCritical("do not select item model.");
		return;
	}
	QStandardItemModel *item_model = const_cast<QStandardItemModel *>(
		static_cast<const QStandardItemModel *>(_curr_item_index->model()));
	QStandardItem *item = item_model->itemFromIndex(*_curr_item_index);
	QVariant var_node = _curr_item_index->data(ROLE_MARK_NODE);
	if (var_node.isValid()) {
		if (MARK_NODE_DESIGN == var_node.toInt()) {
			QString working_path = dataPool::global::getGWorkingProjectPath();
			QString json_path = QString("%1/%2_conf.json").arg(working_path).arg(_atn_problem->name);
			QJsonObject obj = parseJson::getJsonObj(json_path);
			if (obj.isEmpty()) {
				qCritical("get json object field.");
				return;
			}
			designWizard *wizard = new designWizard(_atn_problem, obj, this);
			if (wizard->exec() == 1) {
				QString design_name = QString("设计%1").arg(item->rowCount() + 1);
				QDir* dir = new QDir();
				QString design_path = QString("%1/design%2").arg(working_path).arg(item->rowCount() + 1);

				QStandardItem *child = new QStandardItem(_icon_map[QStringLiteral("treeItem")], design_name);
				child->setData(MARK_ITEM, ROLE_MARK);
				child->setData(MARK_ITEM_ATNDESIGN, ROLE_MARK_ITEM);
				item->appendRow(child);
				dir->mkdir(design_path);

				//copy files(.json, module file) in designDir from projectDir
				bool is_success = true;
				is_success &= dataPool::copyFile(QString("%1/%2_conf.json").arg(working_path).arg(_atn_problem->name),
					QString("%1/%2_conf.json").arg(design_path).arg(_atn_problem->name));
				if(is_success && _atn_problem->type == HFSS)
					is_success &= dataPool::copyFile(QString("%1/%2_design.vbs").arg(working_path).arg(_atn_problem->name),
						QString("%1/%2_design.vbs").arg(design_path).arg(_atn_problem->name));
				if (!is_success) {
					qCritical(dataPool::str2char(QString("create sub-project [%1] failed").arg(design_path)));
					dir->rmdir(design_path);
					delete dir;
					dir = nullptr;
					return;
				}
				//update current design path
				dataPool::global::setGCurrentDesignPath(design_path);
				qInfo(dataPool::str2char(QString("current design path change to [%1]").arg(design_path)));
				//update json file
				parseJson::write(QString("%1/%2_conf.json").arg(design_path).arg(_atn_problem->name), &obj);
				//update xml file
				updateXMLFile(QString("%1/%2.xml").arg(working_path).arg(dataPool::global::getGProjectName()), item, child);
				delete dir;
				dir = nullptr;
			}
		}
	}
}

void treeModel::slot_addOptimize() {
	QStandardItemModel *item_model = const_cast<QStandardItemModel *>(
		static_cast<const QStandardItemModel *>(_curr_item_index->model()));
	QStandardItem *item = item_model->itemFromIndex(*_curr_item_index);
	QVariant var_node = _curr_item_index->data(ROLE_MARK_NODE);
	if (var_node.isValid()) {
		if (MARK_NODE_OPTIMIZE == var_node.toInt()) {
			QString working_path = dataPool::global::getGWorkingProjectPath();
			QString problem_json_path = QString("%1/%2_conf.json").arg(working_path).arg(_atn_problem->name);			
			QJsonObject problem_obj, algorithm_obj;	//do not know which algorithm right now.
			problem_obj = parseJson::getJsonObj(problem_json_path);
			if (problem_obj.isEmpty()) {
				qCritical("get json object field.");
				return;
			}
			//pass by reference or point
			parsAlgorithm* pars_algorithm = 0;
			optimizeWizard *wizard = new optimizeWizard(_atn_problem, problem_obj, algorithm_obj, pars_algorithm, this);
			if (wizard->exec() == 1) {
				//json obj already updated.
				QString optimize_name = QString("优化%1").arg(item->rowCount() + 1);
				QDir *dir = new QDir();
				QString optimize_path = QString("%1/optimize%2").arg(working_path).arg(item->rowCount() + 1);
				QStandardItem *child = new QStandardItem(_icon_map[QStringLiteral("treeItem")], optimize_name);
				child->setData(MARK_ITEM, ROLE_MARK);
				child->setData(MARK_ITEM_ATNOPTIMIZE, ROLE_MARK_ITEM);
				item->appendRow(child);
				dir->mkdir(optimize_path);

				QJsonObject global_obj = parseJson::getJsonObj(QString("%1/global_conf.json").arg(dataPool::global::getGDEA4ADPath()));
				bool is_success = true;
				//copy files(.json..,) in optimizeDir					
				is_success &= (dataPool::copyFile(QString("%1/%2_conf.json").arg(working_path).arg(_atn_problem->name),	QString("%1/%2_conf.json").arg(optimize_path).arg(_atn_problem->name)) &&
					dataPool::copyFile(QString("%1/global_conf.json").arg(dataPool::global::getGDEA4ADPath()), QString("%1/global_conf.json").arg(optimize_path)) &&
					dataPool::copyFile(QString("%1/%2_conf.json").arg(pars_algorithm->path).arg(pars_algorithm->name), QString("%1/%2_conf.json").arg(optimize_path).arg(pars_algorithm->name)));
				if (is_success) {
					dir->mkdir(QString("%1/outfilepath").arg(optimize_path));
					global_obj.insert("outfilepath", QString("%1/outfilepath").arg(optimize_path));
					if (_atn_problem->type == HFSS) {
						dir->mkdir(QString("%1/outhfsspath").arg(optimize_path));
						global_obj.insert("outhfsspath", QString("%1/outhfsspath").arg(optimize_path));
					}
					else if (_atn_problem->type == FEKO)
						is_success &= dataPool::copyFile(QString("%1/%2.cfx").arg(working_path).arg(_atn_problem->name),
							QString("%1/outfilepath/%2.cfx").arg(optimize_path).arg(_atn_problem->name));
					global_obj.insert("ALGORITHM_NAME", pars_algorithm->name);
					global_obj.insert("PROBLEM_NAME", _atn_problem->name);

				}
				if (!is_success) {
					qCritical(dataPool::str2char(QString("create sub-project [%1] failed").arg(optimize_path)));
					dir->rmdir(optimize_path);
					delete dir;
					dir = nullptr;
					return;
				}
				//update current optimize path
				dataPool::global::setGCurrentOptimizePath(optimize_path);
				qInfo(dataPool::str2char(QString("current optimize path change to [%1]").arg(optimize_path)));
				//update json file
				parseJson::write(QString("%1/global_conf.json").arg(optimize_path), &global_obj);
				parseJson::write(QString("%1/%2_conf.json").arg(optimize_path).arg(_atn_problem->name), &problem_obj);
				parseJson::write(QString("%1/%2_conf.json").arg(optimize_path).arg(pars_algorithm->name), &algorithm_obj);
				//update xml file
				updateXMLFile(QString("%1/%2.xml").arg(working_path).arg(dataPool::global::getGProjectName()), item, child);					
				delete dir;
				dir = nullptr;
			}
		}
	}
}

void treeModel::slot_openFile() {
	_model_info = new modelInfo(_atn_problem, this);
	//mf->setModal(true);
	_model_info->exec();
}

void treeModel::slot_modifyDesignVar() {
	QString json_path = QString("%1/%2_conf.json").arg(dataPool::global::getGCurrentDesignPath()).arg(_atn_problem->name);
	QJsonObject obj = parseJson::getJsonObj(json_path);
	if (obj.isEmpty()) {
		qCritical(dataPool::str2char(QString("something wrong in file [%1]").arg(json_path)));
		QMessageBox::critical(0, QString("Error"), QString("error:something wrong in file [%1]").arg(json_path));
		return;
	}
	designTab dTab(_atn_problem, obj, this);
	//dTab->setAttribute(Qt::WA_DeleteOnClose);
	//dTab->setModal(true);
	dTab.exec();
}

void treeModel::slot_modifyOptimizeVar() {
	QString global_json_path = QString("%1/global_conf.json").arg(dataPool::global::getGCurrentOptimizePath());
	QString problem_json_path = QString("%1/%2_conf.json").arg(dataPool::global::getGCurrentOptimizePath()).arg(_atn_problem->name);
	QJsonObject global_obj, problem_obj;
	global_obj = parseJson::getJsonObj(global_json_path);
	problem_obj  = parseJson::getJsonObj(problem_json_path);
	if (global_obj.isEmpty() || problem_obj.isEmpty()) {
		qCritical(dataPool::str2char(QString("something wrong in file [%1] or [%2]").arg(global_json_path).arg(problem_json_path)));
		QMessageBox::critical(0, QString("Error"), QString("error:something wrong in file [%1] or [%2]").arg(global_json_path).arg(problem_json_path));
		return;
	}
	parsAlgorithm* palgorithm = dataPool::getAlgorithmByName(global_obj.value("ALGORITHM_NAME").toString().trimmed());
	optimizeTab *otab = new optimizeTab(_atn_problem, problem_obj, palgorithm, this);
	//otab->setAttribute(Qt::WA_DeleteOnClose);
	//otab->setModal(true);
	otab->exec();
}

void treeModel::slot_designRun() {
	QJsonObject obj = parseJson::getJsonObj(QString("%1/%2_conf.json").arg(dataPool::global::getGCurrentDesignPath()).
		arg(_atn_problem->name));
	designRun *d_run = new designRun(_atn_problem, obj);
	d_run->start();
}

void treeModel::slot_optimizeRun() {
	QString global_json_path = QString("%1/global_conf.json").arg(dataPool::global::getGCurrentOptimizePath());
	QJsonObject global_obj = parseJson::getJsonObj(global_json_path);
	if (global_obj.isEmpty()) {
		qCritical(dataPool::str2char(QString("something wrong in file [%1]").arg(global_json_path)));
		QMessageBox::critical(0, QString("Error"), QString("error:something wrong in file [%1]").arg(global_json_path));
		return;
	}
	parsAlgorithm* palgorithm = dataPool::getAlgorithmByName(global_obj.value("ALGORITHM_NAME").toString().trimmed());
	optRunProcess = new QProcess(0);
	connect(optRunProcess, SIGNAL(readyRead()), this, SLOT(slot_readyRead()));
	optimizeRun *oRun = new optimizeRun(_atn_problem, palgorithm, optRunProcess);
	oRun->start();
}

void treeModel::slot_interrupt() {}

void treeModel::slot_designStop() {}

void treeModel::slot_optimizeStop() {
	QDir dir(QDir::currentPath());
	QString o_stop_path = QString("%1/DEA4AD/trunk/end.bat").arg(dir.path());
	//QString ostopPath = QString("./DEA4AD/trunk/end.bat");
	qDebug() << o_stop_path;
	QProcess p(0);
	//"/c" mean close cmd window after execute .bat file.
	p.execute("cmd.exe", QStringList() << "/c" << o_stop_path);
	p.waitForFinished();
	qDebug() << p.readAllStandardOutput();
}

void treeModel::slot_showResult() {
	QString hfss_path = QString("%1/%2.hfss").arg(dataPool::global::getGCurrentDesignPath()).arg(_atn_problem->name);
	QProcess p(0);;
	int is_ok = p.execute("hfss", QStringList() << hfss_path);
	if (is_ok != 0)
		qCritical(dataPool::str2char(QString("cannot open hfss-path [%1]").arg(hfss_path)));
	p.waitForFinished();
	qDebug() << QString::fromLocal8Bit(p.readAllStandardError());
}

void treeModel::slot_del() {
	//QStandardItemModel *itemM
	QProcess p;
	p.start("tasklist");
	//p.waitForStarted();
	p.waitForFinished();
	QString str = QString::fromLocal8Bit(p.readAllStandardOutput());
	qDebug() << str;
}

//optimize run process to read pipe
void treeModel::slot_readyRead() {
	QString text = optRunProcess->readAll();
	//outputDock->show();
	//QTextEdit* output = outputDock->widget()->findChild<QTextEdit *>();
	emit signal_outputMessage(text);
	//qDebug() << text;
}