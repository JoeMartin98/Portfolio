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

#pragma once

#include "Sample.h"

namespace Urho3D
{
	class Node;
	class Scene;
}

class Character;
class Boid;
class BoidSet;
class Touch;

/// Moving character example.
/// This sample demonstrates:
///     - Controlling a humanoid character through physics
///     - Driving animations using the AnimationController component
///     - Manual control of a bone scene node
///     - Implementing 1st and 3rd person cameras, using raycasts to avoid the 3rd person camera clipping into scenery
///     - Defining attributes of a custom component so that it can be saved and loaded
///     - Using touch inputs/gyroscope for iOS/Android (implemented through an external file)
class CharacterDemo : public Sample
{
	URHO3D_OBJECT(CharacterDemo, Sample);
	static const unsigned short SERVER_PORT = 2345;

public:
	/// Construct.
	CharacterDemo(Context* context);
	/// Destruct.
	~CharacterDemo();

	/// Setup after engine initialization and before running the main loop.
	virtual void Start();

	//BoidSet boidSet;

	Node*CreateControllableObject();
	Node*Shoot();
	// Server: Create a controllable ball
	unsigned clientObjectID_ = 0;
	// Client: ID of own object
	HashMap<Connection*, WeakPtr<Node> > serverObjects_;
	// Server Client/Object HashMap
	// Handle remote event from server to Client to share controlled object node ID.
	void HandleServerToClientObjectID(StringHash eventType, VariantMap& eventData);
	// Handle remote event, client tells server that client is ready to start game
	void HandleClientToServerReadyToStart(StringHash eventType, VariantMap& eventData);
	void HandleClientStartGame(StringHash eventType, VariantMap& eventData);

protected:
	/// Return XML patch instructions for screen joystick layout for a specific sample app, if any.
	virtual String GetScreenJoystickPatchString() const {
		return
			"<patch>"
			"    <add sel=\"/element\">"
			"        <element type=\"Button\">"
			"            <attribute name=\"Name\" value=\"Button3\" />"
			"            <attribute name=\"Position\" value=\"-120 -120\" />"
			"            <attribute name=\"Size\" value=\"96 96\" />"
			"            <attribute name=\"Horiz Alignment\" value=\"Right\" />"
			"            <attribute name=\"Vert Alignment\" value=\"Bottom\" />"
			"            <attribute name=\"Texture\" value=\"Texture2D;Textures/TouchInput.png\" />"
			"            <attribute name=\"Image Rect\" value=\"96 0 192 96\" />"
			"            <attribute name=\"Hover Image Offset\" value=\"0 0\" />"
			"            <attribute name=\"Pressed Image Offset\" value=\"0 0\" />"
			"            <element type=\"Text\">"
			"                <attribute name=\"Name\" value=\"Label\" />"
			"                <attribute name=\"Horiz Alignment\" value=\"Center\" />"
			"                <attribute name=\"Vert Alignment\" value=\"Center\" />"
			"                <attribute name=\"Color\" value=\"0 0 0 1\" />"
			"                <attribute name=\"Text\" value=\"Gyroscope\" />"
			"            </element>"
			"            <element type=\"Text\">"
			"                <attribute name=\"Name\" value=\"KeyBinding\" />"
			"                <attribute name=\"Text\" value=\"G\" />"
			"            </element>"
			"        </element>"
			"    </add>"
			"    <remove sel=\"/element/element[./attribute[@name='Name' and @value='Button0']]/attribute[@name='Is Visible']\" />"
			"    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button0']]/element[./attribute[@name='Name' and @value='Label']]/attribute[@name='Text']/@value\">1st/3rd</replace>"
			"    <add sel=\"/element/element[./attribute[@name='Name' and @value='Button0']]\">"
			"        <element type=\"Text\">"
			"            <attribute name=\"Name\" value=\"KeyBinding\" />"
			"            <attribute name=\"Text\" value=\"F\" />"
			"        </element>"
			"    </add>"
			"    <remove sel=\"/element/element[./attribute[@name='Name' and @value='Button1']]/attribute[@name='Is Visible']\" />"
			"    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button1']]/element[./attribute[@name='Name' and @value='Label']]/attribute[@name='Text']/@value\">Jump</replace>"
			"    <add sel=\"/element/element[./attribute[@name='Name' and @value='Button1']]\">"
			"        <element type=\"Text\">"
			"            <attribute name=\"Name\" value=\"KeyBinding\" />"
			"            <attribute name=\"Text\" value=\"SPACE\" />"
			"        </element>"
			"    </add>"
			"</patch>";
	}

private:
	/// Create static scene content.
	void CreateScene();
	/// Create controllable character.
	void CreateCharacter();
	/// Construct an instruction text to the UI.
	void CreateInstructions();
	/// Subscribe to necessary events.
	void SubscribeToEvents();
	/// Handle application update. Set controls to character.
	void HandleUpdate(StringHash eventType, VariantMap& eventData);
	/// Handle application post-update. Update camera position after character has moved.
	void HandlePostUpdate(StringHash eventType, VariantMap& eventData);


	/// Touch utility object.
	SharedPtr<Touch> touch_;
	/// The controllable character component.
	WeakPtr<Character> character_;
	/// First person camera flag.
	bool firstPerson_;
	bool boidsActive = false;
	bool scoreActive = false;
	
	Text* scoreText;
	int count;
	//Time time;
	Button* QuitButton;
	Button* connectButton_;
	Button* startServerButton_;

	int startNum;
	int score;

	LineEdit* serverAddressLineEdit_;

	SharedPtr<Window>window_;
	bool menuVisible = true;
	bool menuVisible2 = false;
	bool menuVisible3 = false;
	bool cursorVisible = true;
	void CreateMainMenu();
	void UpdateScore();

	Node* bullet;
	
	void HandleQuit(StringHash eventType, VariantMap& eventData);
	void HandleConnect(StringHash eventType, VariantMap& eventData);
	void HandleDisconnect(StringHash eventType, VariantMap& eventData);
	void HandleStartServer(StringHash eventType, VariantMap& eventData);

	Button* startClientGameButton_;

	Button* CreateButton(const String& text, int pHeight, Urho3D::Window* whichWindow, const String& name);
	LineEdit* CreateLineEdit(const String& text, int pHeight, Urho3D::Window* whichWindow);

	//Server Workshop
	Controls FromClientToServerControls();
	void ProcessClientControls();
	void HandlePhysicsPreStep(StringHash eventType, VariantMap& eventData);
	void HandleClientFinishedLoading(StringHash eventType, VariantMap& eventData);
	void HandleCustomEventByOlivier(StringHash eventType, VariantMap& eventData);
	void CreateClientScene();
	void CreateServerScene();

	//A client is connecting to the server
	void HandleClientConnected(StringHash eventType, VariantMap& eventData);
	//A client is disconnecting from the server
	void HandleClientDisconnected(StringHash eventType, VariantMap& eventData);	

	void HandleCollision(StringHash eventType, VariantMap& eventData);
};