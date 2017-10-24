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
	QVector<parsProblem*> g_problems;
	QVector<parsAlgorithm*> g_algorithms;
	QMap<alg4pro, unsigned int> g_associates;

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

		static QString getGDEA4ADPath() {
			return g_DEA4AD_path;
		}
		static void setGDEA4ADPath(QString value) {
			g_DEA4AD_path = value;
		}

		static QString getGProjectName() {
			return g_project_name;
		}
		static void setGProjectName(QString value) {
			g_project_name = value;
		}

		static QString getGDefaultProjectPath(){
			return g_default_project_path;
		}
		static void setGDefaultProjectPath(QString value) {
			g_default_project_path = value;
		}

		static QString getGWorkingProjectPath(){
			return g_working_project_path;
		}
		static void setGWorkingProjectPath(QString value) {
			g_working_project_path = value;
		}

		static QString getGCurrentDesignPath(){
			return g_current_design_path;
		}
		static void setGCurrentDesignPath(QString value) {
			g_current_design_path = value;
		}

		static QString getGCurrentOptimizePath(){
			return g_current_optimize_path;
		}
		static void setGCurrentOptimizePath(QString value) {
			g_current_optimize_path = value;
		}
	private:
		static QString g_DEA4AD_path;
		static QString g_project_name;
		static QString g_default_project_path;
		static QString g_working_project_path;
		static QString g_current_design_path;
		static QString g_current_optimize_path;
	};

	static parsProblem* getProblemByID(const int id) {
		QVector<parsProblem*>::iterator it;
		for (it = g_problems.begin(); it != g_problems.end(); ++it)
			if ((*it)->id == id) return (*it);
		return nullptr;
	}

	static parsAlgorithm* getAlgorithmByID(const int id) {
		QVector<parsAlgorithm*>::iterator it;
		for (it = g_algorithms.begin(); it != g_algorithms.end(); ++it)
			if ((*it)->id == id) return (*it);
		return nullptr;
	}

	//each algorithm has unique name
	static parsAlgorithm* getAlgorithmByName(const QString name) {
		QVector<parsAlgorithm*>::iterator it;
		for (it = g_algorithms.begin(); it != g_algorithms.end(); ++it)
			if ((*it)->name == name) return (*it);
		return nullptr;
	}

	static const char* str2char(QString& str) {
		std::string std_str = str.toStdString();
		return std_str.c_str();
	}

	static bool copyFile(QString source_file, QString target_file, bool cover_file_if_exist = true) {
		QString source_file_path = QDir(source_file).absolutePath();
		QString target_file_path = QDir(target_file).absolutePath();
		if (source_file_path == target_file_path) {
			return true;
		}
		if (!QFile::exists(source_file_path)) {
			qCritical(str2char(QString("source file %1 do not exist.").arg(source_file_path)));
			return false;
		}
		QDir* dir = new QDir();
		bool isExist = dir->exists(target_file_path);
		if (isExist && cover_file_if_exist) {
			dir->remove(target_file_path);
		}
		delete dir;
		dir = nullptr;

		//copy function: if target file already exist then return false 
		if (!QFile::copy(source_file_path, target_file_path)) {
			qCritical(str2char(QString("target file %1 already exist").arg(target_file_path)));
			return false;
		}
		return true;
	}

	static QStringList str2list(QString str) {
		QRegExp re("[\\[,'\\]]");
		QStringList strList = str.split(re, QString::SkipEmptyParts);
		return strList;
	}
}