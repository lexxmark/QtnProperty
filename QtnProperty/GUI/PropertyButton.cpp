/*******************************************************************************
Copyright (c) 2012-2016 Alex Zhondin <lexxmark.dev@gmail.com>
Copyright (c) 2019 Alexandra Cherdantseva <neluhus.vagus@gmail.com>

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

#include "PropertyButton.h"

QtnPropertyButton::QtnPropertyButton(QObject *parent)
	: QtnProperty(parent)
{
	switchState(QtnPropertyStateNonSerialized, true);
}

void QtnPropertyButton::invokeClick()
{
	emit click(this);
}

void QtnPropertyButton::setClickHandler(
	const std::function<void(const QtnPropertyButton *)> &clickHandler)
{
	QObject::connect(this, &QtnPropertyButton::click, clickHandler);
}

void QtnPropertyButton::invokePreDrawButton(QStyleOptionButton *option)
{
	Q_ASSERT(option);
	emit preDrawButton(this, option);
}

bool QtnPropertyButton::fromStrImpl(const QString &, QtnPropertyChangeReason)
{
	return false;
}

bool QtnPropertyButton::toStrImpl(QString &) const
{
	return false;
}
