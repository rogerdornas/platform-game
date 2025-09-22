cmake_minimum_required(VERSION 3.26)
# Configurando o padrão c++
set(CMAKE_CXX_STANDARD 17)

# Configurando o nome e executável do projeto
set(PROJECT_NAME "Echoes_of_Elementum")
project(${PROJECT_NAME})

find_package(SDL2 2.32.2 EXACT REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

add_executable(${PROJECT_NAME}
        Source/Main.cpp
        Source/Math.cpp
        Source/Random.cpp
        Source/Actors/Actor.cpp
        Source/Components/Component.cpp
        Source/Game.cpp
        Source/Components/RigidBodyComponent.cpp
        Source/Components/RigidBodyComponent.h
        Source/Components/CircleColliderComponent.cpp
        Source/Components/CircleColliderComponent.h
        Source/Actors/Particle.cpp
        Source/Actors/Particle.h
        Source/Actors/ParticleSystem.cpp
        Source/Actors/ParticleSystem.h
        Source/Actors/Player.cpp
        Source/Actors/Player.h
        Source/Components/AABBComponent.cpp
        Source/Components/AABBComponent.h
        Source/Actors/Ground.cpp
        Source/Actors/Ground.h
        Source/Camera.cpp
        Source/Camera.h
        Source/Components/DashComponent.cpp
        Source/Components/DashComponent.h
        Source/Actors/Sword.cpp
        Source/Actors/Sword.h
        Source/Actors/FireBall.cpp
        Source/Actors/FireBall.h
        Source/Actors/EnemySimple.cpp
        Source/Actors/EnemySimple.h
        Source/Actors/Enemy.cpp
        Source/Actors/Enemy.h
        Source/Actors/FlyingEnemySimple.cpp
        Source/Actors/FlyingEnemySimple.h
        Source/Json.h
        Source/CSV.h
        Source/Components/DrawComponents/DrawAnimatedComponent.cpp
        Source/Components/DrawComponents/DrawAnimatedComponent.h
        Source/Components/DrawComponents/DrawComponent.cpp
        Source/Components/DrawComponents/DrawComponent.h
        Source/Components/DrawComponents/DrawPolygonComponent.cpp
        Source/Components/DrawComponents/DrawPolygonComponent.h
        Source/Components/DrawComponents/DrawSpriteComponent.cpp
        Source/Components/DrawComponents/DrawSpriteComponent.h
        Source/Components/DrawComponents/DrawGroundSpritesComponent.cpp
        Source/Components/DrawComponents/DrawGroundSpritesComponent.h
        Source/Actors/Fox.cpp
        Source/Actors/Fox.h
        Source/Actors/Trigger.cpp
        Source/Actors/Trigger.h
        Source/Actors/DynamicGround.cpp
        Source/Actors/DynamicGround.h
        Source/Components/DrawComponents/DrawDynamicGroundSpritesComponent.cpp
        Source/Components/DrawComponents/DrawDynamicGroundSpritesComponent.h
        Source/Actors/Lever.cpp
        Source/Actors/Lever.h
        Source/Components/DrawComponents/DrawParticleComponent.cpp
        Source/Components/DrawComponents/DrawParticleComponent.h
        Source/Actors/Effect.cpp
        Source/Actors/Effect.h
        Source/Actors/Frog.cpp
        Source/Actors/Frog.h
        Source/Actors/FrogTongue.cpp
        Source/Actors/FrogTongue.h
        Source/AudioSystem.cpp
        Source/AudioSystem.h
        Source/Actors/FlyingShooterEnemy.cpp
        Source/Actors/FlyingShooterEnemy.h
        Source/Actors/Projectile.cpp
        Source/Actors/Projectile.h
        Source/UIElements/UIScreen.h
        Source/UIElements/UIScreen.cpp
        Source/UIElements/UIFont.h
        Source/UIElements/UIFont.cpp
        Source/UIElements/UIButton.cpp
        Source/UIElements/UIButton.h
        Source/UIElements/UIText.cpp
        Source/UIElements/UIText.h
        Source/UIElements/UIElement.cpp
        Source/UIElements/UIElement.h
        Source/UIElements/UIImage.cpp
        Source/UIElements/UIImage.h
        Source/HUD.cpp
        Source/Actors/Money.cpp
        Source/Actors/Money.h
        Source/Actors/Checkpoint.cpp
        Source/Actors/Checkpoint.h
        Source/Store.cpp
        Source/Store.h
        Source/Actors/Mantis.cpp
        Source/Actors/Mantis.h
        Source/Actors/Moth.cpp
        Source/Actors/Moth.h
        Source/Actors/Golem.cpp
        Source/Actors/Golem.h
        Source/Actors/Skill.cpp
        Source/Actors/Skill.h
        Source/Actors/Fairy.cpp
        Source/Actors/Fairy.h
        Source/DialogueSystem.cpp
        Source/DialogueSystem.h
        Source/Cutscene.cpp
        Source/Cutscene.h
        Source/Actors/BushMonster.cpp
        Source/Actors/BushMonster.h
        Source/Actors/HookPoint.cpp
        Source/Actors/HookPoint.h
        Source/Components/DrawComponents/DrawRopeComponent.cpp
        Source/Components/DrawComponents/DrawRopeComponent.h
        Source/Actors/FlyingGolem.cpp
        Source/Actors/FlyingGolem.h
        Source/Actors/DragonFly.cpp
        Source/Actors/DragonFly.h
        Source/Components/ColliderComponent.cpp
        Source/Components/ColliderComponent.h
        Source/Components/OBBComponent.cpp
        Source/Components/OBBComponent.h
        Source/Map.cpp
        Source/Map.h
        Source/WaveManager.cpp
        Source/WaveManager.h
        Source/Actors/Lava.cpp
        Source/Actors/Lava.h
        Source/Actors/JumpEffect.cpp
        Source/Actors/JumpEffect.h
        Source/Actors/HookEnemy.cpp
        Source/Actors/HookEnemy.h
        Source/Actors/Spawner.cpp
        Source/Actors/Spawner.h
        Source/Actors/FlyingSpawnerEnemy.cpp
        Source/Actors/FlyingSpawnerEnemy.h
        Source/Actors/LittleBat.cpp
        Source/Actors/LittleBat.h
)


#target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
#target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2_image::SDL2_image)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf SDL2_mixer::SDL2_mixer)

