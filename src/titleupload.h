#pragma once
#include <QDir>

class TitleUpload : public QObject
{
	Q_OBJECT
	enum {
		MaxWidth = 200,
		MaxHeight = 150
	};
public:
	TitleUpload(const QDir &path) : path(path) {}
public slots:
	void upload();
private:
	QDir path;
};
