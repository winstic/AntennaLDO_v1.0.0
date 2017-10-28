#pragma once

#include <QtWidgets>
#include "../Utility/global.h"
#include "antennaCell.h"
#include "../Antenna/modeInfo.h"

class atnLibrary : public QWidget {
	Q_OBJECT
public:
	atnLibrary(int dock_width, int table_columns, QWidget *parent = 0);
	~atnLibrary();
	QTableWidget* getTableWidget() const;
	/*QSize sizeHint() const{
	return QSize(500, 900);
	}*/
signals:
	void signal_createAndParseXML(QString, parsProblem*);

public slots:
	void slot_reSizeSlot(int row, int column);
	void slot_tableCellDoubleClick(int, int);
	void slot_customContextMenuRequested(QPoint);
	//emitted when the mouse button press
	void slot_cellPressed(int, int);
	void slot_property();
	void slot_newProject();
	//search antenna
	void slot_clickSearchButton();
	void slot_searchTextChange(QString);

private:
	void atnLibraryLayout();
	void initAtnCellList();
	void initAtnCellWidth(QVector<int>& cell_width_vec);
	void initMenu();
	void newProject();

	parsProblem* _atn_problem;
	QList<antennaCell*> _atn_cell_list;
	QTableWidget* _table_view;
	modelInfo* _model_info;
	int _atn_dock_WH;
	int _num_of_table_col;
	QMenu* _item_menu;
};

