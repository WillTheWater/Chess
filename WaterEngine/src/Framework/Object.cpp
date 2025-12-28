#include "Framework/Object.h"
#include "Framework/Core.h"

namespace we
{
	Object::Object()
		: bIsPendingDestroy{ false }
	{

	}

	Object::~Object()
	{
	}

	void Object::Destroy()
	{
		OnDestroy.Broadcast(this);
		bIsPendingDestroy = true;
	}

	weak<Object> Object::GetWeakObject()
	{
		return weak_from_this();
	}
	weak<const Object> Object::GetWeakObject() const
	{
		return weak_from_this();
	}
}