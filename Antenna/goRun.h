#pragma once

#include <QThread>
#include <QProcess>
#include "../Utility/global.h"

class goRun : public QThread
{
public:
	goRun(QProcess* &optRunP);
	~goRun() {
		this->quit();
	}

protected:
	void run();

private:
	QProcess *_p;
};
