#include "Scene.h" // Include the scene header file for class definition.
#include <AssetManager.h> // Include to manage assets like shaders and textures.
#include "Cube.h" //Include for cube data (vertices, indices).

/*View-Matrix: Verändert die Position und Orientierung der gesamten Szene relativ zur Kamera, um zu bestimmen, was sichtbar ist. Es ist,
 * als ob man die Welt aus der Perspektive der Kamera betrachtet.
Projection-Matrix: Verwandelt die 3D-Szene, die durch die View-Matrix betrachtet wird, in eine 2D-Projektion für den Bildschirm.
 Sie bestimmt, wie Tiefen- und Perspektiveffekte gehandhabt werden, um eine realistischere Darstellung zu erzielen.
 */

Scene::Scene(OpenGLWindow * window) :
	m_window(window) // Initialize the member variable m_window with the passed window pointer.
{
	assert(window != nullptr); // Ensure the window pointer is not null, to avoid undefined behavior.
}

Scene::~Scene()
= default; // Default destructor, uses the compiler-generated destructor.

bool Scene::init()
{
	try
	{
        // Load shaders and link them into a shader program.
		m_assets.addShaderProgram("shader", AssetManager::createShaderProgram("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl"));
		m_shader = m_assets.getShaderProgram("shader");
        m_shader->use(); // Activate the shader program to configure its uniforms before drawing.


        // generate and activate VAO //
        // es steht 1, weil es nur ein Array ist
        // Set up the Vertex Array Object (VAO) for managing vertex buffer and attribute states.
        glGenVertexArrays(1, &vaoID);
        glBindVertexArray(vaoID);

        // generate and activate VBO and upload data //
        // Set up the Vertex Buffer Object (VBO) and load the vertex data into it.
        glGenBuffers( 1, &vboID);
        glBindBuffer(GL_ARRAY_BUFFER, vboID);
        glBufferData(GL_ARRAY_BUFFER, sizeof (cubeVert), &cubeVert, GL_STATIC_DRAW);


        // describe VBO in the VAO //
        // Configure the vertex attributes in the VAO.
        GLint positionAttribute = 0;
        GLint colorAttribute = 1;

        glEnableVertexAttribArray(positionAttribute);
        // 2 weil es nur 2 D ist und 5, weil die werte von Vertex in 2 D, also 2 für x,y und 3 für Farben
        // False, weil die Daten nicht normalisiert werden soll
        // beginnen die Positionen am Anfang des Buffer
        glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 0);


        // Hier wird das Attribut mit Index 1 aktiviert, was bedeutet, dass die Vertex-Shader auf diese Farbwerte
        // zugreifen können
        glEnableVertexAttribArray(colorAttribute);

        glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3* sizeof(float)));


        // Erzeugen IBO
        // um Indices zu speichern, die auf die Positionen von Eckpunkten in VBO zu zeigen
        // Set up the Index Buffer Object (IBO) and load the index data into it.
        glGenBuffers(1, &iboID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeInd), &cubeInd, GL_STATIC_DRAW);


        // Configure OpenGL to cull back faces and enable depth testing for correct 3D rendering.
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
         //setzt die Hintergrundfarbe auf Schwarz, die beim Löschen des Farbpuffers verwendet wird
         // wird  einmal vor dem Rendern der Szene aufgerufen

       // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


        //Depth test
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glClearDepth(1.0);

        // Definiere Kameraparameter
	    /*View Matrix: Transforms world coordinates to camera (view) coordinates.
	     *This matrix places the camera in the world and orients it correctly.
	     *It moves and rotates the entire scene so that the camera appears to be at the origin looking down the negative z-axis.
	     *
	     *Projection Matrix: Transforms camera coordinates to normalized device coordinates (NDC).
	     *This matrix maps the 3D scene into a 2D plane while applying perspective (for a realistic 3D effect).
	     *It defines the field of view, aspect ratio, and the near and far clipping planes.*/


        // Berechne View-Matrix used to convert coordinates from world space to camera space
	    // it transforms the scene so that the camera appears to be at the origin looking down the -ve Z-axis and makes it easier to transform the 3d scene onto the 2d screen
        // to position and orient the camera in the scene
        // Configure view and projection matrices for camera transformation and projection.
	    viewMatrix = glm::lookAt(cameraPosition, cameraTarget, cameraUp);

        // Lade die View-Matrix in den Shader
        m_shader->setUniform("view", viewMatrix, false);

        // Definiere Projektionsparameter
        fieldOfView = 45.0f;
        aspectRatio = 1.0f;
        nearPlane = 0.1f;
        farPlane = 80.0f;

        // Berechne Projektions-Matrix
        projectionMatrix = glm::perspective(glm::radians(fieldOfView), aspectRatio, nearPlane, farPlane);

        // Lade die Projektionsmatrix in den Shader false because we dont want the matrix to be transposed since in opne gl ever matrices are stored in column-measured
        m_shader->setUniform("projection", projectionMatrix, false);


        // Lichtquelle
        // Setup lighting in the scene.
        glm::vec3 lightPos = {0.6, 1, 0.65};
        glm::vec3 lightColor = {1, 1, 1};
        glm::vec3 ambientLight = {0.1, 0.1, 0.1};
        float pointLightIntensity = 20.0;

        // Material
     //   glm::vec3 matDiffuse = {0.1f, 0.8f, 0.0f}; //green
        glm::vec3 matDiffuse = {0.8f, 0.1f, 0.1f}; // More red
      //  glm::vec3 matDiffuse = {1.0f, 1.0f, 1.0f}; // White
        //glm::vec3 matDiffuse = {0.1f, 0.1f, 0.8f}; // More blue

        glm::vec3 emissive = {1.0f, 1.0f, 0.5f};
        glm::vec3 specular = {1, 1, 1};
        float shininess = 50.0;

        // Material
        glm::vec3 materialspekular = {1,1,1};
        float materialShiniees = 50;

        // Light properties
        // Setze Uniforms für die Beleuchtung
        m_shader->use();
        m_shader->setUniform("lightPos", lightPos);
        m_shader->setUniform("lightColor", lightColor);
        m_shader->setUniform("lightAmbient", ambientLight);
        m_shader->setUniform("LightIntensity", pointLightIntensity);

        // Setze Material Uniforms
        m_shader->setUniform("matDiffuse", matDiffuse);
        m_shader->setUniform("Emissive", emissive);
        m_shader->setUniform("specular", specular);
        m_shader->setUniform("shininess", shininess);



        std::cout << "Scene initialization done\n";
        return true;


	}
	catch (std::exception& ex)
	{
        // Handle any exceptions that may occur during initialization and provide a message
	    throw std::logic_error("Scene initialization failed:\n" + std::string(ex.what()) + "\n");
	}
}


// Clears the color and depth buffer in the GPU's framebuffer to prepare for new drawing.
// This is typically done at the start of a new frame to remove the previous frame's data.
void Scene::render(float dt)
{
    // Clear background color and depth buffer for depth test
    // wird dies am Anfang jedes Frames in der Render-Schleife aufgerufen, bevor neue Inhalte gerendert werden.
    // Es wird der Farbpuffer als auch der Tiefenpuffer gelöscht, um den Framebuffer für
    // die Darstellung neuer Inhalte vorzubereiten.
    //  am Anfang jedes Frames in der Render-Schleife aufgerufen, bevor neue Inhalte gerendert werden.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


    // activate VAO //
    // Activates the Vertex Array Object for drawing, which stores all the vertex attribute configurations.
    glBindVertexArray(vaoID);

    // Winkel und die Geschwindigkeit von Drehen setzen
    // Static variable to accumulate time changes and control rotation.
    static float delta = 0.5f;
    // Increase 'delta' by the frame time multiplied by 0.7, affecting the speed of rotation.
    delta += .7f* dt;

    // Calculate angle for rotation based on accumulated delta.
    float angels = delta;
    // Compute a simple animation effect using the sine of 'angels'.
    float animation = sin(angels);

    // Render den Roboter

    // Roboter Transformation- rotation
    cubTrans = new Transform;
   // cubTrans->setRotation(glm::vec3(0, 0, 0));
    // Rotate the transformation around the origin (0, 0, 0) by 'angels' degrees on the y-axis.
    cubTrans->rotateAroundPoint(glm::vec3 (0,0,0), glm::vec3 (0, angels, 0));
    // Update the shader uniform for rotation transformation.
    m_shader->setUniform("rotation", cubTrans->getMatrix(), false);
    // Set object color for the robot.
    m_shader->setUniform("objectColor", glm::vec3 (.65, 0.5, 0.8));
    // Pass the animation value to the shader.
    m_shader->setUniform("animationInput", animation);

//********************Kopf***********************
// Prepare head transformation, position, and scale.
    m_head = new Transform;
    m_head->translate(glm::vec3(0.0, 0.7, 0.0));
    m_head->scale(glm::vec3(0.2, 0.3, 0.2));
    m_shader->setUniform("model", m_head->getMatrix(), false);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //************Body**********************
    m_body = new Transform;
    m_body->translate(glm::vec3 (0, 0.1, 0));
    m_body->scale(glm::vec3(0.32, 0.8, 0.32));
    m_shader->setUniform("model", m_body->getMatrix(), false);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    //************* Rechts-Arm******************
    m_arm_R_top = new Transform;
    m_arm_R_bot = new Transform;

    armRotationAngle += 0.5f * dt;

    m_arm_R_top->rotateAroundPoint(glm::vec3(0.0, 0.1, 0.0), glm::vec3(-cos(angels) / 15));
    m_arm_R_top->translate(glm::vec3(-.25, .25, 0));
    m_arm_R_top->scale(glm::vec3(0.1, 0.35, 0.2));

    m_shader->setUniform("model", m_arm_R_top->getMatrix(), false);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

// unterarm rechts
    m_arm_R_bot->rotateAroundPoint(glm::vec3(0.0, 0.1, 0.0), glm::vec3(-cos(angels) / 15));
    m_arm_R_bot->translate(glm::vec3(-.25, -0.12, 0));
    m_arm_R_bot->scale(glm::vec3(0.1, 0.3, 0.2));

    m_shader->setUniform("model", m_arm_R_bot->getMatrix(), false);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    //***************Links-arm*************************

    m_arm_L_bot= new Transform;

    m_arm_L_top = new Transform;
// *************Oberlinks-arm****************


    m_arm_L_top->rotateAroundPoint(glm::vec3 (0.0, 0.1, 0.0), glm::vec3(sin(angels) / 15));
    m_arm_L_top->translate(glm::vec3(.25, .25, 0));
    m_arm_L_top->scale((glm::vec3(0.1, 0.35, 0.2)));
  //

    m_shader->setUniform("model", m_arm_L_top->getMatrix(), false);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //cubTrans->setRotation(glm::vec3(sin(angels) / 2, angels, 0.0));
    m_shader->setUniform("rotation", cubTrans->getMatrix(), false);

    //**************unterlinks************

    m_arm_L_bot->rotateAroundPoint(glm::vec3 (0.0, 0.1, 0.0), glm::vec3(sin(angels) / 15));
    m_arm_L_bot->translate(glm::vec3(.25, -.12, 0));
    m_arm_L_bot->scale((glm::vec3(0.1, 0.3, 0.2)));


    m_shader->setUniform("model", m_arm_L_bot->getMatrix(), false);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

//    //************* Rotation von unterlinks Arm************
   // cubTrans->setRotation(glm::vec3(sin(angels) / 2, angels, 0.0));
//    m_shader->setUniform("rotation", cubTrans->getMatrix(), false);


    //********************m_leg_R-links**********************
    m_leg_R = new Transform;
    m_leg_L = new Transform;

    m_leg_R->rotateAroundPoint(glm::vec3 (0, .1, 0), glm::vec3(sin(angels) / 16));
    m_leg_R->translate(glm::vec3(-0.1, -0.55, 0.0));
    m_leg_R->scale(glm::vec3 (0.12, 0.4, 0.12));

    m_shader->setUniform("model", m_leg_R->getMatrix(), false);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Rotation - Schwenken vom rechten Bein
    m_shader->setUniform("rotation", cubTrans->getMatrix(), false);
   // cubTrans->setRotation(glm::vec3(0.0, angels, 0.0));

    // bein links
    m_leg_L->rotateAroundPoint(glm::vec3 (0, .0, 0), glm::vec3(cos(angels) / 16));
    //cubTrans->setRotation(glm::vec3(sin(angels) / 4, angels, 0.0));
    m_leg_L->translate(glm::vec3(0.1, -0.55, 0.0));
    m_leg_L->scale(glm::vec3 (0.12, 0.4, 0.12));

    m_shader->setUniform("model", m_leg_L->getMatrix(), false);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Rotation - Schwenken vom linken Bein
   // cubTrans->setRotation(glm::vec3(0.0, angels, 0.0));


   //scene->processInput(Scene::m_window, dt);

    //  reset //
    // Unbind the VAO to avoid altering it after the render method is done.
    glBindVertexArray(0);


}


void Scene::update(float dt)
{

        // Geschwindigkeit von Lightposition
    // Definiere die anfängliche Position der Lichtquelle
    glm::vec3 lightPosition = {0.6, 0.5, 0.65};  //0.6  .5   0.65
    // Geschwindigkeit der Lichtbewegung, kann nach Bedarf angepasst werden.
    float lightSpeed = 5.0f;

/*
    // Überprüfe die Tastendrücke und aktualisiere die Position der Lichtquelle entsprechend.
    if (m_window->getInput().getKeyState(Key::Up) == KeyState::Pressed){
        std::cout << "Up key pressed" << std::endl;
        lightPosition.y += lightSpeed * dt; // Bewege das Licht nach oben.
        std::cout << "Delta time: " << dt << std::endl; // Zeige die verstrichene Zeit seit dem letzten Frame.
    }
    if (m_window->getInput().getKeyState(Key::Down)== KeyState::Pressed){
        std::cout << "Down key pressed" << std::endl;
        lightPosition.y -= lightSpeed * dt; // Bewege das Licht nach unten.
    }
    if (m_window->getInput().getKeyState(Key::Left)== KeyState::Pressed){
        std::cout << "Left key pressed" << std::endl;
        lightPosition.x -= lightSpeed * dt; // Bewege das Licht nach links.
    }
    if (m_window->getInput().getKeyState(Key::Right)== KeyState::Pressed){
        std::cout << "Right key pressed" << std::endl;
        lightPosition.x += lightSpeed * dt; // Bewege das Licht nach rechts.
    }
    if (m_window->getInput().getKeyState(Key::Comma)== KeyState::Pressed){
        std::cout << "Comma key pressed" << std::endl;
        lightPosition.z -= lightSpeed * dt; // Bewege das Licht zurück.
    }
    if (m_window->getInput().getKeyState(Key::Period)== KeyState::Pressed){
        std::cout << "Period key pressed" << std::endl;
        lightPosition.z += lightSpeed * dt; // Bewege das Licht nach vorne.
    }

// Aktualisiere die Uniform-Variable für die Position des Lichts im Shader.
m_shader->setUniform("lightPos", lightPosition);


    // Geschwindigkeit für die Kamerabewegung
    float cameraSpeed = 5.0f; // Kann angepasst werden, um die Bewegung zu beschleunigen oder zu verlangsamen.

    // Berechne Vektoren basierend auf der aktuellen Ausrichtung der Kamera, um die Bewegung zu steuern.
    glm::vec3 cameraFront = glm::normalize(cameraTarget - cameraPosition);// Vektor in die Blickrichtung der Kamera.
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp)); // Rechtsvektor relativ zur Kamera.
    glm::vec3 cameraUpward = glm::cross(cameraRight, cameraFront); // Aufwärtsvektor relativ zur Kamera.

    // Bewegung basierend auf der gedrückten Taste
    if (m_window->getInput().getKeyState(Key::R) == KeyState::Pressed) { //nach Rechts drehen
        cameraPosition -= cameraRight * cameraSpeed * dt;

    }
    if (m_window->getInput().getKeyState(Key::L) == KeyState::Pressed) { //nach Links drehen
        cameraPosition += cameraRight * cameraSpeed * dt;

    }
    if (m_window->getInput().getKeyState(Key::A) == KeyState::Pressed) { //auszoomen
        cameraPosition += cameraFront * cameraSpeed * dt;

    }
    if (m_window->getInput().getKeyState(Key::E) == KeyState::Pressed) { //einzoomen
        cameraPosition -= cameraFront * cameraSpeed * dt;

    }
    if (m_window->getInput().getKeyState(Key::U) == KeyState::Pressed) { //nach unten rotieren
        cameraPosition -= cameraUpward * cameraSpeed * dt;

    }
    if (m_window->getInput().getKeyState(Key::O) == KeyState::Pressed) { //nach oben rotieren
        cameraPosition += cameraUpward * cameraSpeed * dt;

    }
    viewMatrix = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
    m_shader->setUniform("view", viewMatrix, false);

    */
}

OpenGLWindow * Scene::getWindow()
{
	return m_window;
}

void Scene::onKey(Key key, Action action, Modifier modifier)
{

}

void Scene::onMouseMove(MousePosition mouseposition)
{

}

void Scene::onMouseButton(MouseButton button, Action action, Modifier modifier)
{

}

void Scene::onMouseScroll(double xscroll, double yscroll)
{

}

void Scene::onFrameBufferResize(int width, int height)
{

}
void Scene::shutdown()
{
    glDeleteBuffers(1, &vboID);
    glDeleteBuffers(1, &iboID);
    glDeleteVertexArrays(1, &vaoID);
}


