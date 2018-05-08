#pragma execution_character_set("utf-8")
#include "atnlibrary.h"
#include "../Utility/global.h"
#include "../Utility/macrodefined.h"
#include "../Utility/parseJson.h"
#include "../Wizard/projectWizard.h"

atnLibrary::atnLibrary(QWidget *parent) : QWidget(parent), _atn_problem(nullptr), _model_info(nullptr){
	_table_view = new QTableWidget();
	_table_view->setContextMenuPolicy(Qt::CustomContextMenu);
	_table_view->verticalHeader()->setVisible(false);
	_table_view->horizontalHeader()->setVisible(false);
	_table_view->setSelectionMode(QAbstractItemView::SingleSelection);
	_table_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
	_table_view->setStyleSheet("selection-background-color:white;"); //设置选中背景色
	_table_view->setShowGrid(false);

	_item_menu = new QMenu;	
	_act_new = nullptr;
	initMenu();
	initAtnCellList();
	atnLibraryLayout();
	connect(_table_view, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(slot_tableCellDoubleClick(int, int)));
	connect(_table_view, SIGNAL(cellPressed(int, int)), this, SLOT(slot_cellPressed(int, int)));
	connect(_table_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slot_customContextMenuRequested(QPoint)));
}

void atnLibrary::initMenu() {
	_act_new = new QAction("新建工程", this);
	QAction* act_property = new QAction("属性", this);
	connect(_act_new, &QAction::triggered, this, &atnLibrary::slot_newProject);
	connect(act_property, &QAction::triggered, this, &atnLibrary::slot_property);
	_item_menu->addAction(_act_new);
	_item_menu->addAction(act_property);
}

void atnLibrary::initAtnCellList() {
	QVector<parsProblem>::iterator iter;
	for (iter = dataPool::global::g_problems.begin(); iter != dataPool::global::g_problems.end(); ++iter) {
		if (iter->oper == "i") {
			antennaCell* atnCellTemp = new antennaCell(iter);
			_atn_cell_list.append(atnCellTemp);
		}
	}
}

void atnLibrary::atnLibraryLayout() {
	_table_view->clear();
	int cell_size = (dataPool::global::getGWindowWidth() - PROJECT_TREE_WIDTH - 10) / NUM_ANTENNA_COLUMN;
	cell_size = (cell_size < MIN_CELL_ATNWH) ? MIN_CELL_ATNWH : cell_size;
	int num_of_atn = _atn_cell_list.size();
	if (num_of_atn > 0) {
		int num_of_Row = num_of_atn / NUM_ANTENNA_COLUMN;
		if (num_of_atn % NUM_ANTENNA_COLUMN)
			num_of_Row += 1;

		_table_view->setColumnCount(NUM_ANTENNA_COLUMN);
		_table_view->setRowCount(num_of_Row);
		for (int i = 0; i < num_of_Row; ++i) {
			_table_view->setRowHeight(i, cell_size);
		}
		for (int j = 0; j < NUM_ANTENNA_COLUMN; ++j) {
			_table_view->setColumnWidth(j, cell_size);
		}
		for (int k = 0; k < num_of_atn; ++k) {
			_table_view->setCellWidget(k / NUM_ANTENNA_COLUMN, k % NUM_ANTENNA_COLUMN, _atn_cell_list.at(k));
		}
	}
	//QLabel *plusPhoto = new QLabel;
	//QPixmap plusMap = QPixmap("images/plus.png");
	//plusPhoto->setPixmap(plusMap);
	//_table_view->setCellWidget(num_of_atn / NUM_ANTENNA_COLUMN, num_of_atn % NUM_ANTENNA_COLUMN, plusPhoto);
}

QTableWidget* atnLibrary::getTableWidget() const {
	if (_table_view == nullptr)
		qCritical("uninitialize antenna cell list");
	return _table_view;
}

void atnLibrary::newProject() {
	if (_atn_problem != nullptr) {
		projectWizard* wizard = new projectWizard(_atn_problem, this);
		//wizard->setAttribute(Qt::WA_DeleteOnClose);
		if (wizard->exec() == 1) {			
			QString working_path = dataPool::global::getGWorkingProjectPath();
			QString project_name = dataPool::global::getGProjectName();

			QString rel_file = QString("%1.rel").arg(project_name);
			QString default_spec_path = QString("%1/Spec1").arg(working_path);

			//create project dir
			QDir *dir = new QDir();
			dir->mkdir(working_path);
			dir->mkdir(default_spec_path);

			if (! dataPool::copyFile(QString("%1/global_conf.json").arg(dataPool::global::getGDEA4ADPath()),
				QString("%1/global_conf.json").arg(working_path))) {
				qCritical("loss global_json file.");
				QMessageBox::critical(0, QString("Error"), QString("缺失global_json.json文件。"));
				return;
			}
			/*if (!dataPool::copyFile(QString("%1/%2_conf.json").arg(_atn_problem->path).arg(_atn_problem->name),
				QString("%1/%2_conf.json").arg(working_path).arg(_atn_problem->name))) {
				qCritical("loss problem_json file.");
				QMessageBox::critical(0, QString("Error"), QString("缺失problem_json.json文件。"));
				return;
			}*/
			dataPool::global::setGCurrentGlobalJsonPath(QString("%1/global_conf.json").arg(working_path));
			//dataPool::global::setGCurrentProblemJsonPath(QString("%1/%2_conf.json").arg(working_path).arg(_atn_problem->name));

			dir->mkdir(QString("%1/outfilepath").arg(working_path));
			if (_atn_problem->type == "HFSS") {
				dir->mkdir(QString("%1/outhfsspath").arg(working_path));
			}
			if (_atn_problem->type == "FEKO") {
				if (!dataPool::copyFile(QString("%1/%2.cfx").arg(_atn_problem->path).arg(_atn_problem->name),
					QString("%1/outfilepath/%2.cfx").arg(working_path).arg(_atn_problem->name))) {
					qCritical("缺失feko模型文件。");
					QMessageBox::critical(0, QString("Error"), QString("缺失feko模型文件。"));
					return;
				}
			}
			//writen project file(.rel)
			QFile inFile(working_path + "/" + rel_file);
			inFile.open(QIODevice::WriteOnly);
			QTextStream out(&inFile);
			out << PROBLEM_NAME << ":" << _atn_problem->name << endl;			
			inFile.close();
		
			QJsonObject global_obj = parseJson::getJsonObj(dataPool::global::getGCurrentGlobalJsonPath());
			if (global_obj.isEmpty()) {
				return;
			}
			global_obj.insert("PROBLEM_NAME", _atn_problem->name);
			global_obj.insert("outfilepath", QString("%1/outfilepath").arg(dataPool::global::getGWorkingProjectPath()));
			global_obj.insert("outhfsspath", QString("%1/outhfsspath").arg(dataPool::global::getGWorkingProjectPath()));
			if (!parseJson::write(dataPool::global::getGCurrentGlobalJsonPath(), &global_obj)){
				qCritical("save failed in global json.");
				QMessageBox::critical(0, QString("Error"), QString("global_json 格式错误。"));
				delete dir;
				dir = nullptr;
				delete wizard;
				wizard = nullptr;
				return;
			}

			qInfo("successfully create project.");
			//memory leak
			delete dir;
			dir = nullptr;

			emit signal_createAndParseXML(QString("%1/%2.xml").arg(working_path).arg(project_name), _atn_problem);
		}
		delete wizard;
		wizard = nullptr;
	}
}

void atnLibrary::slot_reSizeSlot(int row, int column) {
	/*int x = 0, y = 0, n = 0;
	for (int i = 0; i < row; ++i) {
		y = i * (MBNTICON_HT + MBNTDIS) + MBNTDIS;
		for (int j = 0; j < column; ++j) {
			x = j * (MBNTICON_WH + MBNTDIS) + MBNTDIS;
			if (n >= NUMOFANTENNA) break;
			//mListBtn.at(n++)->move(x, y);
		}
	}*/
}

void atnLibrary::slot_tableCellDoubleClick(int row, int col) {
	QWidget* current_widget = _table_view->cellWidget(row, col);
	if (current_widget) {
		antennaCell* cellTab = static_cast<antennaCell* >(current_widget);
		_atn_problem = cellTab->getAntennaProblem();
		newProject();
	}
}

//when the mouse button press
void atnLibrary::slot_cellPressed(int row, int col) {
	QWidget* currentWidget = _table_view->cellWidget(row, col);
	if (currentWidget) {
		antennaCell* cellTab = static_cast<antennaCell* >(currentWidget);
		_atn_problem = cellTab->getAntennaProblem();
	}
}

void atnLibrary::slot_customContextMenuRequested(QPoint pos) {
	int row = _table_view->indexAt(pos).row();
	int column = _table_view->indexAt(pos).column();
	int index = row * _table_view->columnCount() + column;
	if (index < _atn_cell_list.size())
		_item_menu->exec(QCursor::pos());
}

void atnLibrary::slot_newProject() {
	newProject();
}

void atnLibrary::slot_property() {
	_model_info = new modelInfo(_atn_problem);
	//mf->setAttribute(Qt::WA_DeleteOnClose);
	_model_info->exec();
	qInfo("scan '%s' antenna model info.", qUtf8Printable(_atn_problem->name));
	delete _model_info;
	_model_info = nullptr;
}

void atnLibrary::slot_searchTextChange(QString searchText) {
	//update antenna cell list
	_atn_cell_list.clear();
	QVector<parsProblem>::iterator iter;
	for (iter = dataPool::global::g_problems.begin(); iter != dataPool::global ::g_problems.end(); ++iter) {
		//we can use kmp algorithm instead
		if (iter->oper == "i" && (iter->name.contains(searchText) || iter->info.contains(searchText))) {
			antennaCell* atn_cell_temp = new antennaCell(iter);
			_atn_cell_list.append(atn_cell_temp);
		}
	}
	atnLibraryLayout();
}

void atnLibrary::slot_checkNewProject(bool flag) {
	if (flag) {
		_act_new->setEnabled(false);
		disconnect(_table_view, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(slot_tableCellDoubleClick(int, int)));
	}
	else {
		_act_new->setEnabled(true);
		connect(_table_view, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(slot_tableCellDoubleClick(int, int)));
	}
}

atnLibrary::~atnLibrary() {
	//qt支持父子控件，一旦atnLibrary实例析构后，会自动调用内部控件的析构函数
	//delete table_view;
	//delete search_label;
	//delete item_menu;
	//delete action_new;
	//delete action_property;

	/*for (QList<antennaCell*>::iterator iter = _atn_cell_list.begin(); iter != _atn_cell_list.end(); ++iter) {
		if ((*iter) != nullptr) {
			delete (*iter);
			//(*iter) = nullptr;
		}
	}
	
	delete _model_info;
	_model_info = nullptr;*/
}