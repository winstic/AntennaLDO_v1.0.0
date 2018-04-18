#pragma once

#include "../Utility/global.h"
#include "iTemplate.h"
#include "tableTemplate.h"

class lossTemplate : public iTemplate {
	Q_OBJECT
public:
	lossTemplate(parsProblem* atn_problem, QJsonObject* obj, unsigned int index = 0, iTemplate *parent = 0);
	~lossTemplate();

public:
	QLayout* getLayout();
	//update _obj
	void updateJObj();
	bool checkInputValid();

signals:
	void signal_checkValid();

public slots:
	void slot_ChangeOptimaltype(QString);

private:
	void initDefaultData();
	void initLayout();
	void initLossTypeComBox(QComboBox *combox);


public:
	parsProblem* _atn_problem;
	QJsonObject* _obj;
	QLayout* _layout;
	tableTemplate* _loss_table;
	unsigned int _index;

	QStringList _R0_real_lists;
	QStringList _R0_image_lists;
	QStringList _return_loss_type_lists;
	QStringList _optimal_type_lists;
	QStringList _delta_real_lists;
	QStringList _delta_image_lists;
	QStringList _vswr_obj_lists;
	QStringList _s11_obj_lists;
	QStringList _R1_real_lists;
	QStringList _R1_image_lists;
	QStringList _weight_lists;
};