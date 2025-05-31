#include "graph.h"
#include <QFile>
#include <QTextStream>
#include <QQueue>
#include <QPair>
#include <QSet>
#include <QtMath>
#include <QDebug>
#include <QRegularExpression>
#include <queue>

Node::Node() : id(""), name(""), pos({0, 0}) {}

Node::Node(const QString& id_, const QString& name_, const Position& pos_)
    : id(id_), name(name_), pos(pos_) {}

static double heuristic(const Position& a, const Position& b) {
    return qAbs(a.x - b.x) + qAbs(a.y - b.y); // 曼哈顿距离
}

QString Graph::getIdByName(const QString& name) const {
    return nameToId.value(name, "");
}

QString Graph::getNameById(const QString& id) const {
    return idToName.value(id, "");
}

void Graph::addNode(const QString& id, const QString& name, double x, double y) {
    Position pos = { x, y };
    nodes[id] = Node(id, name, pos);
    nameToId[name] = id;
    idToName[id] = name;
}

void Graph::addEdge(const QString& from, const QString& to) {
    if (nodes.contains(from) && nodes.contains(to)) {
        double cost = heuristic(nodes[from].pos, nodes[to].pos);
        nodes[from].neighbors[to] = cost;
        nodes[to].neighbors[from] = cost;
    } else {
        qWarning() << "Error: No such node" << from << "or" << to;
    }
}

void Graph::loadNodesFromFile(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open node file:" << filename;
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList parts = line.split(QRegularExpression("\\s+"));

        if (parts.size() != 4) {
            qWarning() << "Invalid node line:" << line;
            continue;
        }

        QString id = parts[0];
        QString name = parts[1];
        double x = parts[2].toDouble();
        double y = parts[3].toDouble();

        addNode(id, name, x, y);
    }
    file.close();
}

void Graph::loadEdgesFromFile(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open edge file:" << filename;
        return;
    }

    QTextStream in(&file);
    QSet<QPair<QString, QString>> addedEdges;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList parts = line.split(QRegularExpression("\\s+"));
        if (parts.size() < 2) continue;

        QString from = parts[0];
        for (int i = 1; i < parts.size(); ++i) {
            QString to = parts[i];
            auto edge = qMakePair(qMin(from, to), qMax(from, to));
            if (!addedEdges.contains(edge)) {
                addEdge(from, to);
                addedEdges.insert(edge);
            }
        }
    }
    file.close();
}

QVector<QString> Graph::aStarSearch(const QString& start, const QString& goal) const {
    if (!nodes.contains(start) || !nodes.contains(goal)) {
        qWarning() << "Start or goal node not found.";
        return {};
    }

    struct NodeRecord {
        QString id;
        double f;
        double g;
        bool operator>(const NodeRecord& other) const {
            return f > other.f;
        }
    };

    std::priority_queue<NodeRecord, QVector<NodeRecord>, std::greater<NodeRecord>> openSet;
    QSet<QString> closedSet;
    QHash<QString, QString> cameFrom;
    QHash<QString, double> gScore;

    gScore[start] = 0.0;
    double h = heuristic(nodes[start].pos, nodes[goal].pos);
    openSet.push({ start, h, 0.0 });

    while (!openSet.empty()) {
        NodeRecord current = openSet.top();
        openSet.pop();

        if (current.id == goal) {
            QVector<QString> path;
            QString node = goal;
            while (node != start) {
                path.append(node);
                node = cameFrom[node];
            }
            path.append(start);
            std::reverse(path.begin(), path.end());
            return path;
        }

        if (closedSet.contains(current.id)) continue;
        closedSet.insert(current.id);

        const Node& currentNode = nodes[current.id];
        for (auto it = currentNode.neighbors.begin(); it != currentNode.neighbors.end(); ++it) {
            QString neighborId = it.key();
            double tentative_g = gScore[current.id] + it.value();

            if (!gScore.contains(neighborId) || tentative_g < gScore[neighborId]) {
                gScore[neighborId] = tentative_g;
                double f = tentative_g + heuristic(nodes[neighborId].pos, nodes[goal].pos);
                openSet.push({ neighborId, f, tentative_g });
                cameFrom[neighborId] = current.id;
            }
        }
    }

    qWarning() << "No path found from" << start << "to" << goal;
    return {};
}

QVector<QString> Graph::findPathThroughWaypoints(const QString& start, const QVector<QString>& waypoints, const QString& end) const {
    QVector<QString> fullPath;
    QString current = start;

    QVector<QString> points = waypoints;
    points.append(end);

    for (const QString& next : points) {
        QVector<QString> partialPath = aStarSearch(current, next);
        if (partialPath.isEmpty()) return {}; // 路径断了

        if (!fullPath.isEmpty()) {
            partialPath.removeFirst(); // 去除重复节点
        }
        fullPath.append(partialPath);
        current = next;
    }

    return fullPath;
}


void Graph::printGraph() const {
    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        const Node& node = it.value();
        qDebug() << it.key() << "(" << node.name << ") at (" << node.pos.x << "," << node.pos.y << ")";
        if (node.neighbors.isEmpty()) {
            qDebug() << "  No connections.";
        } else {
            qDebug() << "  Connected to:";
            for (auto nIt = node.neighbors.begin(); nIt != node.neighbors.end(); ++nIt) {
                qDebug() << "    →" << nIt.key() << "[distance =" << nIt.value() << "]";
            }
        }
        qDebug() << "---------------------------";
    }
}
