#ifndef GRAPH_H//整张地图基本的图结构
#define GRAPH_H

#include <QString>
#include <QMap>
#include <QSet>
#include <QVector>
#include <QHash>
#include <QDebug>

struct Position {//节点位置，横纵坐标
    double x;
    double y;
};

class Node {//基本节点
public:
    QString id;
    QString name;
    Position pos;
    QMap<QString, double> neighbors;//连接的节点和对应的代价

    Node();
    Node(const QString& id_, const QString& name_, const Position& pos_);
};

class Graph {//图结构
public:
    QMap<QString, Node> nodes;
    QHash<QString, QString> nameToId;//名字和序号的转换
    QHash<QString, QString> idToName;

    QString getIdByName(const QString& name) const;
    QString getNameById(const QString& id) const;

    void addNode(const QString& id, const QString& name, double x, double y);//添加节点信息
    void addEdge(const QString& from, const QString& to);//添加连接信息

    void loadNodesFromFile(const QString& filename);//载入节点信息
    void loadEdgesFromFile(const QString& filename);//载入连接信息

    QVector<QString> aStarSearch(const QString& start, const QString& goal) const;//A*算法搜索最优路径

    QVector<QString> findPathThroughWaypoints(const QString& start, const QVector<QString>& waypoints, const QString& end) const;

    void printGraph() const;
};

#endif // GRAPH_H
