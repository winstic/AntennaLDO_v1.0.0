#pragma once

#include <QtWidgets>
#include "../Templates/algorithmTemplate.h"

class algorithmModel : public QDialog {
	Q_OBJECT
public:
	algorithmModel(parsProblem* atn_problem, parsAlgorithm* algorithm, QJsonObject global_obj, bool is_running = false, QWidget *parent = 0);
	~algorithmModel();

protected:
	//关闭窗口时提示是否保存已经修改的数据
	//void closeEvent(QCloseEvent *event) override;

public slots:
	void slot_confirmButton(bool);
	void slot_cancelButton(bool);

private:
	void initLayout();

private:
	parsProblem* _atn_problem;
	parsAlgorithm* _algorithm;
	QJsonObject* _algorithm_obj;
	QJsonObject _global_obj;
	algorithmTemplate* _algorithm_widget;
	QPushButton* _confirm_button;
	QPushButton* _cancel_button;
	QLabel* _hint;
	QVBoxLayout* _layout;
	bool _is_running;
};
