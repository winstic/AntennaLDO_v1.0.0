#include "atnlibrary.h"
#include "../Utility/global.h"
#include "../Utility/macrodefined.h"
#include "../Wizard/projectWizard.h"

atnLibrary::atnLibrary(int dock_width, int table_columns, QWidget *parent) : QWidget(parent) ,_atn_dock_WH(dock_width), 
_num_of_table_col(table_columns), _atn_problem(nullptr), _model_info(nullptr){
	_table_view = new QTableWidget(this);
	_table_view->setContextMenuPolicy(Qt::CustomContextMenu);
	_item_menu = new QMenu(this);	
	initMenu();
	initAtnCellList();
	atnLibraryLayout();
	//QMessageBox::information(this, "infomation", "atnlibrary:"+QString::number(geometry().width()));
	//qDebug() << sizeHint().width() << "," << sizeHint().height();
	connect(_table_view, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(slot_tableCellDoubleClick(int, int)));
	connect(_table_view, SIGNAL(cellPressed(int, int)), this, SLOT(slot_cellPressed(int, int)));
	connect(_table_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slot_customContextMenuRequested(QPoint)));
}

void atnLibrary::initMenu() {
	QAction act_new(QStringLiteral("新建工程"), this);
	QAction act_property(QStringLiteral("属性"), this);
	connect(&act_new, &QAction::triggered, this, &atnLibrary::slot_newProject);
	connect(&act_property, &QAction::triggered, this, &atnLibrary::slot_property);
	_item_menu->addAction(&act_new);
	_item_menu->addAction(&act_property);
}

void atnLibrary::initAtnCellList() {
	QVector<parsProblem*>::iterator iter;
	for (iter = dataPool::g_problems.begin(); iter != dataPool::g_problems.end(); ++iter) {
		antennaCell* atnCellTemp = new antennaCell(*iter);
		_atn_cell_list.append(atnCellTemp);
	}
}

void atnLibrary::atnLibraryLayout() {
	_table_view->clear();
	int num_of_atn = _atn_cell_list.size();
	if (num_of_atn > 0) {
		_num_of_table_col = (num_of_atn < _num_of_table_col) ? num_of_atn : _num_of_table_col;
		QVector<int> cell_width_vec(_num_of_table_col);
		initAtnCellWidth(cell_width_vec);
		int num_of_Row = num_of_atn / _num_of_table_col;
		if (num_of_atn % _num_of_table_col)
			num_of_Row += 1;

		_table_view->setColumnCount(_num_of_table_col);
		_table_view->setRowCount(num_of_Row);
		for (int i = 0; i < num_of_Row; ++i) {
			_table_view->setRowHeight(i, CELLATNHT);
		}
		for (int j = 0; j < _num_of_table_col; ++j) {
			_table_view->setColumnWidth(j, cell_width_vec.at(j));
		}
		for (int k = 0; k < num_of_atn; ++k) {
			_table_view->setCellWidget(k / _num_of_table_col, k % _num_of_table_col, _atn_cell_list.at(k));
		}
	}
	QLabel *plusPhoto = new QLabel;
	QPixmap plusMap = QPixmap("./images/plus.png");
	plusPhoto->setPixmap(plusMap);
	_table_view->setCellWidget(num_of_atn / _num_of_table_col, num_of_atn % _num_of_table_col, plusPhoto);
}

// set antenna width
void atnLibrary::initAtnCellWidth(QVector<int>& cell_width_vec) {
	if (cell_width_vec.size() <= 0) {
		qWarning("setting wrong parameter of table column.");
		return;
	}
	for (int i = 0; i < _num_of_table_col; ++i) {
		cell_width_vec[i] = _atn_dock_WH / _num_of_table_col;
	}
	int restPixNum = _atn_dock_WH % _num_of_table_col;
	for (int j = 0; j < restPixNum; ++j) {
		++cell_width_vec[_num_of_table_col - 1 - j];
	}
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
			bool is_success = true;

			//create project dir
			QDir *dir = new QDir();
			dir->mkdir(working_path);

			//copy antenna problem
			QString path_suffix = QString("%1/%2").arg(working_path).arg(_atn_problem->name);
			QString project_problem_path = QString("%1_conf.json").arg(path_suffix);
			QString project_hfss_path = QString("%1.hfss").arg(path_suffix);
			QString project_feko_path = QString("%1.cfx").arg(path_suffix);
			QString project_script_path = QString("%1_design.vbs").arg(path_suffix);

			path_suffix = QString("%1/%2").arg(_atn_problem->path).arg(_atn_problem->name);
			is_success &= dataPool::copyFile(QString("%1_conf.json").arg(path_suffix), project_problem_path);
			if (is_success && (_atn_problem->type == HFSS)) {
				is_success &= dataPool::copyFile(QString("%1.hfss").arg(path_suffix), project_hfss_path);
				is_success &= dataPool::copyFile(QString("%1_design.vbs").arg(path_suffix), project_script_path);
			}
			else if (is_success && (_atn_problem->type == FEKO)) {
				is_success &= dataPool::copyFile(QString("%1.cfx").arg(path_suffix), project_feko_path);
			}

			if (!is_success) {
				qCritical("created project failed.");
				dir->rmdir(working_path);
				delete dir;
				dir = nullptr;
				return;
			}
			qInfo("successfully copy related files.");

			//writen project file(.rel)
			QFile inFile(working_path + "/" + rel_file);
			inFile.open(QIODevice::WriteOnly);
			QTextStream out(&inFile);
			out << "PID:" << _atn_problem->id << endl;
			out << "PROBLEM:" << _atn_problem->name << endl;			
			inFile.close();
			qInfo("successfully create project.");
			//memory leak
			delete dir;
			dir = nullptr;

			emit signal_createAndParseXML(QString("%1/%2.xml").arg(working_path).arg(project_name), _atn_problem);
		}
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
	if (_table_view->indexAt(pos).model())
		_item_menu->exec(QCursor::pos());
}

void atnLibrary::slot_newProject() {
	newProject();
}

void atnLibrary::slot_property() {
	_model_info = new modelInfo(_atn_problem);
	//mf->setAttribute(Qt::WA_DeleteOnClose);
	_model_info->exec();
	qInfo(dataPool::str2char(QString("scan [%1] antenna model info.").arg(_atn_problem->name)));
}

void atnLibrary::slot_clickSearchButton() {}
void atnLibrary::slot_searchTextChange(QString searchText) {
	//update antenna cell list
	_atn_cell_list.clear();
	QVector<parsProblem*>::iterator iter;
	for (iter = dataPool::g_problems.begin(); iter != dataPool::g_problems.end(); ++iter) {
		//we can use kmp algorithm instead
		if ((*iter)->name.contains(searchText) || (*iter)->info.contains(searchText)) {
			antennaCell* atn_cell_temp = new antennaCell(*iter);
			_atn_cell_list.append(atn_cell_temp);
		}
	}
	atnLibraryLayout();
}


atnLibrary::~atnLibrary() {
	//qt支持父子控件，一旦atnLibrary实例析构后，会自动调用内部控件的析构函数
	//delete table_view;
	//delete search_label;
	//delete item_menu;
	//delete action_new;
	//delete action_property;
	for (QList<antennaCell*>::iterator iter = _atn_cell_list.begin(); iter != _atn_cell_list.end(); ++iter) {
		if ((*iter) != nullptr) {
			delete (*iter);
			//(*iter) = nullptr;
		}
	}
	
	delete _model_info;
	_model_info = nullptr;
}