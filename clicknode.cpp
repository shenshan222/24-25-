#include "infodialog.h"
#include "imageview.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QGraphicsSceneMouseEvent>

ClickNode::ClickNode(const QString& image, const QPointF& position, const QString& info, const QPixmap& pixmap, QGraphicsItem* parent)
    : QGraphicsPixmapItem(pixmap, parent), infoText(info), imagePath(image)
{
    setPos(position - QPointF(pixmap.width() / 2, pixmap.height() / 2)); // 居中放置
    setToolTip("点击获取更多信息");
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);
}

void ClickNode::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (contains(event->pos())) {
        pressedInside = true;
    } else {
        pressedInside = false;
    }

    QGraphicsPixmapItem::mousePressEvent(event);
}

void ClickNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (pressedInside && contains(event->pos())) {
        // 重置拖动状态和光标
        QGraphicsView* view = scene()->views().isEmpty() ? nullptr : scene()->views().first();
        if (view) {
            view->setDragMode(QGraphicsView::NoDrag);
            view->unsetCursor();
        }

        //弹窗
        InfoDialog* dialog = new InfoDialog(infoText, imagePath);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->show();
    }

    pressedInside = false;
    QGraphicsPixmapItem::mouseReleaseEvent(event);
}
