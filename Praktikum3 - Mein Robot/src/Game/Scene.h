#pragma once

#include "OpenGLWindow.h"
#include <ShaderProgram.h>
#include <memory>
#include <AssetManager.h>
#include "Framework/SceneElements/Transform.h"


class Scene
{
public:
	Scene(OpenGLWindow* window);
	~Scene();

	bool init();
	void shutdown();
	void render(float dt);
	void update(float dt);
	OpenGLWindow* getWindow();

	void onKey(Key key, Action action, Modifier modifier);
	void onMouseMove(MousePosition mouseposition);
	void onMouseButton(MouseButton button, Action action, Modifier modifier);
	void onMouseScroll(double xscroll, double yscroll);
	void onFrameBufferResize(int width, int height);


private:
	OpenGLWindow* m_window;
	AssetManager m_assets;
    ShaderProgram* m_shader;
    GLuint vaoID, vboID , iboID, count;
    int cubeIndSize;
    Transform *cubTrans;

    // Körper
    Transform* m_head ,* m_body, *m_arm_R_top, *m_arm_L_top, *m_arm_R_bot, *m_arm_L_bot, *m_leg_R,*m_leg_L;
    float armRotationAngle = 0.0f;


    glm::vec3 cameraPosition = glm::vec3(0.0, 0.0, 3.0); //Position der Kamera, hier wird der Roboter näher/ weiter gezeigt 3 units abovve the  origin alongside the z axis
    glm::vec3 cameraTarget = glm::vec3(0.0, 0.0, 0.0); //Blickpunkt der Kamera zum Ursprung center of the scene
    glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0); //Up-Vektor der Kamera

    glm::mat4 viewMatrix;

    // Definiere Projektionsparameter
    float fieldOfView; //Öffnungswinkel der Kamera
    float aspectRatio; //Seitenverhältnis
    float nearPlane; //Entfernung zur nahen Clipping-Ebene
    float farPlane; //Entfernung zur fernen Clipping-Ebene


    glm::mat4 projectionMatrix;





};

