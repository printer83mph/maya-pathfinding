#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

// A perspective projection camera
// Receives its eye position and reference point from the scene XML file
class Camera
{
public:
  Camera();
  Camera(unsigned int w, unsigned int h);
  Camera(unsigned int w, unsigned int h, float rotY, float rotX, float distance,
         const glm::vec3& worldUp);
  Camera(const Camera& c);

  float fovy;
  unsigned int width, height; // Screen dimensions
  float near_clip;            // Near clip plane distance
  float far_clip;             // Far clip plane distance

  // camera rotation!!
  float rotY, rotX, zoom;

  // Computed attributes
  float aspect;

  // The position of the camera in world space
  glm::vec3 eye,
      // The point in world space towards which the camera is pointing
      ref,
      // The normalized vector from eye to ref. Is also known as the camera's
      // "forward" vector.
      look,
      // The normalized vector pointing upwards IN CAMERA SPACE. This vector is
      // perpendicular to LOOK and RIGHT.
      up,
      // The normalized vector pointing rightwards IN CAMERA SPACE. It is
      // perpendicular to UP and LOOK.
      right,
      // The normalized vector pointing upwards IN WORLD SPACE. This is
      // primarily used for computing the camera's initial UP vector.
      world_up,
      // Represents the vertical component of the plane of the viewing frustum
      // that passes through the camera's reference point. Used in
      // Camera::Raycast.
      V,
      // Represents the horizontal component of the plane of the viewing frustum
      // that passes through the camera's reference point. Used in
      // Camera::Raycast.
      H;

  glm::mat4 getViewProj();

  void RecomputePosition();
  void RecomputeAttributes();

  void RotateAboutUp(float deg);
  void RotateAboutRight(float deg);

  void TranslateAlongLook(float amt, bool withRef);
  void ScaleZoom(float ratio);
  void TranslateAlongRight(float amt);
  void TranslateAlongUp(float amt);
};