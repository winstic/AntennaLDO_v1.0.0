#include "antennaCell.h"
#include "../Utility/macrodefined.h"

antennaCell::antennaCell(parsProblem* atn_problem, QWidget* parent):QWidget(parent), 
_atn_problem(atn_problem){
    QHBoxLayout *hLayout = new QHBoxLayout(this);
    QVBoxLayout *vLayout = new QVBoxLayout(this);
	_atn_name_label = new QLabel(QString("%1 Antenna").arg(_atn_problem->name), this);
	_atn_photo = new QLabel(this);
    QPixmap mPixmap = QPixmap(_atn_problem->name);
	_atn_photo->setPixmap(mPixmap.scaled(CELLATNWH, CELLATNHT, Qt::KeepAspectRatio));

	//hLayout->addWidget((QWidget *)m_icon);
    vLayout->addWidget(_atn_photo);
    vLayout->addWidget(_atn_name_label);
	vLayout->setStretch(0,2);
	vLayout->setStretch(1,1);
    hLayout->addSpacerItem(&QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
	hLayout->addLayout(vLayout);
    hLayout->addSpacerItem(&QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
    setLayout(hLayout);

	setMouseTracking(true);
}

parsProblem* antennaCell::getAntennaProblem() const {
	if (_atn_problem == nullptr)
		qCritical("uninitialize problem data struct");
	return _atn_problem;
}

//void antennaCell::enterEvent(QEvent *e)
//{
//	
//}

/*void antennaCell::mouseMoveEvent(QMouseEvent* event)
{

	int index = m_sqliteManage->m_antennaName_list.indexOf(m_name->text());
	QString info = m_sqliteManage->m_antennaInfo_list[index];
	if (info.size() >= 30)
	{
		info=info.mid(0, 30);
		info += "...";
	}
	QToolTip::showText(cursor().pos(), info, 0);
	//setToolTip(info);
}*/
