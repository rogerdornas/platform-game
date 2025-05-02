cmake_minimum_required(VERSION 3.26)
# Configurando o padrão c++
set(CMAKE_CXX_STANDARD 17)

# Configurando o nome e executável do projeto
set(PROJECT_NAME "game-v0")
project(${PROJECT_NAME})

find_package(SDL2 2.32.2 EXACT REQUIRED)
add_executable(${PROJECT_NAME}
        Source/Main.cpp
        Source/Math.cpp
        Source/Random.cpp
        Source/Actors/Actor.cpp
        Source/Components/Component.cpp
        Source/Game.cpp
        Source/Components/DrawComponent.cpp
        Source/Components/DrawComponent.h
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
        Source/Actors/Boss1.cpp
        Source/Actors/Boss1.h
)


target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
