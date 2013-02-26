#ifndef QCUSTOMVARIANTANIMATOR_H
#define QCUSTOMVARIANTANIMATOR_H

#include <QVariantAnimation>

class QCustomVariantAnimator : public QVariantAnimation
{
		Q_OBJECT

	public:
		QCustomVariantAnimator(QObject *parent = 0);
		virtual ~QCustomVariantAnimator();

	protected:
		virtual void updateCurrentValue(const QVariant &value);
};

#endif // QCUSTOMVARIANTANIMATOR_H
