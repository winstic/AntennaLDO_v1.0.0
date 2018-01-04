#pragma once
#include <QtWidgets>
class comboboxDelegate : public QItemDelegate {
	Q_OBJECT
public:
	comboboxDelegate(int combobox_column, QObject* parent = 0);
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	//void setEditorData(QWidget *editor, const QModelIndex &index) const;
	//void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

//private slots:
	//void slot_commitAndCloseEditor();

private:
	int _combobox_column;
	int _low;
	int _up;
	int _step;
};