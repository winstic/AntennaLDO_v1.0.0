﻿#pragma once

#include <QtWidgets>
#include <QMenu>
#include <QDomDocument>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include "../Antenna/modeInfo.h"
#include "../Antenna/geometryModel.h"

class treeModel : public QTreeView {
	Q_OBJECT
public:
	explicit treeModel(QWidget* parent = 0);
	QTreeView* getTreeWidget();
	bool updateXMLFile(const QString &fileName, const QStandardItem *item, const QStandardItem *child);
	bool parseXML(const QString& file_name, parsProblem* atn_problem, QJsonObject* obj);
	~treeModel() {}

private:
	void parseProjectElement(const QDomElement &element, QStandardItem* parent);
	void parseNodeElement(const QDomElement &element, QStandardItem *parent);
	void parseItemElement(const QDomElement &element, QStandardItem *parent);	
	bool writeXMLFile(const QString& file_name, parsProblem* atn_problem, QJsonObject* obj);

	//make no sence
	QList<QStandardItem*> getRoots();           //获取所有节点
	QStandardItem* getProNode();                //获取项目工程节点
	QList<QStandardItem*> getFolderNode();      //获取文件夹节点

	void initMenu();
	void initIcon();

signals:
	void signal_outputMessage(QString);

public slots:
	void slot_showAll();
	void slot_hideAll();
	void slot_designRun();
	void slot_optimizeRun();
	void slot_interrupt();
	void slot_designStop();
	void slot_optimizeStop();
	void slot_del();
	void slot_designDel();
	void slot_optimizeDel();
	void slot_openFile();
	void slot_modifyDesignVar();
	void slot_modifyOptimizeVar();
	void slot_showResult();

	void slot_modifyGeometryVariables();
	//实时读取标准输出
	void slot_readyRead();

	//mouse right click
	void slot_customContextMenuRequested(const QPoint &pos);
	void slot_doubleClicked(const QModelIndex& item_index);
	void slot_clicked(const QModelIndex& item_index);
	void slot_creatProTreeByXML(QString&, parsProblem*, QJsonObject*);

private:
	parsProblem* _atn_problem;
	QJsonObject* _obj;
	QTreeView *_pro_tree;
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
};
