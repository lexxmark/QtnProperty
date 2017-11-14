/*******************************************************************************
Copyright 2015-2017 Alexandra Cherdantseva <neluhus.vagus@gmail.com>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*******************************************************************************/

#include "PropertyQRectF.h"

#include "Delegates/PropertyDelegateFactory.h"
#include "Core/PropertyQRect.h"
#include "QObjectPropertySet.h"

#include <QLocale>
#include <QLineEdit>

static QByteArray qtnQRectF_LTRB()
{
	returnQByteArrayLiteral("QRectF_LTRB");
}
static QByteArray qtnQRectF_LTWH()
{
	returnQByteArrayLiteral("QRectF_LTWH");
}

QtnPropertyQRectFBase::QtnPropertyQRectFBase(QObject *parent)
	: ParentClass(parent)
	, coordinates(false)
{
	addState(QtnPropertyStateCollapsed);
}

void QtnPropertyQRectFBase::setMode(bool coordinates)
{
	this->coordinates = coordinates;

	setDelegateCallback([coordinates]() -> const QtnPropertyDelegateInfo * {
		auto result = new QtnPropertyDelegateInfo;
		result->name = coordinates ? qtnQRectF_LTRB() : qtnQRectF_LTWH();
		return result;
	});
}

QtnProperty *QtnPropertyQRectFBase::createLeftProperty(bool move)
{
	return createFieldProperty(QtnPropertyQRectF::tr("Left"),
		QtnPropertyQRectF::tr("Left position of the %1"), &QRectF::left,
		move ? &QRectF::moveLeft : &QRectF::setLeft);
}

QtnProperty *QtnPropertyQRectFBase::createTopProperty(bool move)
{
	return createFieldProperty(QtnPropertyQRectF::tr("Top"),
		QtnPropertyQRectF::tr("Top position of the %1"), &QRectF::top,
		move ? &QRectF::moveTop : &QRectF::setTop);
}

QtnProperty *QtnPropertyQRectFBase::createRightProperty(bool move)
{
	return createFieldProperty(QtnPropertyQRectF::tr("Right"),
		QtnPropertyQRectF::tr("Right position of the %1"), &QRectF::right,
		move ? &QRectF::moveRight : &QRectF::setRight);
}

QtnProperty *QtnPropertyQRectFBase::createBottomProperty(bool move)
{
	return createFieldProperty(QtnPropertyQRectF::tr("Bottom"),
		QtnPropertyQRectF::tr("Bottom position of the %1"), &QRectF::bottom,
		move ? &QRectF::moveBottom : &QRectF::setBottom);
}

QtnProperty *QtnPropertyQRectFBase::createWidthProperty()
{
	return createFieldProperty(QtnPropertyQRectF::tr("Width"),
		QtnPropertyQRectF::tr("Width of the %1"), &QRectF::width,
		&QRectF::setWidth);
}

QtnProperty *QtnPropertyQRectFBase::createHeightProperty()
{
	return createFieldProperty(QtnPropertyQRectF::tr("Height"),
		QtnPropertyQRectF::tr("Height of the %1"), &QRectF::height,
		&QRectF::setHeight);
}

bool QtnPropertyQRectFBase::fromStrImpl(const QString &str, bool edit)
{
	QRegExp *rect_parser;
	if (coordinates)
	{
		static QRegExp coordRectParser(QStringLiteral("\
\\s*\\[\\s*\\(\\s*(-?((\\d+\\.\\d*)|(\\d*\\.\\d+)|(\\d+)))\\s*,\
\\s*(-?((\\d+\\.\\d*)|(\\d*\\.\\d+)|(\\d+)))\\)\\s*,\
\\s*\\(\\s*(-?((\\d+\\.\\d*)|(\\d*\\.\\d+)|(\\d+)))\\s*,\
\\s*(-?((\\d+\\.\\d*)|(\\d*\\.\\d+)|(\\d+)))\\)\\s*\\]\\s*"));
		rect_parser = &coordRectParser;
	} else
	{
		static QRegExp simpleRectParser(QStringLiteral("\
\\s*\\[\\s*\\(\\s*(-?((\\d+\\.\\d*)|(\\d*\\.\\d+)|(\\d+)))\\s*,\
\\s*(-?((\\d+\\.\\d*)|(\\d*\\.\\d+)|(\\d+)))\\)\\s*,\
\\s*((\\d+\\.\\d*)|(\\d*\\.\\d+)|(\\d+))\\s*x\
\\s*((\\d+\\.\\d*)|(\\d*\\.\\d+)|(\\d+))\\s*\\]\\s*"));
		rect_parser = &simpleRectParser;
	}

	if (!rect_parser->exactMatch(str))
		return false;

	bool ok;
	auto x = qreal(rect_parser->cap(1).toDouble(&ok));

	if (!ok)
		return false;

	auto y = qreal(rect_parser->cap(6).toDouble(&ok));

	if (!ok)
		return false;

	if (coordinates)
	{
		auto right = qreal(rect_parser->cap(11).toDouble(&ok));

		if (!ok)
			return false;

		auto bottom = qreal(rect_parser->cap(15).toDouble(&ok));

		if (!ok)
			return false;

		return setValue(QRectF(x, y, right - x, bottom - y));
	}

	auto width = qreal(rect_parser->cap(11).toDouble(&ok));

	if (!ok)
		return false;

	auto height = qreal(rect_parser->cap(15).toDouble(&ok));

	if (!ok)
		return false;

	return setValue(QRectF(x, y, width, height), edit);
}

bool QtnPropertyQRectFBase::toStrImpl(QString &str) const
{
	auto v = value();

	if (coordinates)
	{
		str = QStringLiteral("[(%1, %2), (%3, %4)]")
				  .arg(v.left())
				  .arg(v.top())
				  .arg(v.right())
				  .arg(v.bottom());
	} else
	{
		str = QStringLiteral("[(%1, %2), %3 x %4]")
				  .arg(v.left())
				  .arg(v.top())
				  .arg(v.width())
				  .arg(v.height());
	}

	return true;
}

QtnPropertyQRectFCallback::QtnPropertyQRectFCallback(
	QObject *parent, bool coordinates)
	: QtnSinglePropertyCallback<QtnPropertyQRectFBase>(parent)
{
	setMode(coordinates);
}

QtnPropertyQRectFCallback *QtnPropertyQRectFCallback::createFrom(
	QObject *object, const QMetaProperty &metaProperty)
{
	auto property =
		new QtnPropertyQRectFCallback(nullptr, metaProperty.revision() == 1);

	property->setCallbackValueGet([object, metaProperty]() -> QRectF {
		return metaProperty.read(object).toRectF();
	});

	property->setCallbackValueSet([object, metaProperty](QRectF value) {
		metaProperty.write(object, value);
	});

	property->setCallbackValueAccepted(
		[property](QRectF) -> bool { return property->isEditableByUser(); });

	return property;
}

QtnProperty *QtnPropertyQRectF::internalCreateFrom(
	QObject *object, const QMetaProperty &metaProperty)
{
	return QtnPropertyQRectFCallback::createFrom(object, metaProperty);
}

QtnPropertyQRectF::QtnPropertyQRectF(QObject *parent, bool coordinates)
	: QtnSinglePropertyValue<QtnPropertyQRectFBase>(parent)
{
	setMode(coordinates);
}

bool QtnPropertyQRectF::Register()
{
	qtnRegisterMetaPropertyFactory(
		QVariant::RectF, &QtnPropertyQRectF::internalCreateFrom);

	return QtnPropertyDelegateFactory::staticInstance().registerDelegateDefault(
			   &QtnPropertyQRectFBase::staticMetaObject,
			   &QtnPropertyDelegateQRectF::createLTWH, qtnQRectF_LTWH()) &&
		QtnPropertyDelegateFactory::staticInstance().registerDelegateDefault(
			&QtnPropertyQRectFBase::staticMetaObject,
			&QtnPropertyDelegateQRectF::createLTRB, qtnQRectF_LTRB());
}

QtnPropertyDelegateQRectF::QtnPropertyDelegateQRectF(
	QtnPropertyQRectFBase &owner, bool coordinates)
	: QtnPropertyDelegateTypedEx<QtnPropertyQRectFBase>(owner)
	, coordinates(coordinates)
{
	addSubProperty(owner.createLeftProperty(!coordinates));
	addSubProperty(owner.createTopProperty(!coordinates));

	if (coordinates)
	{
		addSubProperty(owner.createRightProperty(false));
		addSubProperty(owner.createBottomProperty(false));
	} else
	{
		addSubProperty(owner.createWidthProperty());
		addSubProperty(owner.createHeightProperty());
	}
}

QtnPropertyDelegate *QtnPropertyDelegateQRectF::createLTWH(QtnProperty &owner)
{
	auto theOwner = qobject_cast<QtnPropertyQRectFBase *>(&owner);

	if (!theOwner)
		return nullptr;

	return new QtnPropertyDelegateQRectF(*theOwner, false);
}

QtnPropertyDelegate *QtnPropertyDelegateQRectF::createLTRB(QtnProperty &owner)
{
	auto theOwner = qobject_cast<QtnPropertyQRectFBase *>(&owner);

	if (!theOwner)
		return nullptr;

	return new QtnPropertyDelegateQRectF(*theOwner, true);
}

QWidget *QtnPropertyDelegateQRectF::createValueEditorImpl(
	QWidget *parent, const QRect &rect, QtnInplaceInfo *inplaceInfo)
{
	return createValueEditorLineEdit(parent, rect, true, inplaceInfo);
}

bool QtnPropertyDelegateQRectF::propertyValueToStr(QString &strValue) const
{
	auto value = owner().value();

	QLocale locale;
	strValue =
		QtnPropertyQRect::getToStringFormat(coordinates)
			.arg(locale.toString(value.left(), 'g', 6),
				locale.toString(value.top(), 'g', 6),
				locale.toString(
					coordinates ? value.right() : value.width(), 'g', 6),
				locale.toString(
					coordinates ? value.bottom() : value.height(), 'g', 6));

	return true;
}
