#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QTimer>

#include "titleupload.h"

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	QCoreApplication::setApplicationName(QStringLiteral("Clonkspot Title Uploader"));
	QCoreApplication::setApplicationVersion(QStringLiteral("1.0"));

	QCommandLineParser parser;
	parser.setApplicationDescription(QStringLiteral("Uploads scenario title images for Clonkspot's live game view"));
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addPositionalArgument(QStringLiteral("path"), QStringLiteral("Directory containing the scenarios. (optional)"), QStringLiteral("[path]"));

	parser.process(app);
	const QStringList &arguments = parser.positionalArguments();

	if (arguments.length() < 1)
	{
		parser.showHelp(1);
	}

	QDir dir(arguments[0]);
	if (!dir.exists() || !dir.isReadable())
	{
		qWarning() << "Can't open" << argv[1];
		return 1;
	}

	TitleUpload uploader(dir);
	QTimer::singleShot(0, &uploader, &TitleUpload::upload);
	uploader.upload();
	return app.exec();
}
