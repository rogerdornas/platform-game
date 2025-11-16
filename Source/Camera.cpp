//
// Created by roger on 23/04/2025.
//

#include "Camera.h"
#include "Random.h"

Camera::Camera(class Game* game, Vector2 startPosition)
    :mPos(startPosition)
    ,mGame(game)
    ,mCameraLerpSpeed(6.0f * mGame->GetScale())
    ,mNormalSpeed(6.0f * mGame->GetScale())
    ,mSlowTransitionSpeed(2.0f * mGame->GetScale())
    ,mInitPositionSpeed(20.0f * mGame->GetScale())
    ,mTransitionDuration(0.0f)
    ,mTransitionTimer(mTransitionDuration)
    ,mCameraMode(CameraMode::FollowPlayer)
    ,mFixedCameraPosition(Vector2::Zero)
    ,mLimitMinCameraPosition(Vector2(-1, -1))
    ,mLimitMaxCameraPosition(Vector2(-1, -1))
    ,mCurrentOffsetX(0.0f)
    ,mInitPositionTimer(0.1f)
    ,mIsAdjustingInitialPosition(true)
    ,mDistMove(200 * mGame->GetScale())
    ,mIsShaking(false)
    ,mShakeDuration(1.0f)
    ,mShakeTimer(0.0f)
    ,mShakeStrength(5.0f * mGame->GetScale())
    ,mCameraSpeed(500.0f * mGame->GetScale())
    ,mCameraVelocity(Vector2::Zero)
    ,mLookUp(false)
    ,mLookDown(false)
    ,mZoom(1.0f)
{
}

void Camera::StartCameraShake(float duration, float strength) {
    mIsShaking = true;
    mShakeTimer = 0.0f;
    mShakeDuration = duration;
    mShakeStrength = strength;
}

void Camera::ChangeCameraMode(CameraMode cameraMode) {
    if (mCameraMode != cameraMode) {
        mTransitionTimer = 0;
    }
    mCameraMode = cameraMode;
}

void Camera::SetLimitMinCameraPosition(Vector2 pos) {
    if (mLimitMinCameraPosition.x != pos.x ||
        mLimitMinCameraPosition.y != pos.y)
    {
        mLimitMinCameraPosition = pos;
        mCurrentLimitMinPosition = mPos;
    }
}

void Camera::SetLimitMaxCameraPosition(Vector2 pos) {
    if (mLimitMaxCameraPosition.x != pos.x ||
        mLimitMaxCameraPosition.y != pos.y)
    {
        mLimitMaxCameraPosition = pos;
        mCurrentLimitMaxPosition = mPos + Vector2(mGame->GetRenderer()->GetZoomedWidth(), mGame->GetRenderer()->GetZoomedHeight());
    }
}


void Camera::Update(float deltaTime) {
    // Ajuste rápido de câmera no início de cenas
    if (mInitPositionTimer > 0) {
        mInitPositionTimer -= deltaTime;
        mCameraLerpSpeed = mInitPositionSpeed;
        mIsAdjustingInitialPosition = true;
    }
    else {
        mIsAdjustingInitialPosition = false;
    }

    if (mCameraMode == CameraMode::FollowPlayerLimited) {
        // Atualiza limites suavizados
        if (!mIsAdjustingInitialPosition) {
            mCameraLerpSpeed = mSlowTransitionSpeed;
        }
        mCurrentLimitMinPosition = Vector2::Lerp(mCurrentLimitMinPosition, mLimitMinCameraPosition, mCameraLerpSpeed * deltaTime);
        mCurrentLimitMaxPosition = Vector2::Lerp(mCurrentLimitMaxPosition, mLimitMaxCameraPosition, mCameraLerpSpeed * deltaTime);

        // OFFSET HORIZONTAL BASEADO NA ROTAÇÃO
        float desiredOffsetX = mGame->GetPlayer()->GetWidth() * 2.5f * mGame->GetPlayer()->GetForward().x;

        // Suaviza a transição do offset
        if (!mIsAdjustingInitialPosition) {
            mCameraLerpSpeed = mSlowTransitionSpeed;
        }
        mCurrentOffsetX = Math::Lerp(mCurrentOffsetX, desiredOffsetX, mCameraLerpSpeed * deltaTime);

        // Calcula posição alvo do player
        Vector2 playerPos = mGame->GetPlayer()->GetPosition();
        Vector2 playerPosOffset(
            playerPos.x - mGame->GetRenderer()->GetZoomedWidth() / 2 + mCurrentOffsetX,
            playerPos.y - mGame->GetRenderer()->GetZoomedHeight() / 2
        );

        // Aplica limites já suavizados ao alvo
        playerPosOffset.x = Math::Clamp(playerPosOffset.x, mCurrentLimitMinPosition.x, mCurrentLimitMaxPosition.x - mGame->GetRenderer()->GetZoomedWidth());
        playerPosOffset.y = Math::Clamp(playerPosOffset.y, mCurrentLimitMinPosition.y, mCurrentLimitMaxPosition.y - mGame->GetRenderer()->GetZoomedHeight());

        // Aplica deslocamento vertical se estiver olhando para cima ou para baixo
        if (mLookUp) {
            playerPosOffset.y -= mDistMove;
        }
        else if (mLookDown) {
            playerPosOffset.y += mDistMove;
        }

        // Interpola da posição atual até a posição alvo (já limitada)
        if (!mIsAdjustingInitialPosition) {
            mCameraLerpSpeed = mNormalSpeed;
        }
        mPos = Vector2::Lerp(mPos, playerPosOffset, mCameraLerpSpeed * deltaTime);

        // Camera Shake
        if (mShakeTimer < mShakeDuration) {
            mShakeTimer += deltaTime;
        }

        float shakeOffsetX = 0;
        float shakeOffsetY = 0;

        if (mIsShaking) {
            shakeOffsetX = Random::GetFloat() * (2 * mShakeStrength + 1) - mShakeStrength;
            shakeOffsetY = Random::GetFloat() * (2 * mShakeStrength + 1) - mShakeStrength;

            if (mShakeTimer >= mShakeDuration) {
                mIsShaking = false;
                shakeOffsetX = 0;
                shakeOffsetY = 0;
            }
        }
        mPos.x += shakeOffsetX * mCameraLerpSpeed * deltaTime;
        mPos.y += shakeOffsetY * mCameraLerpSpeed * deltaTime;

        // Reset flags para o próximo frame
        mLookUp = false;
        mLookDown = false;

        return;
    }

    if (mTransitionTimer < mTransitionDuration) {
        mTransitionTimer += deltaTime;
        mCameraLerpSpeed = mSlowTransitionSpeed;
    }
    else {
        mCameraLerpSpeed = mNormalSpeed;
    }

    Vector2 targetPosition = Vector2::Zero;
    switch (mCameraMode) {
        case CameraMode::Fixed:
            targetPosition = Fixed(Vector2(mFixedCameraPosition));
            break;

        case CameraMode::FollowPlayer:
            targetPosition = FollowPlayer();
            break;

        case CameraMode::FollowPlayerHorizontally:
            targetPosition = FollowPlayerHorizontally();
            break;

        case CameraMode::FollowPlayerLimitRight:
            targetPosition = FollowPlayerLimitRight();
            break;

        case CameraMode::FollowPlayerLimitLeft:
            targetPosition = FollowPlayerLimitLeft();
            break;

        case CameraMode::FollowPlayerLimitUp:
            targetPosition = FollowPlayerLimitUp();
            break;

        case CameraMode::FollowPlayerLimitDown:
            targetPosition = FollowPlayerLimitDown();
            break;

        case CameraMode::FollowPlayerLimitRightHorizontally:
            targetPosition = FollowPlayerLimitRightHorizontally();
            break;

        case CameraMode::FollowPlayerLimitLeftHorizontally:
            targetPosition = FollowPlayerLimitLeftHorizontally();
            break;

        case CameraMode::ScrollRight:
            targetPosition = ScrollRight(deltaTime, mCameraSpeed);
            break;

        case CameraMode::ScrollUp:
            targetPosition = ScrollUp(deltaTime, -mCameraSpeed / 2);
            break;

        case CameraMode::PanoramicCamera:
            targetPosition = PanoramicCamera(deltaTime);
            break;
    }

    // Aplica deslocamento vertical se estiver olhando para cima ou para baixo
    if (mLookUp) {
        targetPosition.y -= mDistMove;
    }
    else if (mLookDown) {
        targetPosition.y += mDistMove;
    }

    // Interpola camera se estiver seguindo o player
    if (mCameraMode == CameraMode::FollowPlayer ||
        mCameraMode == CameraMode::FollowPlayerHorizontally ||
        mCameraMode == CameraMode::FollowPlayerLimitRight ||
        mCameraMode == CameraMode::FollowPlayerLimitLeft ||
        mCameraMode == CameraMode::FollowPlayerLimitUp ||
        mCameraMode == CameraMode::FollowPlayerLimitDown ||
        mCameraMode == CameraMode::FollowPlayerLimitRightHorizontally ||
        mCameraMode == CameraMode::FollowPlayerLimitLeftHorizontally )
    {
        mPos = Vector2(int(mPos.x + (targetPosition.x - mPos.x) * mCameraLerpSpeed * deltaTime),
                       int(mPos.y + (targetPosition.y - mPos.y) * mCameraLerpSpeed * deltaTime));
    }
    else if (mCameraMode == CameraMode::ScrollRight) {
        mPos = Vector2(targetPosition.x,
                    int(mPos.y + (targetPosition.y - mPos.y) * mCameraLerpSpeed * deltaTime));
    }
    else if (mCameraMode == CameraMode::ScrollUp) {
        mPos = Vector2(int(mPos.x + (targetPosition.x - mPos.x) * mCameraLerpSpeed * deltaTime),
                        targetPosition.y);
    }
    else if (mCameraMode == CameraMode::Fixed) {
        // mPos = targetPosition;
        mPos = Vector2(int(mPos.x + (targetPosition.x - mPos.x) * mCameraLerpSpeed * deltaTime),
               int(mPos.y + (targetPosition.y - mPos.y) * mCameraLerpSpeed * deltaTime));
    }
    else if (mCameraMode == CameraMode::PanoramicCamera) {
        mPos = targetPosition;
    }


    // Camera Shake
    if (mShakeTimer < mShakeDuration) {
        mShakeTimer += deltaTime;
    }

    float shakeOffsetX = 0;
    float shakeOffsetY = 0;

    if (mIsShaking) {
        shakeOffsetX = Random::GetFloat() * (2 * mShakeStrength + 1) - mShakeStrength;
        shakeOffsetY = Random::GetFloat() * (2 * mShakeStrength + 1) - mShakeStrength;

        if (mShakeTimer >= mShakeDuration) {
            mIsShaking = false;
            shakeOffsetX = 0;
            shakeOffsetY = 0;
        }
    }
    mPos.x += shakeOffsetX * mCameraLerpSpeed * deltaTime;
    mPos.y += shakeOffsetY * mCameraLerpSpeed * deltaTime;

    // Reset flags para o próximo frame
    mLookUp = false;
    mLookDown = false;
}

Vector2 Camera::Fixed(Vector2 pos) {
    return pos;
}

Vector2 Camera::FollowPlayer() {
    Vector2 playerPos = mGame->GetPlayer()->GetPosition();
    Vector2 targetPos(playerPos.x - mGame->GetRenderer()->GetZoomedWidth() / 2,
                      playerPos.y - mGame->GetRenderer()->GetZoomedHeight() / 2);
    return targetPos;
}

Vector2 Camera::FollowPlayerHorizontally() {
    Vector2 playerPos = mGame->GetPlayer()->GetPosition();
    Vector2 targetPos(playerPos.x - mGame->GetRenderer()->GetZoomedWidth() / 2,
                      mFixedCameraPosition.y);
    return targetPos;
}

Vector2 Camera::FollowPlayerLimitRight() {
    Vector2 playerPos = mGame->GetPlayer()->GetPosition();
    Vector2 targetPos(playerPos.x - mGame->GetRenderer()->GetZoomedWidth() / 2,
                      playerPos.y - mGame->GetRenderer()->GetZoomedHeight() / 2);

    if (targetPos.x + mGame->GetRenderer()->GetZoomedWidth() > mFixedCameraPosition.x) {
        targetPos.x = mFixedCameraPosition.x - mGame->GetRenderer()->GetZoomedWidth();
    }
    return targetPos;
}

Vector2 Camera::FollowPlayerLimitLeft() {
    Vector2 playerPos = mGame->GetPlayer()->GetPosition();
    Vector2 targetPos(playerPos.x - mGame->GetRenderer()->GetZoomedWidth() / 2,
                      playerPos.y - mGame->GetRenderer()->GetZoomedHeight() / 2);

    if (targetPos.x < mFixedCameraPosition.x) {
        targetPos.x = mFixedCameraPosition.x;
    }
    return targetPos;
}

Vector2 Camera::FollowPlayerLimitUp() {
    Vector2 playerPos = mGame->GetPlayer()->GetPosition();
    Vector2 targetPos(playerPos.x - mGame->GetRenderer()->GetZoomedWidth() / 2,
                      playerPos.y - mGame->GetRenderer()->GetZoomedHeight() / 2);

    if (targetPos.y < mFixedCameraPosition.y) {
        targetPos.y = mFixedCameraPosition.y;
    }
    return targetPos;
}

Vector2 Camera::FollowPlayerLimitDown() {
    Vector2 playerPos = mGame->GetPlayer()->GetPosition();
    Vector2 targetPos(playerPos.x - mGame->GetRenderer()->GetZoomedWidth() / 2,
                      playerPos.y - mGame->GetRenderer()->GetZoomedHeight() / 2);

    if (targetPos.y + mGame->GetRenderer()->GetZoomedHeight() > mFixedCameraPosition.y) {
        targetPos.y = mFixedCameraPosition.y - mGame->GetRenderer()->GetZoomedHeight();
    }
    return targetPos;
}

Vector2 Camera::FollowPlayerLimitRightHorizontally() {
    Vector2 playerPos = mGame->GetPlayer()->GetPosition();
    Vector2 targetPos(playerPos.x - mGame->GetRenderer()->GetZoomedWidth() / 2,
                      mFixedCameraPosition.y);

    if (targetPos.x + mGame->GetRenderer()->GetZoomedWidth() > mFixedCameraPosition.x) {
        targetPos.x = mFixedCameraPosition.x - mGame->GetRenderer()->GetZoomedWidth();
    }
    return targetPos;
}

Vector2 Camera::FollowPlayerLimitLeftHorizontally() {
    Vector2 playerPos = mGame->GetPlayer()->GetPosition();
    Vector2 targetPos(playerPos.x - mGame->GetRenderer()->GetZoomedWidth() / 2,
                      mFixedCameraPosition.y);

    if (targetPos.x < mFixedCameraPosition.x) {
        targetPos.x = mFixedCameraPosition.x;
    }
    return targetPos;
}

Vector2 Camera::ScrollRight(float deltaTime, float speed) {
    Vector2 targetPos = GetPosCamera();
    if (mGame->GetGamePlayState() == Game::GamePlayState::Playing) {
        Vector2 playerPos = mGame->GetPlayer()->GetPosition();
        targetPos.x = mPos.x + speed * deltaTime;
        targetPos.y = playerPos.y - mGame->GetRenderer()->GetZoomedHeight() / 2;

        if (playerPos.x < targetPos.x - 50 * mGame->GetScale()) {
            mGame->SetBackToCheckpoint();
            mGame->GetAudio()->StopAllSounds();
            mGame->GetPlayer()->SetState(ActorState::Paused);
            mFixedCameraPosition = mPos - Vector2(mGame->GetRenderer()->GetZoomedWidth() / 2, 0);
            mCameraMode = CameraMode::Fixed;
        }
    }
    return targetPos;
}

Vector2 Camera::ScrollUp(float deltaTime, float speed) {
    Vector2 targetPos = GetPosCamera();
    if (mGame->GetGamePlayState() == Game::GamePlayState::Playing) {
        Vector2 playerPos = mGame->GetPlayer()->GetPosition();
        targetPos.x = playerPos.x - mGame->GetRenderer()->GetZoomedWidth() / 2;
        targetPos.y = mPos.y + speed * deltaTime;


        if (playerPos.y > targetPos.y + mGame->GetRenderer()->GetZoomedHeight() + 50 * mGame->GetScale()) {
            mGame->SetBackToCheckpoint();
            mGame->GetAudio()->StopAllSounds();
            mGame->GetPlayer()->SetState(ActorState::Paused);
            mFixedCameraPosition = mPos + Vector2(0, mGame->GetRenderer()->GetZoomedHeight() / 2);
            mCameraMode = CameraMode::Fixed;
        }
    }
    return targetPos;
}

Vector2 Camera::PanoramicCamera(float deltaTime) {
    Vector2 targetPos = GetPosCamera();
    targetPos.x = mPos.x + mCameraVelocity.x * deltaTime;
    targetPos.y = mPos.y + mCameraVelocity.y * deltaTime;
    return targetPos;
}

void Camera::SetZoom(float zoom)
{
    // Limita o zoom para evitar valores inválidos
    if (zoom < 0.01f)
    {
        zoom = 0.01f;
    }

    mZoom = zoom;

    // Informa ao Renderer sobre o novo zoom
    // (Isso recalcula a matriz de projeção)
    mGame->GetRenderer()->SetZoom(mZoom);
}

void Camera::ChangeResolution(float oldScale, float newScale) {
    mPos.x = mPos.x / oldScale * newScale;
    mPos.y = mPos.y / oldScale * newScale;
    mDistMove = mDistMove / oldScale * newScale;
    mShakeStrength = mShakeStrength / oldScale * newScale;
    mCameraSpeed = mCameraSpeed / oldScale * newScale;
    mNormalSpeed = mNormalSpeed / oldScale * newScale;
    mSlowTransitionSpeed = mSlowTransitionSpeed / oldScale * newScale;
    mInitPositionSpeed = mInitPositionSpeed / oldScale * newScale;
    mCurrentOffsetX = mCurrentOffsetX / oldScale * newScale;
    mFixedCameraPosition.x = mFixedCameraPosition.x / oldScale * newScale;
    mFixedCameraPosition.y = mFixedCameraPosition.y / oldScale * newScale;
    mLimitMinCameraPosition.x = mLimitMinCameraPosition.x / oldScale * newScale;
    mLimitMinCameraPosition.y = mLimitMinCameraPosition.y / oldScale * newScale;
    mLimitMaxCameraPosition.x = mLimitMaxCameraPosition.x / oldScale * newScale;
    mLimitMaxCameraPosition.y = mLimitMaxCameraPosition.y / oldScale * newScale;
    mCurrentLimitMinPosition.x = mCurrentLimitMinPosition.x / oldScale * newScale;
    mCurrentLimitMinPosition.y = mCurrentLimitMinPosition.y / oldScale * newScale;
    mCurrentLimitMaxPosition.x = mCurrentLimitMaxPosition.x / oldScale * newScale;
    mCurrentLimitMaxPosition.y = mCurrentLimitMaxPosition.y / oldScale * newScale;
}
