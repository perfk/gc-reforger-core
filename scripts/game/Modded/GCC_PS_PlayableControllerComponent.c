modded class PS_PlayableControllerComponent
{
	override void SwitchToObserver(IEntity from)
	{
		if (m_eCamera) return;
		GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.SpectatorMenu);
		PlayerController thisPlayerController = PlayerController.Cast(GetOwner());
		IEntity entity = thisPlayerController.GetControlledEntity();
		EntitySpawnParams params = new EntitySpawnParams();
		if (from) from.GetTransform(params.Transform);
		MoveToVoNRoom(thisPlayerController.GetPlayerId(), "", "");
		Resource resource = Resource.Load("{6EAA30EF620F4A2E}Prefabs/Editor/Camera/ManualCameraSpectator.et");
		m_eCamera = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
		
		if (lastCameraTransform[3][1] < 10000 && lastCameraTransform[3][1] > 0)
		{
			m_eCamera.SetTransform(lastCameraTransform);
			lastCameraTransform[3][1] = 10000;
		} else if (m_vObserverPosition != "0 0 0") { 
			m_eCamera.SetOrigin(m_vObserverPosition);
			m_vObserverPosition = "0 0 0";
		} else {
			SCR_MapEntity mapEntity = SCR_MapEntity.GetMapInstance();
			m_eCamera.SetOrigin(mapEntity.Size() / 2.0 + vector.Up * 100);
		}
		GetGame().GetCameraManager().SetCamera(CameraBase.Cast(m_eCamera));
		GetGame().GetCallqueue().CallLater(UpdateSpectator, 100, true);
	}
	
	override void SwitchFromObserver()
	{
		if (!m_eCamera) return;
		GetGame().GetMenuManager().CloseMenuByPreset(ChimeraMenuPreset.SpectatorMenu);
		SCR_EntityHelper.DeleteEntityAndChildren(m_eCamera);
		m_eCamera = null;
		GetGame().GetCallqueue().Remove(UpdateSpectator);
	}
	
	void UpdateSpectator()
	{
		PlayerController playerController = GetGame().GetPlayerController();
		PS_PlayableControllerComponent playableController = PS_PlayableControllerComponent.Cast(playerController.FindComponent(PS_PlayableControllerComponent));
		Print("GRAY.UpdateSpectator VoNPosition = " + playableController.VoNPosition);
		playableController.VoNPosition = m_eCamera.GetOrigin();
	}
}