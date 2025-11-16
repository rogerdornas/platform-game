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

find_package(OpenGL REQUIRED)

set(GLEW_INCLUDE_DIRS "C:/Arquivos de Programas/Glew/include")
set(GLEW_LIBRARIES "C:/Arquivos de Programas/Glew/lib/Release/x64/glew32.lib")
include_directories(${GLEW_INCLUDE_DIRS})
link_libraries(${GLEW_LIBRARIES})

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
        Source/Actors/Fox.cpp
        Source/Actors/Fox.h
        Source/Actors/Trigger.cpp
        Source/Actors/Trigger.h
        Source/Actors/DynamicGround.cpp
        Source/Actors/DynamicGround.h
        Source/Actors/Lever.cpp
        Source/Actors/Lever.h
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
        Source/SaveData.cpp
        Source/SaveData.h
        Source/SaveManager.cpp
        Source/SaveManager.h
        Source/Actors/Decorations.cpp
        Source/Actors/Decorations.h
        Source/Actors/Snake.cpp
        Source/Actors/Snake.h
        Source/Actors/Mushroom.cpp
        Source/Actors/Mushroom.h
        Source/Actors/CloneEnemy.cpp
        Source/Actors/CloneEnemy.h
        Source/Actors/MirrorBoss.cpp
        Source/Actors/MirrorBoss.h
        Source/Actors/Light.cpp
        Source/Actors/Light.h
        Source/Renderer/Renderer.cpp
        Source/Renderer/Renderer.h
        Source/Renderer/VertexArray.h
        Source/Renderer/VertexArray.cpp
        Source/Renderer/Texture.cpp
        Source/Renderer/Texture.h
        Source/Renderer/Shader.cpp
        Source/Renderer/Shader.h
        Source/Components/Drawing/DrawComponent.cpp
        Source/Components/Drawing/DrawComponent.h
        Source/Components/Drawing/AnimatorComponent.cpp
        Source/Components/Drawing/AnimatorComponent.h
        Source/Components/Drawing/RectComponent.cpp
        Source/Components/Drawing/RectComponent.h
        Source/Components/Drawing/TileMapComponent.cpp
        Source/Components/Drawing/TileMapComponent.h
)


#target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
#target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2_image::SDL2_image)
target_link_libraries(${PROJECT_NAME} PRIVATE ${GLEW_LIBRARIES} SDL2::SDL2 SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf SDL2_mixer::SDL2_mixer OpenGL::GL)
