//
// Created by roger on 16/08/2025.
//

#include "ColliderComponent.h"

ColliderComponent::ColliderComponent(class Actor *owner, ColliderType type)
    :Component(owner)
    ,mType(type)
    ,mIsActive(true)
{
}
