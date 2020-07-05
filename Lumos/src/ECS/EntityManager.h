#pragma once

#include "Entity.h"
#include <entt/entt.hpp>

namespace Lumos
{

	class Scene;
	class Entity;

	template<class T>
	class EntityView
	{
		entt::view<T> m_View;
	};

	template<typename... Components>
	class EntityGroup
	{
		entt::sparse_set<entt::entity>::iterator begin()
		{
			return m_Group.begin();
		}
		entt::sparse_set<entt::entity>::iterator end()
		{
			return m_Group.end();
		}

		entt::group<Components...> m_Group;
	};

	template<typename...>
	struct TypeList
	{
	};

	class EntityManager
	{
	public:
		EntityManager(Scene* scene)
			: m_Scene(scene)
		{
		}

		Entity Create();

		template<typename... Components>
		auto GetEntitiesWithTypes()
		{
			return m_Registry.group<Components...>();
		}

		template<typename Component>
		auto GetEntitiesWithType()
		{
			return m_Registry.view<Component>();
		}

		entt::registry& GetRegistry()
		{
			return m_Registry;
		}

		void Clear();

	private:
		Scene* m_Scene = nullptr;
		entt::registry m_Registry;
	};
}
