#include <QDialog>//介绍节点具体信息的弹窗类
#include <QString>

class InfoDialog : public QDialog
{
    Q_OBJECT
public:
    explicit InfoDialog(const QString& infoText, const QString& imagePath, QWidget* parent = nullptr);
};
