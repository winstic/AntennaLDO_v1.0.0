#pragma once
#include <QString>
#include "../Utility/macrodefined.h"

class problem {	
public:
	problem(QString pName, QString pPath);
	virtual ~problem() {};
	virtual void qrun();

	QString problemName;
	QString problemPath;	
};
