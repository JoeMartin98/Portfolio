//
// Copyright (c) 2008-2016 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/ProcessUtils.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Input/Controls.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include<Urho3D/UI/LineEdit.h>
#include<Urho3D/UI/Button.h>
#include<Urho3D/UI/UIEvents.h>
#include<Urho3D/UI/Window.h>
#include<Urho3D/UI/CheckBox.h>
#include<Urho3D/core/Timer.h>

#include <Urho3D/Network/Connection.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>

#include <Urho3D/Physics/PhysicsEvents.h>

#include "Character.h"
#include "CharacterDemo.h"
#include "Touch.h"
#include "boids.h"

#include <Urho3D/DebugNew.h>
#include <Urho3D/IO/Log.h>

static const StringHash E_CLIENTCUSTOMEVENTBYOLIVIER("ClientCustomEventByOlivier");
URHO3D_DEFINE_APPLICATION_MAIN(CharacterDemo)
BoidSet* myBoidSet = new BoidSet;

// Custom remote event we use to tell the client which object they control
static const StringHash E_CLIENTOBJECTAUTHORITY("ClientObjectAuthority");
// Identifier for the node ID parameter in the event data
static const StringHash PLAYER_ID("IDENTITY");
// Custom event on server, client has pressed button that it wants to start game
static const StringHash E_CLIENTISREADY("ClientReadyToStart");

//static const StringHash E_CLIENTHASFIRED("ClientHasFired");

CharacterDemo::CharacterDemo(Context* context) :
	Sample(context),
	firstPerson_(false)
{
	
}

CharacterDemo::~CharacterDemo()
{
}

void CharacterDemo::Start()
{
	// Execute base class startup
	Sample::Start();
	if (touchEnabled_)
		touch_ = new Touch(context_, TOUCH_SENSITIVITY);

	Timer();
	startNum = 0;
	CreateMainMenu();
	count = NumBoids;
	score = 0;
	CreateClientScene();
	SubscribeToEvents();
	Sample::InitMouseMode(MM_RELATIVE);
}

void CharacterDemo::SubscribeToEvents()
{
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(CharacterDemo, HandleUpdate));
	SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(CharacterDemo, HandlePostUpdate));

	SubscribeToEvent(connectButton_, E_RELEASED, URHO3D_HANDLER(CharacterDemo, HandleConnect));
	SubscribeToEvent(startServerButton_, E_RELEASED, URHO3D_HANDLER(CharacterDemo, HandleStartServer));
	SubscribeToEvent(QuitButton, E_RELEASED, URHO3D_HANDLER(CharacterDemo, HandleQuit));
	SubscribeToEvent(startClientGameButton_, E_RELEASED, URHO3D_HANDLER(CharacterDemo, HandleClientStartGame));

	SubscribeToEvent(E_NODECOLLISION, URHO3D_HANDLER(CharacterDemo, HandleCollision));

	SubscribeToEvent(E_CLIENTCONNECTED, URHO3D_HANDLER(CharacterDemo, HandleClientConnected));
	SubscribeToEvent(E_CLIENTDISCONNECTED, URHO3D_HANDLER(CharacterDemo, HandleClientDisconnected));
	
	SubscribeToEvent(E_PHYSICSPRESTEP, URHO3D_HANDLER(CharacterDemo, HandlePhysicsPreStep));
	SubscribeToEvent(E_CLIENTSCENELOADED, URHO3D_HANDLER(CharacterDemo, HandleClientFinishedLoading));

	SubscribeToEvent(E_CLIENTCUSTOMEVENTBYOLIVIER, URHO3D_HANDLER(CharacterDemo, HandleCustomEventByOlivier));
	GetSubsystem<Network>()->RegisterRemoteEvent(E_CLIENTCUSTOMEVENTBYOLIVIER);

	SubscribeToEvent(E_CLIENTISREADY, URHO3D_HANDLER(CharacterDemo,HandleClientToServerReadyToStart));
	GetSubsystem<Network>()->RegisterRemoteEvent(E_CLIENTISREADY);
	SubscribeToEvent(E_CLIENTOBJECTAUTHORITY, URHO3D_HANDLER(CharacterDemo,HandleServerToClientObjectID));
	GetSubsystem<Network>()->RegisterRemoteEvent(E_CLIENTOBJECTAUTHORITY);

	UnsubscribeFromEvent(E_SCENEUPDATE);
}

void CharacterDemo::CreateMainMenu()
{
	//Set the mouse mode to use in the sample
	InitMouseMode(MM_RELATIVE);

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	UI* ui = GetSubsystem<UI>();
	UIElement* root = ui->GetRoot();
	XMLFile* uiStyle = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
	root->SetDefaultStyle(uiStyle);//need to set default ui style

	SharedPtr<Cursor> cursor(new Cursor(context_));
	cursor->SetStyleAuto(uiStyle);
	ui->SetCursor(cursor);

	window_ = new Window(context_);
	root->AddChild(window_);
	
	window_->SetMinWidth(384);
	window_->SetLayout(LM_VERTICAL, 6, IntRect(6, 6, 6, 6));
	window_->SetAlignment(HA_CENTER, VA_CENTER);
	window_->SetName("Window");
	window_->SetStyleAuto();
	
	Font* font = cache->GetResource<Font>("Fonts/Anonymous Pro.ttf");

	startServerButton_ = CreateButton("Start Server", 24, window_, "startServerButton");
	connectButton_ = CreateButton("Join Server", 24, window_, "connectButton");
	serverAddressLineEdit_ = CreateLineEdit("", 24, window_);
	QuitButton = CreateButton("Exit", 24, window_, "QuitButton");
	startClientGameButton_ = CreateButton("Client: Start Game", 24, window_, "startClientGameButton");
	
	window_->AddChild(connectButton_);
	window_->AddChild(serverAddressLineEdit_);
	window_->AddChild(startServerButton_);
	window_->AddChild(startClientGameButton_);
	window_->AddChild(QuitButton);
}

Button* CharacterDemo::CreateButton(const String& text, int pHeight, Urho3D::Window* whichWindow, const String& name)
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	Font* font = cache->GetResource<Font>("Fonts/Anonymous Pro.ttf");
	Button* button = whichWindow->CreateChild<Button>();
	button->SetMinHeight(pHeight);
	button->SetStyleAuto();
	button->SetName(name);
	Text* buttonText = button->CreateChild<Text>();
	buttonText->SetFont(font, 12);
	buttonText->SetAlignment(HA_CENTER, VA_CENTER);
	buttonText->SetText(text);
	return button;
}

LineEdit* CharacterDemo::CreateLineEdit(const String& text, int pHeight, Urho3D::Window* whichWindow)
{
	LineEdit* lineEdit = whichWindow->CreateChild<LineEdit>();
	lineEdit->SetMinHeight(pHeight);
	lineEdit->SetAlignment(HA_CENTER, VA_CENTER);
	lineEdit->SetText(text);
	lineEdit->SetStyleAuto();
	return lineEdit;
}

void CharacterDemo::CreateClientScene()
{
	//ACCESS RESOURCES
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	scene_ = new Scene(context_);
	//SUBSYSTEM COMPONENTS
	scene_->CreateComponent<Octree>(LOCAL);
	scene_->CreateComponent<PhysicsWorld>(LOCAL);

	//CAMERA
	cameraNode_ = new Node(context_);
	Camera* camera = cameraNode_->CreateComponent<Camera>(LOCAL);
	cameraNode_->SetPosition(Vector3(0.0f, 70.0f, -70.0f));
	camera->SetFarClip(300.0f);

	GetSubsystem<Renderer>()->SetViewport(0, new Viewport(context_, scene_, camera));

	//STATIC LIGHTING AND FOG
	Node* zoneNode = scene_->CreateChild("Zone", LOCAL);
	Zone* zone = zoneNode->CreateComponent<Zone>(LOCAL);
	zone->SetAmbientColor(Color(0.15f, 0.15f, 0.15f));
	zone->SetFogColor(Color(0.25f, 0.25f, 0.25f));
	zone->SetFogStart(100.0f);
	zone->SetFogEnd(300.0f);
	zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));

	//DIRECTIONAL LIGHTING
	Node* lightNode = scene_->CreateChild("DirectionalLight", LOCAL);
	lightNode->SetDirection(Vector3(0.3f, -0.5f, 0.425f));
	Light* light = lightNode->CreateComponent<Light>(LOCAL);
	light->SetLightType(LIGHT_DIRECTIONAL);
	light->SetCastShadows(true);
	light->SetShadowBias(BiasParameters(0.00025f, 0.5f));
	light->SetShadowCascade(CascadeParameters(10.0f, 50.0f, 200.0f, 0.0f, 0.8f));
	light->SetSpecularIntensity(0.5f);

	CreateInstructions();
}

void CharacterDemo::CreateServerScene()
{
	Log::WriteRaw("Game loading");
	//ACCESS RESOURCES
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	CreateInstructions();
	scene_ = new Scene(context_);
	//SUBSYSTEM COMPONENTS
	scene_->CreateComponent<Octree>(LOCAL);
	scene_->CreateComponent<PhysicsWorld>(LOCAL);

	//CAMERA
	cameraNode_ = new Node(context_);
	Camera* camera = cameraNode_->CreateComponent<Camera>(LOCAL);
	cameraNode_->SetPosition(Vector3(0.0f, 70.0f, -70.0f));
	camera->SetFarClip(3000.0f);

	GetSubsystem<Renderer>()->SetViewport(0, new Viewport(context_, scene_, camera));

	//STATIC LIGHTING AND FOG
	Node* zoneNode = scene_->CreateChild("Zone", LOCAL);
	Zone* zone = zoneNode->CreateComponent<Zone>(LOCAL);
	zone->SetAmbientColor(Color(0.15f, 0.15f, 0.15f));
	zone->SetFogColor(Color(0.25f, 0.25f, 0.25f));
	zone->SetFogStart(10.0f);
	zone->SetFogEnd(3000.0f);
	zone->SetBoundingBox(BoundingBox(-2000.0f, 2000.0f));

	//DIRECTIONAL LIGHTING
	Node* lightNode = scene_->CreateChild("DirectionalLight", LOCAL);
	lightNode->SetDirection(Vector3(0.3f, -0.5f, 0.425f));
	Light* light = lightNode->CreateComponent<Light>(LOCAL);
	light->SetLightType(LIGHT_DIRECTIONAL);
	light->SetCastShadows(true);
	light->SetShadowBias(BiasParameters(0.00025f, 0.5f));
	light->SetShadowCascade(CascadeParameters(10.0f, 50.0f, 200.0f, 0.0f, 0.8f));
	light->SetSpecularIntensity(0.5f);

	CreateInstructions();
	scoreActive = true;
	boidsActive = true;
	myBoidSet->Initialise(cache, scene_);
	Log::WriteRaw("Game loaded");
}

void CharacterDemo::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	using namespace Update;

	ResourceCache* cache = GetSubsystem<ResourceCache>();

	//FRAME TIME
	float timeStep = eventData[P_TIMESTEP].GetFloat();

	//FREEZE
	if (GetSubsystem<UI>()->GetFocusElement()) return;

	Input* input = GetSubsystem<Input>();

	//MOVEMENT SPEED
	const float MOVE_SPEED = 100.0f;
	//MOUSE SENSITIVITY
	const float MOUSE_SENSITIVITY = 0.1f;

	//ADJUST CAMERA YAW + PITCH
	IntVector2 mouseMove = input->GetMouseMove();
	yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
	pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
	pitch_ = Clamp(pitch_, -90.0f, 90.0f);

	//CAMERA ORIENTATION
	cameraNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));

	//MOVE CAMERA
	if (input->GetKeyDown(KEY_W)) cameraNode_->Translate(Vector3::FORWARD * MOVE_SPEED * timeStep);
	if (input->GetKeyDown(KEY_S)) cameraNode_->Translate(Vector3::BACK * MOVE_SPEED * timeStep);
	if (input->GetKeyDown(KEY_A)) cameraNode_->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
	if (input->GetKeyDown(KEY_D)) cameraNode_->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);

	//UPDATE BOIDS
	if (boidsActive)myBoidSet->Update(timeStep);

	if (input->GetKeyPress(KEY_M)) menuVisible = !menuVisible;
	if (input->GetKeyPress(KEY_Y)) CreateServerScene();
// Only move the camera if we have a controllable object  
	if (clientObjectID_)
	{
		Node* ballNode = this->scene_->GetNode(clientObjectID_);
		if (ballNode)
		{
			const float CAMERA_DISTANCE = 15.0f;
			cameraNode_->SetPosition(ballNode->GetPosition() + cameraNode_->GetRotation()* Vector3::BACK * CAMERA_DISTANCE);
		}
	}
	//if (scoreActive)
	//{
	//	score = 1;
	//	instructionText->SetText(
	//		"Time: " + score
	//	);
	//}
}

void CharacterDemo::HandlePostUpdate(StringHash eventType, VariantMap& eventData)
{
	UI* ui = GetSubsystem<UI>();
	Input* input = GetSubsystem<Input>();
	ui->GetCursor()->SetVisible(menuVisible);
	window_->SetVisible(menuVisible);
}

void CharacterDemo::HandleQuit(StringHash eventType, VariantMap& eventData)
{
	engine_->Exit();
}

void CharacterDemo::HandleConnect(StringHash eventType, VariantMap& eventData)
{
	Network* network = GetSubsystem<Network>();
	String address = serverAddressLineEdit_->GetText().Trimmed();
	if (address.Empty()) { address = "localhost"; }

	network->Connect(address, SERVER_PORT, scene_);
	menuVisible = !menuVisible;
}

void CharacterDemo::HandleDisconnect(StringHash eventType, VariantMap& eventData)
{
	Log::WriteRaw("HandleDisconnect has been pressed. \n");
	Network* network = GetSubsystem<Network>();
	Connection* serverConnection = network->GetServerConnection();
	// Running as Client
	if (serverConnection)
	{
		serverConnection->Disconnect();
		scene_->Clear(true, false);
		clientObjectID_ = 0;
	}
	// Running as a server, stop it
	else if (network->IsServerRunning())
	{
		network->StopServer();
		scene_->Clear(true, false);
	}
}

void CharacterDemo::HandleStartServer(StringHash eventType, VariantMap& eventData)
{
	Log::WriteRaw("(HandleStartServer called) Server is started!");
	Network* network = GetSubsystem<Network>();
	network->StartServer(SERVER_PORT);
	CreateServerScene();
	menuVisible = !menuVisible;	
}

void CharacterDemo::HandlePhysicsPreStep(StringHash eventType, VariantMap & eventData)
{
	Network* network = GetSubsystem<Network>();
	Connection* serverConnection = network->GetServerConnection();
	// Client: collect controls
	if (serverConnection)
	{
		serverConnection->SetPosition(cameraNode_->GetPosition()); // send camera position too
		serverConnection->SetControls(FromClientToServerControls()); // send controls to server
		//Client can get the server
		Connection* serverConnection = network->GetServerConnection();
		VariantMap remoteEventData;
		remoteEventData["aValueRemoteValue"] = 0;
		serverConnection->SendRemoteEvent(E_CLIENTCUSTOMEVENTBYOLIVIER, true, remoteEventData);
	}
	// Server: Read Controls, Apply them if needed
	else if (network->IsServerRunning())
	{
		ProcessClientControls(); // take data from clients, process it
	}
}

Controls CharacterDemo::FromClientToServerControls()
{
	Input* input = GetSubsystem<Input>();
	Controls controls;
	//Check which button has been pressed, keep track
	controls.Set(CTRL_FORWARD, input->GetKeyDown(KEY_W));
	controls.Set(CTRL_BACK, input->GetKeyDown(KEY_S));
	controls.Set(CTRL_LEFT, input->GetKeyDown(KEY_A));
	controls.Set(CTRL_RIGHT, input->GetKeyDown(KEY_D));
	controls.Set(CTRL_JUMP, input->GetKeyDown(KEY_SPACE));
	controls.Set(14, input->GetKeyDown(KEY_SHIFT));
	controls.Set(1024, input->GetKeyDown(KEY_E));
	// mouse yaw to server
	controls.yaw_ = yaw_;
	return controls;
}

void CharacterDemo::ProcessClientControls()
{
	Input* input = GetSubsystem<Input>();
	Network* network = GetSubsystem<Network>();
	const Vector<SharedPtr<Connection> >& connections = network->GetClientConnections();
	//Server: go through every client connected
	for (unsigned i = 0; i < connections.Size(); ++i)
	{
		Connection* connection = connections[i];
		// Get the object this connection is controlling
		Node* ballNode = serverObjects_[connection];
		// Client has no item connected
		if (!ballNode) continue;
		RigidBody* body = ballNode->GetComponent<RigidBody>();
		// Get the last controls sent by the client
		const Controls& controls = connection->GetControls();
		// Torque is relative to the forward vector
		Quaternion rotation(0.0f, controls.yaw_, 0.0f);
		const float MOVE_TORQUE = 5.0f;
		Vector3 vn = body->GetLinearVelocity().Normalized();
		Vector3 cp = -vn.CrossProduct(Vector3(0.0f, 1.0f, 0.0f));
		float dp = cp.DotProduct(vn);
		body->SetRotation(Quaternion(Acos(dp), cp));
		if (controls.buttons_& CTRL_FORWARD)
			body->ApplyForce(rotation * Vector3::FORWARD * MOVE_TORQUE);
		if (controls.buttons_& CTRL_BACK)
			body->ApplyForce(rotation * Vector3::BACK * MOVE_TORQUE);
		if (controls.buttons_& CTRL_LEFT)
			body->ApplyForce(rotation * Vector3::LEFT * MOVE_TORQUE);
		if (controls.buttons_& CTRL_RIGHT)
			body->ApplyForce(rotation * Vector3::RIGHT * MOVE_TORQUE);
		if (controls.buttons_& CTRL_JUMP)
			body->ApplyForce(rotation * Vector3::UP * MOVE_TORQUE);
		if (controls.buttons_& 14)
			body->ApplyForce(rotation * Vector3::DOWN * MOVE_TORQUE);
		if (controls.buttons_ & 1024)
		{
			Log::WriteRaw("E PRESSED \n");
			Node* bulletFired = serverObjects_[connection + 1];
			bulletFired->SetPosition(ballNode->GetPosition() + Vector3(1.5f, 1.5f, 1.5f));
			RigidBody* bulletBody = bulletFired->GetComponent<RigidBody>();
			bulletBody->SetLinearVelocity(rotation * Vector3(0.0f, 0.25f, 1.0f) * 7.0f);
		}	
	}
}

void CharacterDemo::HandleClientFinishedLoading(StringHash eventType, VariantMap & eventData)
{
	Log::WriteRaw("Client has finished loading up the scene from the server \n");
}

void CharacterDemo::HandleCustomEventByOlivier(StringHash eventType, VariantMap & eventData)
{
	int exampleValue = eventData["aValueRemoteValue"].GetUInt();
	printf("This is a custom event by Olivier - passed Value - %i \n", exampleValue);
}

void CharacterDemo::HandleClientConnected(StringHash eventType, VariantMap& eventData)
{
	Log::WriteRaw("(HandleClientConnected) A client has connected!");
	using namespace ClientConnected;
	// When a client connects, assign to a scene
	Connection* newConnection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
	newConnection->SetScene(scene_);

	VariantMap remoteEventData;
	remoteEventData["aValueRemoteValue"] = 0;
	newConnection->SendRemoteEvent(E_CLIENTCUSTOMEVENTBYOLIVIER, true, remoteEventData);
}

void CharacterDemo::HandleClientDisconnected(StringHash eventType, VariantMap & eventData)
{
}

void CharacterDemo::HandleClientStartGame(StringHash eventType, VariantMap & eventData)
{
	menuVisible = !menuVisible;
	Log::WriteRaw("Client has pressed START GAME \n");
	if (clientObjectID_ == 0) // Client is still observer
	{
		Network* network = GetSubsystem<Network>();
		Connection* serverConnection = network->GetServerConnection();
		if (serverConnection)
		{
			VariantMap remoteEventData;
			remoteEventData[PLAYER_ID] = 0;
			serverConnection->SendRemoteEvent(E_CLIENTISREADY, true, remoteEventData);
		}
	}
}

Node* CharacterDemo::CreateControllableObject()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	// Create the scene node & visual representation. This will be a replicated object
	Node* ballNode = scene_->CreateChild("AClientBall");
	ballNode->SetPosition(Vector3(0.0f, 5.0f, 0.0f));
	ballNode->SetScale(2.5f);
	StaticModel* ballObject = ballNode->CreateComponent<StaticModel>();
	ballObject->SetModel(cache->GetResource<Model>("Models/Cone.mdl"));
	ballObject->SetMaterial(cache->GetResource<Material>("Materials/StoneSmall.xml"));
	// Create the physics components
	RigidBody* body = ballNode->CreateComponent<RigidBody>();
	body->SetMass(1.0f);
	body->SetUseGravity(false);
	// motion damping so that the ball can not accelerate limitlessly
	body->SetLinearDamping(0.25f);
	body->SetAngularDamping(0.25f);
	CollisionShape* shape = ballNode->CreateComponent<CollisionShape>();
	shape->SetSphere(1.0f);
	return ballNode;
}

void CharacterDemo::HandleClientToServerReadyToStart(StringHash eventType, VariantMap & eventData)
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	// Construct new Text object, set string to display and font to use
	
	printf("Event sent by the Client and running on Server: Client is ready to start the game \n");
	using namespace ClientConnected;
	Connection* newConnection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
	// Create a controllable object for that client 
	Node* newObject = CreateControllableObject();
	bullet = Shoot();
	serverObjects_[newConnection] = newObject;
	serverObjects_[newConnection + 1] = bullet;
	// Finally send the object's node ID using a remote event
	VariantMap remoteEventData;
	remoteEventData[PLAYER_ID] = newObject->GetID();
 	newConnection->SendRemoteEvent
(E_CLIENTOBJECTAUTHORITY, true, remoteEventData);
}

void CharacterDemo::HandleServerToClientObjectID(StringHash eventType, VariantMap & eventData)
{
	clientObjectID_ = eventData[PLAYER_ID].GetUInt();
	printf("Client ID : %i \n", clientObjectID_);
}

Node* CharacterDemo::Shoot()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	//CREATES THE BULLET
	// Create a smaller box at camera position
	Node* boxNode = scene_->CreateChild("SmallBox");
	Node* ballNode = scene_->GetChild("AClientBall");
	boxNode->SetPosition(ballNode->GetPosition() + Vector3(0.5f, 0.5f, 0.5f));
	boxNode->SetRotation(cameraNode_->GetRotation());
	boxNode->SetScale(0.1f);
	StaticModel* boxObject = boxNode->CreateComponent<StaticModel>();
	boxObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
	boxObject->SetMaterial(cache->GetResource<Material>("Materials/StoneEnvMapSmall.xml"));
	boxObject->SetCastShadows(true);

	// Create physics components, use a smaller mass also
	RigidBody* body = boxNode->CreateComponent<RigidBody>();
	body->SetMass(0.1f);
	body->SetFriction(0.75f);
	body->SetUseGravity(false);
	CollisionShape* shape = boxNode->CreateComponent<CollisionShape>();
	shape->SetBox(Vector3::ONE);

	const float OBJECT_VELOCITY = 7.0f;

	// Set initial velocity for the RigidBody based on camera forward vector. Add also a slight up component
	// to overcome gravity better
	/*body->SetLinearVelocity(cameraNode_->GetRotation() * Vector3(0.0f, 0.25f, 1.0f) * OBJECT_VELOCITY)*/;

	return boxNode;
}

void CharacterDemo::CreateInstructions()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	UI* ui = GetSubsystem<UI>();
	
	// Construct new Text object, set string to display and font to use
	scoreText = ui->GetRoot()->CreateChild<Text>("score");
	scoreText->SetText(
		"Boids left : "
	);
	scoreText->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 15);
	// The text has multiple rows. Center them in relation to each other
	scoreText->SetTextAlignment(HA_CENTER);
	// Position the text relative to the screen center
	scoreText->SetHorizontalAlignment(HA_LEFT);
	scoreText->SetVerticalAlignment(VA_TOP);
}

void CharacterDemo::UpdateScore()
{
	count--;
	
}

void CharacterDemo::HandleCollision(StringHash eventType, VariantMap & eventData)
{
	Log::WriteRaw("Collision detected!");
	Node* boidNode = static_cast<Node*>(eventData[NodeCollision::P_OTHERNODE].GetPtr());
	if (boidNode->GetName() == "Boid")
	{
		boidNode->SetEnabled(false);
		count--;
		UpdateScore();
	}
}