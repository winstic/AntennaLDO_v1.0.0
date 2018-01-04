#include "gainAxialDelegateTemplate.h"

gainAxialDelegate::gainAxialDelegate(QList<int> &combobox_columns, QList<int> &line_edit_columns, QList<double> &lows, QList<double> &ups, QList<double> &steps, QObject* parent) : QItemDelegate(parent)
 ,_combobox_columns(combobox_columns), _line_edit_columns(line_edit_columns), _lows(lows), _ups(ups), _steps(steps){
	_itemplate = new iTemplate;
} 

void gainAxialDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
	QItemDelegate::paint(painter, option, index);
}


QWidget* gainAxialDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
	int col = index.column();
	if (_combobox_columns.contains(col)) {
		QComboBox *comb = new QComboBox(parent);
		comb->setEditable(true);
		//comb->installEventFilter(const_cast<gainAxialDelegate *>(this));
		connect(comb, SIGNAL(currentIndexChanged()), this, SLOT(slot_comboBoxCommitAndCloseEditor()));
		return comb;
	}
	else if(_line_edit_columns.contains(col)) {
		QLineEdit *edit = new QLineEdit(parent);
		QRegExpValidator* floatValidReg = _itemplate->getFloatReg();    //float
		edit->setValidator(floatValidReg);
		connect(edit, SIGNAL(textChanged()), this, SLOT(slot_lineEditCommitAndCloseEditor()));
		return edit;
	}
	else {
		return QItemDelegate::createEditor(parent, option, index);
	}
}

void gainAxialDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
	int col = index.column();
	if (_combobox_columns.contains(col)) {
		QString angle = index.model()->data(index, Qt::DisplayRole).toString();
		QComboBox *comb = qobject_cast<QComboBox *>(editor);
		_itemplate->initAngleComboBox(comb, _lows[col], _ups[col], _steps[col]);
		comb->setCurrentText(angle);
	}
	else if (_line_edit_columns.contains(col)) {
		QString text = index.model()->data(index, Qt::DisplayRole).toString();
		QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
		edit->setText(text);
	}
	else {
		QItemDelegate::setEditorData(editor, index);
	}
}

void gainAxialDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
	int col = index.column();
	if (_combobox_columns.contains(col)){
		QComboBox *comb = qobject_cast<QComboBox *>(editor);
		model->setData(index, comb->currentText());
	}
	else if (_line_edit_columns.contains(col)) {
		QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
		model->setData(index, edit->text());
	}
	else {
		QItemDelegate::setModelData(editor, model, index);
	}
}

void gainAxialDelegate::slot_comboBoxCommitAndCloseEditor() {
	QComboBox *comb = qobject_cast<QComboBox *>(sender());
	emit commitData(comb);
	emit closeEditor(comb);
}
void gainAxialDelegate::slot_lineEditCommitAndCloseEditor() {
	QLineEdit *edit = qobject_cast<QLineEdit *>(sender());
	emit commitData(edit);
	emit closeEditor(edit);
}