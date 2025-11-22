#pragma once
#include "Framework/Object.h"
#include "SFML/Graphics.hpp"
#include "Framework/Core.h"
#include "box2d/id.h"

namespace we
{
	class World;

	class Actor : public Object
	{
	public:
		Actor(World* OwningWorld, const std::string& TexturePath = "");
		virtual ~Actor();

		void BeginPlayGlobal();
		void TickGlobal(float DeltaTime);
		virtual void BeginPlay();
		virtual void Tick(float DeltaTime);
		virtual void Render(sf::RenderWindow& Window);

		World* GetWorld() const { return OwningWorld; }

		sf::Vector2u GetWindowSize() const;

		void SetTexture(const std::string& TexturePath, float SpriteScale = 1.f);
		void SetTextureRect(const sf::IntRect& Rect);
		void SetActorScale(float NewScale);
		bool IsOutOfBounds() const;

		void SetActorLocation(const sf::Vector2f& NewLocation);
		void SetActorRotation(const sf::Angle& NewRotation);
		sf::Vector2f GetActorLocation() const;
		sf::Angle GetActorRotation() const;
		void AddActorLocationOffset(const sf::Vector2f& Offset);
		void AddActorRotationOffset(const sf::Angle& RotOffset);
		sf::Vector2f GetActorFowardVector() const;
		sf::Vector2f GetActorRightVector() const;
		sf::FloatRect GetSpriteBounds() const;

		void SetEnablePhysics(bool Enabled);

	private:
		World* OwningWorld;
		bool bHasBegunPlay;
		bool bPhysicsEnabled;

		shared<sf::Texture> ATexture;
		shared<sf::Sprite> ASprite;
		void CenterPivot();

		sf::Vector2i FrameSize{ 0, 0 };
		int CurrentFrame = 0;
		b2BodyId APhysicsBody;
		void InitializePhysics();
		void UninitializePhysics();
		void UpdatePhysicsTransforms();
	};
}