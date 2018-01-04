#pragma once
#include <QtWidgets>
#include "iTemplate.h"
class gainAxialDelegate : public QItemDelegate{
	Q_OBJECT
public:
	gainAxialDelegate(QList<int> &combobox_columns, QList<int> &line_edit_columns, QList<double> &lows, QList<double> &ups, QList<double> &steps, QObject* parent = 0);
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

private slots:
	void slot_comboBoxCommitAndCloseEditor();
	void slot_lineEditCommitAndCloseEditor();

private:
	iTemplate* _itemplate;
	QList<int> _combobox_columns;		//设置QComboBox委托的所有列
	QList<int> _line_edit_columns;		//设置line edit委托的所有列
	QList<double> _lows;
	QList<double> _ups;
	QList<double> _steps;
};