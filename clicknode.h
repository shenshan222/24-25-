//可点击显示信息的类
#include <QGraphicsEllipseItem>
#include <QObject>
#include <QString>
#include <QPointF>
#include <QGraphicsPixmapItem>

class ClickNode : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    ClickNode(const QString& image, const QPointF& position, const QString& info, const QPixmap& pixmap, QGraphicsItem* parent = nullptr);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

public:
    QString infoText;
    QString imagePath;
    bool pressedInside = false;
};
