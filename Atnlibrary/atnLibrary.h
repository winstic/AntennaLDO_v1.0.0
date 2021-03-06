﻿#pragma once

#include <QtWidgets>
#include "../Utility/global.h"
#include "antennaCell.h"
#include "../Antenna/modeInfo.h"

class atnLibrary : public QWidget {
	Q_OBJECT
public:
	atnLibrary(QWidget *parent = 0);
	~atnLibrary();
	QTableWidget* getTableWidget() const;
	void atnLibraryLayout();
	/*QSize sizeHint() const{
	return QSize(500, 900);
	}*/
signals:
	void signal_createAndParseXML(QString&, parsProblem*);

public slots:
	void slot_reSizeSlot(int row, int column);
	void slot_tableCellDoubleClick(int, int);
	void slot_customContextMenuRequested(QPoint);
	//emitted when the mouse button press
	void slot_cellPressed(int, int);
	void slot_property();
	void slot_newProject();
	//search antenna
	void slot_searchTextChange(QString);
	//正在运行时，不能新建工程
	void slot_checkNewProject(bool);

private:
	void initAtnCellList();
	void initMenu();
	void newProject();

	parsProblem* _atn_problem;
	QList<antennaCell*> _atn_cell_list;
	QTableWidget* _table_view;
	modelInfo* _model_info;
	QMenu* _item_menu;
	QAction* _act_new;
};

