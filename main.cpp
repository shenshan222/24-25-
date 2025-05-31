#include <QApplication>
#include "MainWindow.h"
#include "introdialog.h"
#include <QScreen>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    IntroDialog introDlg;

    if (introDlg.exec() == QDialog::Accepted) {
        MainWindow window;
        window.resize(1346, 800);
        window.show();

        QScreen* screen = QGuiApplication::primaryScreen();//实现主窗口居中展示
        QRect screenGeometry = screen->availableGeometry();
        int x = (screenGeometry.width() - window.width()) / 2;
        int y = (screenGeometry.height() - window.height()) / 2;
        window.move(x, y);
        return app.exec();
    }

    return app.exec();
}
