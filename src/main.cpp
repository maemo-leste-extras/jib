#include <QApplication>
#include <QResource>
#include <QtCore>
#include <QSslSocket>

#if defined(Q_OS_WIN)
#include <windows.h>
#endif

#if defined(Q_OS_LINUX) && defined(STATIC)
Q_IMPORT_PLUGIN(QXcbIntegrationPlugin)
#endif

#include <unistd.h>
#include <sys/types.h>
#include "mainwindow.h"
#include "ctx.h"
#include "windowmanager.h"
#include "lib/clion_debug.h"

int main(int argc, char *argv[]) {
  Q_INIT_RESOURCE(assets);

#ifdef DEBUG
  clion_debug_setup();
#endif

  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
  QApplication::setApplicationName("jib");
  QApplication::setOrganizationDomain("https://maemo-leste.github.io/");
  QApplication::setOrganizationName("Maemo Leste");
  QApplication app(argc, argv);

  QCommandLineParser parser;
  parser.addHelpOption();
  parser.addVersionOption();
  parser.setApplicationDescription("Web");
  parser.process(app);

  qDebug() << "SSL version: " << QSslSocket::sslLibraryVersionString();
  qDebug() << "SSL build: " << QSslSocket::sslLibraryBuildVersionString();

  auto *ctx = new AppContext(&parser);
#ifdef DEBUG
  ctx->isDebug = true;
#endif
  auto windowManager = new WindowManager(ctx);
  windowManager->onSpawn();
  return QApplication::exec();
}
