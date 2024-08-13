#include "pch.h"
#include "Fisics.h"

#include "Input/InputManager.h"


void Fisics::init(float force, float maxVelocity)
{
	mForce = force;
	mMaxVelocity = maxVelocity;
	mDragFactor = 0.85f;
}


void Fisics::handleInput(InputManager* input)
{
	//mAcceleration.zero();
	//mHasForce.zero();

	//// Movement
	//if (input->isHeld(Button::Up))
	//{
	//	mAcceleration.y -= 1.0f;
	//	mHasForce.y = true;
	//}

	//if (input->isHeld(Button::Down))
	//{
	//	mAcceleration.y += 1.0f;
	//	mHasForce.y = true;
	//}

	//if (input->isHeld(Button::Left))
	//{
	//	mAcceleration.x -= 1.0f;
	//	mHasForce.x = true;
	//}

	//if (input->isHeld(Button::Right))
	//{
	//	mAcceleration.x += 1.0f;
	//	mHasForce.x = true;
	//}

	//mAcceleration = mAcceleration.normalise() * mForce;
}



void Fisics::fastUpdate(float dt)
{
	mVelocity += mAcceleration * dt;
	mVelocity = mVelocity.clamp( -mMaxVelocity, mMaxVelocity);
	applyDrag();
}


void Fisics::move(VectorF velocity, float dt)
{
	mRect = mRect.MoveCopy(velocity * dt);
}

void Fisics::move(float dt)
{
	mRect = mRect.MoveCopy(mVelocity * dt);
}

void Fisics::move(VectorF movement)
{
	mRect = mRect.MoveCopy(movement);
}


// TODO: I think because the force is huge, i.e. instanct accel the normalisation is essentially not doing anything...
// fix me!
void Fisics::accellerate(VectorF acceleration)
{
	if (acceleration.x != 0.0f)
	{
		mHasForce.x = true;
	}

	if (acceleration.y != 0.0f)
	{
		mHasForce.y = true;
	}

	mAcceleration = acceleration;
	mAcceleration = mAcceleration.clamp(- 1.0f, +1.0f);
	mAcceleration = mAcceleration.normalise() * mForce;
}


VectorF Fisics::direction() const
{
	return mVelocity.clamp(- 1.0f, +1.0f);
}


float Fisics::relativeSpeed() const
{ 
	return mVelocity.lengthSquared() / (mMaxVelocity * mMaxVelocity); 
}


void Fisics::reset()
{
	mRect.SetTopLeft(VectorF());
	mVelocity = VectorF();
	mAcceleration = VectorF();
	mFlip = SDL_FLIP_NONE;

	resetHasForce();
}


// --- Private Functions --- //
void Fisics::applyDrag()
{
	// No movement
	if (!mHasForce.x)
	{
		mVelocity.x = mVelocity.x * mDragFactor;

		if (mVelocity.x < mMaxVelocity * 0.01)
			mVelocity.x = 0;
	}

	if (!mHasForce.y)
	{
		mVelocity.y = mVelocity.y * mDragFactor;

		if (mVelocity.y < mMaxVelocity * 0.01)
			mVelocity.y = 0;
	}


	// Changing direction
	if (mVelocity.x > 0.0f && mAcceleration.x < 0.0f ||
		mVelocity.x < 0.0f && mAcceleration.x > 0.0f)
	{
		mVelocity.x = mVelocity.x * mDragFactor;
	}

	if (mVelocity.y > 0.0f && mAcceleration.y < 0.0f ||
		mVelocity.y < 0.0f && mAcceleration.y > 0.0f)
	{
		mVelocity.y = mVelocity.y * mDragFactor;
	}
}


VectorF Fisics::applyDrag(VectorF velocity, VectorF maxVelocity, VectorF acceleration, float drag)
{
	const float dragFractor = 1.0f - drag;
	const float minSpeedMultiple = 0.1f;

	// No movement, apply drag then halt at min speed
	if (!acceleration.x)
	{
		velocity.x = velocity.x * dragFractor;

		const float min_speed = maxVelocity.x * minSpeedMultiple;
		if (velocity.x < min_speed && velocity.x > -min_speed)
			velocity.x = 0;
	}
	if (!acceleration.y)
	{
		velocity.y = velocity.y * dragFractor;

		const float min_speed = maxVelocity.y * minSpeedMultiple;
		if (velocity.y < min_speed && velocity.y > -min_speed)
			velocity.y = 0;
	}

	// Changing direction
	if (velocity.x > 0.0f && acceleration.x < 0.0f ||
		velocity.x < 0.0f && acceleration.x > 0.0f)
	{
		velocity.x = velocity.x * dragFractor;
	}

	if (velocity.y > 0.0f && acceleration.y < 0.0f ||
		velocity.y < 0.0f && acceleration.y > 0.0f)
	{
		velocity.y = velocity.y * dragFractor;
	}

	return velocity;
}


void Fisics::facePoint(VectorF point)
{
	if (point.x > mRect.Center().x)		// -->
		mFlip = SDL_FLIP_NONE;
	else							// <--
		mFlip = SDL_FLIP_HORIZONTAL;
}