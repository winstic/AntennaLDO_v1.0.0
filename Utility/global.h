#pragma once

#include <QString>
#include <qvector.h>
#include <qmap.h>
#include <qpair.h>

typedef QPair<QString, QString> alg4pro;
typedef QPair<double, double> freRange;

struct parsProblem {
	QString name;
	QString path;
	QString pImage;
	QString info;
	QString type;
	QString oper;
	QVector<freRange> frequencyRanges;
	double max_frequency;
	parsProblem() : name(""), path(""), pImage("./images/antenna.png"), info(""), type(""), oper("i"), max_frequency(0.001){}
	void fillMaxFrequency() {
		max_frequency = 0.01;
		QVector<freRange>::iterator iter = frequencyRanges.begin();
		for (; iter != frequencyRanges.end(); ++iter) {
			if (iter->second > max_frequency)
				max_frequency = iter->second;
		}
	}
};

struct parsAlgorithm {
	QString name;
	QString path;
	QString info;
	QString type;
	QString oper;
	parsAlgorithm() : name(""), path(""), info(""), type(""), oper("i") {}
};

namespace dataPool{

	class global {
	public:
		global() {}
		~global(){}

		static parsProblem* getProblemByName(const QString name);
		//each algorithm has unique name
		static parsAlgorithm* getAlgorithmByName(const QString name);

		static int getGWindowWidth();
		static void setGWindowWidth(int value);
		static int getGWindowHeight();
		static void setGWindowHeight(int value);
		static QString getGDEA4ADPath();
		static void setGDEA4ADPath(QString value);
		static QString getGConfigFile();
		static void setGConfigFile(QString value);
		static QString getGProjectName();
		static void setGProjectName(QString value);
		static QString getGDefaultProjectPath();
		static void setGDefaultProjectPath(QString value);
		static QString getGWorkingProjectPath();
		static void setGWorkingProjectPath(QString value);
		static QString getGCurrentSpecName();
		static void setGCurrentSpecName(QString value);
	public:
		static QVector<parsProblem> g_problems;
		static QVector<parsAlgorithm> g_algorithms;
		static QMap<alg4pro, QString> g_associates;
		static QMap<QString, QString> g_flags;
	private:
		static int g_window_width;
		static int g_window_height;
		static QString g_DEA4AD_path;
		static QString g_CONFIG_FILE;
		static QString g_project_name;
		static QString g_default_project_path;
		static QString g_working_project_path;
		static QString g_currend_spec_name;
	};
	bool copyFile(QString source_file, QString target_file, bool cover_file_if_exist = true);
	bool deleteDir(QString dir_path);
	QStringList str2list(QString str);
	QStringList stack2list(QString str);
	QString getInfoFromRelFile(const QString &path, const QString &key);
}