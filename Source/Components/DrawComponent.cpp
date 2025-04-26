//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "DrawComponent.h"
#include "CircleColliderComponent.h"
#include "AABBComponent.h"
#include "../Actors/Actor.h"
#include "../Game.h"

DrawComponent::DrawComponent(class Actor* owner, std::vector<Vector2> &vertices, int drawOrder)
    :Component(owner)
    ,mVertices(vertices)
    ,mDrawOrder(drawOrder)
{
    mOwner->GetGame()->AddDrawable(this);
}

DrawComponent::~DrawComponent()
{
    mOwner->GetGame()->RemoveDrawable(this);
}

void DrawComponent::DrawPolygon(SDL_Renderer *renderer, std::vector<Vector2>& vertices)
{
    Vector2 pos = mOwner->GetPosition();

    // --------------
    // TODO - PARTE 1
    // --------------

    // TODO 1.1 (~3 linhas): percorra do primeiro até o penúltimo vértices, utilizando a função
    //  SDL_RenderDrawLine para desenhar linhas entre os vértices i e i+1. Some a posição do dono
    //  do componente (pos) a cada vértice.
    for (int i = 0; i < vertices.size() - 1; i++) {
        SDL_RenderDrawLine(renderer,
                        vertices[i].x + pos.x, vertices[i].y + pos.y,
                        vertices[i+1].x + pos.x, vertices[i+1].y + pos.y);
    }

    // TODO 1.2 (~1 linha): Utilize a função SDL_RenderDrawLine para desenhar uma linha entre o último
    //  e o primeiro vértice. Some a posição do dono do componente (pos) a cada vértice.
    SDL_RenderDrawLine(renderer,
                       vertices[vertices.size() - 1].x + pos.x, vertices[vertices.size() - 1].y + pos.y,
                       vertices[0].x + pos.x, vertices[0].y + pos.y);
}

void DrawComponent::DrawCircle(SDL_Renderer *renderer, const Vector2& center, const float radius, const int numVertices)
{
    std::vector<Vector2> vertices;

    // --------------
    // TODO - PARTE 1
    // --------------

    // TODO 2.1 (~1 linha): inicialize variável angle (float) com zero. Ela será
    //  utilizada para percorrer o arco de uma circunferência em intervalos angulares de tamanho fixo.
    float angle = 0;

    // TODO 2.2 (~6 linhas): Repita o seguinte procedimento para um dado número de vértices (numVertices):
    //  (a) Calcule a coordenada x do novo vértice multiplicando o raio da circunferência (radius) pelo cosseno
    //  do ângulo corrente (angle);
    //  (b) Calcule a coordenada y da mesma forma, porém multiplicando pelo seno do ângulo corrente;
    //  (c) Some o vetor (x,y) com o centro (center) e adicione o vetor resultante ao conjunto de vértices (vertices);
    //  (d) Incremente o ângulo corrente por 2*PI dividido pelo número de vértices (numVertices).
    for (int i =0; i < numVertices; i++) {
        float x = radius * Math::Cos(angle);
        float y = radius * Math::Sin(angle);
        vertices.push_back(Vector2(center.x + x, center.y + y));
        angle += 2*Math::Pi / numVertices;
    }

    DrawComponent::DrawPolygon(renderer, vertices);
}

void DrawComponent::DrawAABB(SDL_Renderer *renderer, const Vector2 &min, const Vector2 &max) {
    if (mOwner->GetComponent<AABBComponent>() == nullptr) {
        return;
    }

    Vector2 pos = mOwner->GetPosition();
    Vector2 cam = GetGame()->GetCamera()->GetPosCamera();

    SDL_RenderDrawLine(renderer,
                    min.x + pos.x - cam.x, min.y + pos.y - cam.y,
                    max.x + pos.x - cam.x, min.y + pos.y - cam.y);
    SDL_RenderDrawLine(renderer,
                    max.x + pos.x - cam.x, min.y + pos.y - cam.y,
                    max.x + pos.x - cam.x, max.y + pos.y - cam.y);
    SDL_RenderDrawLine(renderer,
                    max.x + pos.x - cam.x, max.y + pos.y - cam.y,
                    min.x + pos.x - cam.x, max.y + pos.y - cam.y);
    SDL_RenderDrawLine(renderer,
                    min.x + pos.x - cam.x, max.y + pos.y - cam.y,
                    min.x + pos.x - cam.x, min.y + pos.y - cam.y);
}


void DrawComponent::Draw(SDL_Renderer *renderer)
{
    // --------------
    // TODO - PARTE 1
    // --------------

    // TODO 3.1 (~1 linha): Utilize a função Matrix3::CreateRotation para criar uma matriz de rotação com o ângulo
    //  do dono desse componente (-mOwner->GetRotation()).
    Matrix3 rotationMatrix = Matrix3::CreateRotation(mOwner->GetRotation());

    // TODO 3.2 (~4 linhas): Percorra os vértices desse componente (mVertices), multiplicando-os pela matriz de
    //  rotação com a função Vector2::Transform. Adicione o vetor transformado à um vetor temporária de vertices.
    std::vector<Vector2> vertices;
    for (Vector2 v : mVertices) {
        vertices.push_back(Vector2::Transform(v, rotationMatrix));
    }

    // TODO 3.3 (~1 linha): Utilize a função SDL_SetRenderDrawColor para alterar a cor de desenho para branco.
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // TODO 3.4 (~1 linha): Chame a função DrawPolygon para desenhar o conjunto de vértices transformado.
    // DrawPolygon(renderer, vertices);

    // TODO 3.5 (~4 linhas): Utilize a função DrawCircle para desenhar o círculo de colisão desse objeto.
    //  Antes de desenhar, altere a cor para verde com a função SDL_SetRenderDrawColor. Esse trecho
    //  de código será útil para debugar a detecção de colisão.
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    if (mOwner->GetComponent<CircleColliderComponent>() != nullptr) {
        // DrawCircle(renderer, Vector2::Zero, mOwner->GetComponent<CircleColliderComponent>()->GetRadius(), 30);
    }

    // Draw HitBoxes
    // SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    if (mOwner->GetComponent<AABBComponent>() != nullptr && mOwner->GetComponent<AABBComponent>()->IsActive()) {
        SDL_Color color = mOwner->GetComponent<AABBComponent>()->GetColor();
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        DrawAABB(renderer, mOwner->GetComponent<AABBComponent>()->GetMin(), mOwner->GetComponent<AABBComponent>()->GetMax());
    }
}