#pragma once

#include <QtWidgets>
#include <QMenu>
#include <QDomDocument>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include "../Antenna/modeInfo.h"
#include "../Antenna/geometryModel.h"
#include "../Antenna/algorithmModel.h"
#include "../Antenna/performanceModel.h"

class treeModel : public QTreeView {
	Q_OBJECT
public:
	explicit treeModel(QWidget* parent = 0);
	QTreeView* getTreeWidget();
	//oper = "i" means add new item; oper = "d" means delete item;
	bool updateXMLFile(const QString &file_name, const QString &node_attribute, const QStandardItem *item, const QString &oper);
	bool parseXML(const QString& file_name, parsProblem* atn_problem);
	~treeModel() {}

private:
	void parseProjectElement(const QDomElement &element, QStandardItem* parent);
	void parseNodeElement(const QDomElement &element, QStandardItem *parent);
	void parseItemElement(const QDomElement &element, QStandardItem *parent);	
	bool writeXMLFile(const QString& file_name, parsProblem* atn_problem);
	void findXMLNodeElement(QDomElement &element, QDomElement &target, const QString &node_attribute);

	bool addDelAntennaPerformanceCore(QJsonObject& obj, const QStringList fre_end_list, const int flag, const QString oper, const int index = -1);
	bool addDelAntennaPerformance(const QString oper, const int index = -1);

	int modifyPerformanceParameters(unsigned int index = 0);

	void initMenu();
	void initIcon();

	void updateSpecName(QModelIndex* model_index);

public:
	void openFile();
	void modifyGeometryVariables();
	void modefyAlgorithmParameters();

signals:
	void signal_outputMessage(QString);
	void signal_calculate(bool);

public slots:
	void slot_showAll();
	void slot_hideAll();
	void slot_stopRun();
	void slot_run();
	void slot_rename_spec();
	void slot_copy_spec();
	void slot_delet_spec();	
	void slot_addPerFormanceSetting();
	void slot_delPerFormanceSetting();
	

	//实时读取标准输出
	void slot_readyRead();

	//mouse right click
	void slot_customContextMenuRequested(const QPoint &pos);
	void slot_doubleClicked(const QModelIndex& item_index);
	void slot_clicked(const QModelIndex& item_index);
	void slot_creatProTreeByXML(QString&, parsProblem*);

private:
	parsProblem* _atn_problem;
	QTreeView *_pro_tree;
	QStandardItemModel* _tree_model;
	modelInfo* _model_info;
	QMap<QString, QIcon> _icon_map;
	//different menu can respond different right click
	QMenu* _project_menu;
	QMenu* _atn_design_menu;
	QMenu* _performance_menu;
	QMenu* _item_performance_menu;

	QModelIndex* _curr_item_index;

	//optimizal process
	QProcess* optRunProcess; //better? i don't know. maybe
	//QDockWidget* outputDock;

	QAction* _act_design_run;
	QAction* _act_design_stop;	

	bool _is_running;
};
