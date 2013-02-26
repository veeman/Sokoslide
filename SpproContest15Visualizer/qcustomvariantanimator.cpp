#include "qcustomvariantanimator.h"
#include <QDebug>

QCustomVariantAnimator::QCustomVariantAnimator(QObject *parent)
	: QVariantAnimation(parent)
{

}

QCustomVariantAnimator::~QCustomVariantAnimator()
{

}

void QCustomVariantAnimator::updateCurrentValue(const QVariant &value) {
	qreal progress = (qreal)currentTime() / (qreal)duration();
	qreal pos = easingCurve().valueForProgress(progress);
	
	QPointF start = startValue().toPointF();
	QPointF end   = endValue().toPointF();
	QPointF diff = (end - start) * pos;
	QPointF currentPos(start + diff);

    emit valueChanged(currentPos);
}
