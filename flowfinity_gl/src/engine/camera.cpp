#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include <math.h>

Camera::Camera() : Camera(400, 400)
{
  look = glm::vec3(0, 0, -1);
  up = glm::vec3(0, 1, 0);
  right = glm::vec3(1, 0, 0);
}

Camera::Camera(unsigned int w, unsigned int h) : Camera(w, h, 0.f, 0.f, 10.f, glm::vec3(0, 1, 0)) {}

Camera::Camera(unsigned int w, unsigned int h, float ry, float rx, float zoom,
               const glm::vec3& worldUp)
    : fovy(45), width(w), height(h), near_clip(0.1f), far_clip(1000), rotY(ry), rotX(rx),
      zoom(zoom), world_up(worldUp)
{
  RecomputeAttributes();
}

Camera::Camera(const Camera& c)
    : fovy(c.fovy), width(c.width), height(c.height), near_clip(c.near_clip), far_clip(c.far_clip),
      rotY(c.rotY), rotX(c.rotX), zoom(c.zoom), aspect(c.aspect), eye(c.eye), ref(c.ref),
      look(c.look), up(c.up), right(c.right), world_up(c.world_up), V(c.V), H(c.H)
{
}

void Camera::RecomputeAttributes()
{
  eye = ref + glm::vec3(glm::rotate(glm::mat4(1), glm::radians(rotY), world_up) *
                        glm::rotate(glm::mat4(1), glm::radians(rotX), glm::vec3(1, 0, 0)) *
                        glm::vec4(0, 0, zoom, 0));

  look = glm::normalize(ref - eye);
  right = glm::normalize(glm::cross(look, world_up));
  up = glm::cross(right, look);

  float tan_fovy = tan(glm::radians(fovy / 2));
  float len = glm::length(ref - eye);
  aspect = width / static_cast<float>(height);
  V = up * len * tan_fovy;
  H = right * len * aspect * tan_fovy;
}

glm::mat4 Camera::getViewProj()
{
  return glm::perspective(glm::radians(fovy), width / (float)height, near_clip, far_clip) *
         glm::lookAt(eye, ref, up);
}

void Camera::RotateAboutUp(float deg) { rotY = fmod((rotY + deg), 360.f); }
void Camera::RotateAboutRight(float deg) { rotX = glm::clamp(rotX + deg, -89.f, 89.f); }

// TODO if i care enough: limit translation forward
void Camera::TranslateAlongLook(float amt, bool withRef)
{
  if (withRef) {
    glm::vec3 translation = look * amt;
    eye += translation;
    ref += translation;
  } else {
    zoom = glm::max(zoom - amt, 0.f);
  }
}

void Camera::ScaleZoom(float ratio) { zoom = glm::clamp(zoom * ratio, 0.f, 500.f); }

void Camera::TranslateAlongRight(float amt)
{
  glm::vec3 translation = right * amt;
  eye += translation;
  ref += translation;
}
void Camera::TranslateAlongUp(float amt)
{
  glm::vec3 translation = up * amt;
  eye += translation;
  ref += translation;
}