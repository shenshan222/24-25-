#include <QDialog>//介绍程序的弹窗类

class QLabel;
class QPushButton;

class IntroDialog : public QDialog
{
    Q_OBJECT
public:
    explicit IntroDialog(QWidget* parent = nullptr);

private:
    QLabel* label;
    QPushButton* btn;
};
