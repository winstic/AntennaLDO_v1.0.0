#pragma once

#include <QThread>
#include "../Utility/global.h"

class aRun : public QThread
{
public:
	aRun() {};
	~aRun() {}

protected:
	virtual void run() {};
};
