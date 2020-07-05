#pragma once
#include "lmpch.h"
#include "Constraint.h"

namespace Lumos
{
	class RigidBody3D;

	class LUMOS_EXPORT SpringConstraint : public Constraint
	{
	public:
		SpringConstraint(RigidBody3D* obj1, RigidBody3D* obj2, const Maths::Vector3& globalOnA, const Maths::Vector3& globalOnB, float springConstant, float dampingFactor);

		virtual void ApplyImpulse() override;
		virtual void DebugDraw() const override;

	protected:
		RigidBody3D* m_pObj1;
		RigidBody3D* m_pObj2;

		float m_restDistance;

		float m_springConstant;
		float m_dampingFactor;

		Maths::Vector3 m_LocalOnA;
		Maths::Vector3 m_LocalOnB;
	};
}