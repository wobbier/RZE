#include <StdAfx.h>
#include <ECS/Systems/FreeCameraSystem.h>

#include <algorithm>

#include <Brofiler/Brofiler.h>

#include <EngineApp.h>

#include <ECS/Components/CameraComponent.h>
#include <ECS/Components/NameComponent.h>
#include <ECS/Components/TransformComponent.h>

#include <Utils/Math/Math.h>

FreeCameraSystem::FreeCameraSystem(Apollo::EntityHandler* const entityHandler)
	: Apollo::EntitySystem(entityHandler)
{
}

FreeCameraSystem::~FreeCameraSystem()
{
}

void FreeCameraSystem::Initialize()
{
	InternalGetComponentFilter().AddFilterType<CameraComponent>();
	InternalGetComponentFilter().AddFilterType<TransformComponent>();

	RegisterComponentAddedNotifications();
}

void FreeCameraSystem::Update(const std::vector<Apollo::EntityID>& entities)
{
	BROFILER_CATEGORY("FreeCameraSystem::Update", Profiler::Color::Yellow);

	Apollo::EntityHandler& handler = InternalGetEntityHandler();
	for (auto& entity : entities)
	{
		CameraComponent* const camComp = handler.GetComponent<CameraComponent>(entity);
		AssertNotNull(camComp);

		if (camComp->bIsActiveCamera)
		{
			TransformComponent* const transfComp = handler.GetComponent<TransformComponent>(entity);
			AssertNotNull(transfComp);

			MouseInput(*camComp, *transfComp);
			KeyboardInput(*camComp, *transfComp);
		}
	}

}

void FreeCameraSystem::ShutDown()
{
}

void FreeCameraSystem::KeyboardInput(CameraComponent& camComp, TransformComponent& transfComp)
{
	InputHandler& inputHandler = RZE_Application::RZE().GetInputHandler();

	float dt = static_cast<float>(RZE_Application::RZE().GetDeltaTime());
	float speedDelta = mSpeed * dt;

	// TODO(Josh::The extra condition here for hold is because of the frame delay for ::Hold. Should fix eventually.)
	if (inputHandler.GetKeyboardState().GetButtonState(Win32KeyCode::Key_W) == EButtonState::ButtonState_Pressed
		|| inputHandler.GetKeyboardState().GetButtonState(Win32KeyCode::Key_W) == EButtonState::ButtonState_Hold)
	{
		transfComp.Position += camComp.Forward * speedDelta;
	}
	else if (inputHandler.GetKeyboardState().CurKeyStates[Win32KeyCode::Key_S])
	{
		transfComp.Position -= camComp.Forward * speedDelta;
	}
	else if (inputHandler.GetKeyboardState().CurKeyStates[Win32KeyCode::Key_A])
	{
		transfComp.Position -= camComp.Forward.Cross(camComp.UpDir).Normalize() * speedDelta;
	}
	else if (inputHandler.GetKeyboardState().CurKeyStates[Win32KeyCode::Key_D])
	{
		transfComp.Position += camComp.Forward.Cross(camComp.UpDir).Normalize() * speedDelta;
	}
	else if (inputHandler.GetKeyboardState().CurKeyStates[Win32KeyCode::Key_E])
	{
		transfComp.Position += camComp.Forward.Cross(camComp.UpDir).Cross(camComp.Forward) * speedDelta;
	}
	else if (inputHandler.GetKeyboardState().CurKeyStates[Win32KeyCode::Key_Q])
	{
		transfComp.Position -= camComp.Forward.Cross(camComp.UpDir).Cross(camComp.Forward) * speedDelta;
	}
	else if (inputHandler.GetKeyboardState().CurKeyStates[Win32KeyCode::Key_1])
	{
		// Focus object
	}

	if (inputHandler.GetKeyboardState().GetButtonState(Win32KeyCode::Space) == EButtonState::ButtonState_Pressed)
	{
		mSpeed = kFullSpeed / 8.0f;
	}
	else if (!inputHandler.GetKeyboardState().IsDownThisFrame(Win32KeyCode::Space))
	{
		mSpeed = kFullSpeed;
	}

	Int32 wheelVal = RZE_Application::RZE().GetInputHandler().GetMouseState().CurWheelVal;
	if (wheelVal != 0)
	{
		wheelVal = MathUtils::Clamp(wheelVal, -1, 1);
		transfComp.Position = transfComp.Position + (camComp.Forward * static_cast<float>(wheelVal)) * mWheelZoomSpeed;
	}
}

void FreeCameraSystem::MouseInput(CameraComponent& camComp, TransformComponent& transfComp)
{
	InputHandler& inputHandler = RZE_Application::RZE().GetInputHandler();

	const float deltaT = static_cast<float>(RZE_Application::RZE().GetDeltaTime());
	Vector3D curPos = inputHandler.GetMouseState().CurPosition;

	Int32 wheelVal = RZE_Application::RZE().GetInputHandler().GetMouseState().CurWheelVal;
	if (wheelVal != 0)
	{
		wheelVal = MathUtils::Clamp(wheelVal, -1, 1);
		transfComp.Position = transfComp.Position + (camComp.Forward * static_cast<float>(wheelVal)) * mWheelZoomSpeed;
	}

	if (RZE_Application::RZE().GetInputHandler().GetMouseState().GetButtonState(EMouseButton::MouseButton_Right) == EButtonState::ButtonState_Pressed)
	{
		Vector3D diff = curPos - mMousePrevPos;
		diff = diff * 0.1f; // #TODO(Josh) Move this to a better place (mouse sensitivity) -- config file
		mPitchYawRoll += diff;

		Vector3D lookDir;
		lookDir.SetX(std::cos(mPitchYawRoll.X() * MathUtils::ToRadians) * std::cos(mPitchYawRoll.Y() * MathUtils::ToRadians));
		lookDir.SetY(std::sin(mPitchYawRoll.Y() * MathUtils::ToRadians));
		lookDir.SetZ(std::sin(mPitchYawRoll.X() * MathUtils::ToRadians) * std::cos(mPitchYawRoll.Y() * MathUtils::ToRadians));

		lookDir.SetY(lookDir.Y() * -1);

		camComp.Forward = std::move(lookDir);
		camComp.Forward.Normalize();
	}

	mMousePrevPos = curPos;
}

void FreeCameraSystem::RegisterComponentAddedNotifications()
{
}

Vector3D FreeCameraSystem::ArcBallProjection(const Vector3D& vec)
{
	const Vector2D& winDims = RZE_Application::RZE().GetWindowSize();
	float x = 1.0f * (vec.X() / winDims.X() * 2.0f - 1.0f);
	float y = 1.0f * (vec.Y() / winDims.Y() * 2.0f - 1.0f);

	y = -y;

	Vector3D result(x, y, 0.0f);

	float sq = x * x + y * y;
	if (sq <= 1)
	{
		result.SetZ(sqrt(1 - sq));
	}
	else
	{
		result.Normalize();
	}

	return result;
}
