#pragma once

#include <QString>
#include <qvector.h>
#include <qmap.h>
#include <qpair.h>

typedef QPair<int, int> alg4pro;
enum problemType { TEST = 0, HFSS, FEKO, NEC };

struct parsProblem {
	unsigned int id;
	QString name;
	QString path;
	QString pImage;
	QString info;
	unsigned int type;
	QString oper;
};

struct parsAlgorithm {
	unsigned int id;
	QString name;
	QString path;
	QString info;
	QString oper;
};

namespace dataPool{

	class global {
	public:
		global() {
			g_DEA4AD_path = "./DEA4AD/trunk";
			/*g_default_project_path = "";
			g_working_project_path = "";
			g_current_design_path = 
			g_current_optimize_path = "";*/
		}
		~global(){}

		static parsProblem* getProblemByID(const int id);
		static parsAlgorithm* getAlgorithmByID(const int id);
		//each algorithm has unique name
		static parsAlgorithm* getAlgorithmByName(const QString name);

		static int getGWindowWidth();
		static void setGWindowWidth(int value);
		static int getGWindowHeight();
		static void setGWindowHeight(int value);
		static QString getGDEA4ADPath();
		static void setGDEA4ADPath(QString value);
		static QString getGProjectName();
		static void setGProjectName(QString value);
		static QString getGDefaultProjectPath();
		static void setGDefaultProjectPath(QString value);
		static QString getGWorkingProjectPath();
		static void setGWorkingProjectPath(QString value);
		static QString getGCurrentDesignPath();
		static void setGCurrentDesignPath(QString value);
		static QString getGCurrentOptimizePath();
		static void setGCurrentOptimizePath(QString value);
	public:
		static QVector<parsProblem> g_problems;
		static QVector<parsAlgorithm> g_algorithms;
		static QMap<alg4pro, unsigned int> g_associates;
	private:
		static int g_window_width;
		static int g_window_height;
		static QString g_DEA4AD_path;
		static QString g_project_name;
		static QString g_default_project_path;
		static QString g_working_project_path;
		static QString g_current_design_path;
		static QString g_current_optimize_path;
	};
	bool copyFile(QString source_file, QString target_file, bool cover_file_if_exist = true);
	QStringList str2list(QString str);
	QString getInfoFromRelFile(const QString &key);
}