#pragma once

#include <QtWidgets>
#include <QMenu>
#include <QDomDocument>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include "../Antenna/modeInfo.h"

class treeModel : public QTreeView {
	Q_OBJECT
public:
	explicit treeModel(QWidget* parent = 0);
	QTreeView* getTreeWidget();
	bool updateXMLFile(const QString &fileName, const QStandardItem *item, const QStandardItem *child);
	bool parseXML(const QString& file_name, parsProblem* atn_problem);
	~treeModel() {}

private:
	void parseProjectElement(const QDomElement &element, QStandardItem* parent);
	void parseNodeElement(const QDomElement &element, QStandardItem *parent);
	void parseItemElement(const QDomElement &element, QStandardItem *parent);	
	bool writeXMLFile(const QString& file_name, parsProblem* atn_problem);

	//make no sence
	QList<QStandardItem*> getRoots();           //获取所有节点
	QStandardItem* getProNode();                //获取项目工程节点
	QList<QStandardItem*> getFolderNode();      //获取文件夹节点

	void initMenu();
	void initIcon();
	//void showTree();

	inline QMenu* getProjectMenu() { return _project_menu; }
	inline QMenu* getAtnDesignMenu() { return _atn_design_menu; }
	inline QMenu* getAtnOptimizeMenu() { return _atn_optimize_menu; }
	inline QMenu* getResultMenu() { return _result_menu; }
	inline QMenu* getItemViewMenu() { return _item_view_menu; }
	inline QMenu* getItemDesignMenu() { return _item_design_menu; }
	inline QMenu* getItemOptimizeMenu() { return _item_optimize_menu; }

signals:
	void signal_outputMessage(QString text);

private slots:
	void slot_showAll();
	void slot_hideAll();
	void slot_addDesign();
	void slot_addOptimize();
	void slot_designRun();
	void slot_optimizeRun();
	void slot_interrupt();
	void slot_designStop();
	void slot_optimizeStop();
	void slot_del();
	void slot_openFile();
	void slot_modifyDesignVar();
	void slot_modifyOptimizeVar();
	void slot_showResult();
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
	modelInfo* _model_info;
	QMap<QString, QIcon> _icon_map;
	//different menu can respond different right click
	QMenu* _project_menu;
	QMenu* _atn_design_menu;
	QMenu* _atn_optimize_menu;
	QMenu* _result_menu;
	QMenu* _item_design_menu;
	QMenu* _item_optimize_menu;
	QMenu* _item_view_menu;
	QModelIndex* _curr_item_index;

	//optimizal process
	QProcess* optRunProcess; //better? i don't know. maybe
	//QDockWidget* outputDock;
};
