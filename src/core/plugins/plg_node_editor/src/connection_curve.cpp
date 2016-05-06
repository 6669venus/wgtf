#include "connection_curve.hpp"
#include <qpainter.h>

ConnectionCurve::ConnectionCurve(QQuickItem *parent) : QQuickPaintedItem(parent)
{
    setAntialiasing(true);
    setFlag(ItemHasContents, true);
    setAcceptedMouseButtons(Qt::RightButton);
}

void ConnectionCurve::setFromPoint(const QPointF &p)
{
    m_fromPoint = p;
    emit fromPointChanged(m_fromPoint);
    update();
}

void ConnectionCurve::setToPoint(const QPointF &p)
{
    m_toPoint = p;
    emit toPointChanged(m_toPoint);
    update();
}

void ConnectionCurve::setFromNode(const QRectF &nodeRect)
{
    m_fromNode = nodeRect;
}

void ConnectionCurve::setToNode(const QRectF &nodeRect)
{
    m_toNode = nodeRect;
}

void ConnectionCurve::paint(QPainter *painter)
{
    #define CONNECTION_ROUNDING 10

    if (m_fromPoint.isNull() || m_toPoint.isNull())
        return;

    QPainterPath path;
    path.moveTo(m_fromPoint);

    bool useDefault = true;

    // For reverse connection (where OUT if to the right of IN), use more complex algorithm
    if (m_fromPoint.x() >= m_toPoint.x() - 32 && !m_fromNode.isNull() && !m_toNode.isNull())
    {
        // Determine a line that divides the blocks vertically
        float dividerY = 0.25 * (m_fromNode.top() + m_fromNode.bottom() + m_toNode.top() + m_toNode.bottom());

        // Determine the vertical segments of the connector
        float outPartX = m_fromPoint.x() + 28;
        float inPartX =  m_toPoint.x() - 28;

        float arcWidth = 2 * CONNECTION_ROUNDING;

        if (m_toNode.bottom() < m_fromNode.top())
        {
            dividerY = 0.5 * (m_toNode.bottom() + m_fromNode.top());

            path.lineTo(outPartX - CONNECTION_ROUNDING, m_fromPoint.y());
            path.arcTo(outPartX - arcWidth, m_fromPoint.y() - arcWidth, arcWidth, arcWidth, 270, 90);
            path.lineTo(outPartX, dividerY + CONNECTION_ROUNDING);
            path.arcTo(outPartX - arcWidth, dividerY, arcWidth, arcWidth, 0, 90);

            path.lineTo(inPartX + CONNECTION_ROUNDING, dividerY);
            path.arcTo(inPartX, dividerY - arcWidth, arcWidth, arcWidth, 270, -90);
            path.lineTo(inPartX, m_toPoint.y() + CONNECTION_ROUNDING);
            path.arcTo(inPartX, m_toPoint.y(), arcWidth, arcWidth, 180, -90);

            path.lineTo(m_toPoint);
            useDefault = false;
        }
        else if (m_fromNode.bottom() < m_toNode.top())
        {
            dividerY = 0.5 * (m_fromNode.bottom() + m_toNode.top());

            path.lineTo(outPartX - CONNECTION_ROUNDING, m_fromPoint.y());
            path.arcTo(outPartX - arcWidth, m_fromPoint.y(), arcWidth, arcWidth, 90, -90);
            path.lineTo(outPartX, dividerY - CONNECTION_ROUNDING);
            path.arcTo(outPartX - arcWidth, dividerY - arcWidth, arcWidth, arcWidth, 0, -90);

            path.lineTo(inPartX + CONNECTION_ROUNDING, dividerY);
            path.arcTo(inPartX, dividerY, arcWidth, arcWidth, 90, 90);
            path.lineTo(inPartX, m_toPoint.y() - CONNECTION_ROUNDING);
            path.arcTo(inPartX, m_toPoint.y() - arcWidth, arcWidth, arcWidth, 180, 90);

            path.lineTo(m_toPoint);
            useDefault = false;
        }
    }

    if (useDefault)
    {
        qreal dx = m_toPoint.x() - m_fromPoint.x();
        qreal dy = m_toPoint.y() - m_fromPoint.y();

        QPointF ctr1(m_fromPoint.x() + dx * 0.5, m_fromPoint.y() + dy * 0.05);
        QPointF ctr2(m_fromPoint.x() + dx * 0.5, m_fromPoint.y() + dy * 0.95);

        path.cubicTo(ctr1, ctr2, m_toPoint);
    }

    QColor wireColor(79, 106, 25);
    QColor maskColor = Qt::black;

    painter->setPen(QPen(maskColor, 4, Qt::SolidLine,
        Qt::RoundCap, Qt::RoundJoin));
    painter->drawPath(path);
    

    painter->setPen(QPen(wireColor, 2, Qt::SolidLine,
        Qt::RoundCap, Qt::RoundJoin));
    painter->drawPath(path);

    m_path = path;
}

void ConnectionCurve::mousePressEvent(QMouseEvent *event)
{
    QPainterPath path;
    path.moveTo(event->pos());
    path.addEllipse(event->pos(), 4, 4);
    bool result = m_path.intersects(path);
    event->setAccepted(result);

    if (result)
    {
        emit connectionClicked();
    }        
}