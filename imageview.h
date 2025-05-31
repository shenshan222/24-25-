#include <QGraphicsView>//展示地图的类，实现缩放拖动等功能
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QVector>
#include "graph.h"
#include "clicknode.h"

class ImageView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ImageView(QWidget* parent = nullptr);
    void highlightPath(const QVector<QString>& path, const Graph& graph);
    // 多段路径绘制接口
    void highlightMultiSegmentPaths(const QVector<QVector<QString>>& segments, const Graph& graph, const QVector<QString>& waypointIDs);
    void highlightAndClickNode(const QString& nodeId); // 高亮并点击节点

protected:
    void wheelEvent(QWheelEvent* event) override;//滚轮缩放
    void mousePressEvent(QMouseEvent* event) override;//查看点的坐标
    void mouseReleaseEvent(QMouseEvent* event) override;
    void drawNodeMarkers(const QVector<QString>& path, const Graph& graph, const QVector<QString>& waypointsNames);
    QVector<QString> flattenPath(const QVector<QVector<QString>>& segments);

private:
    QGraphicsScene* scene;//场景对象
    QGraphicsPixmapItem* pixmapItem;//图片对象
    qreal scaleFactor;//qreal是对double或float的封装
    QList<QGraphicsLineItem*> pathLines;//用于记录当前绘制的路径线，便于清除
    QTimer* pathAnimationTimer = nullptr;  // 声明指针，初始化为 nullptr

    void loadNodesFromFile(const QString& filename);//加载可点击节点信息
    QMap<QString, ClickNode*> nodeMap;  // <-- 映射：ID -> ClickNode*

    QMap<QString, QGraphicsItem*> nodeMarkers;
    QColor startColor = Qt::green;                   // 起点颜色
    QColor endColor = Qt::red;                       // 终点颜色
    QColor waypointColor = Qt::blue;                 // 途径点颜色
};
