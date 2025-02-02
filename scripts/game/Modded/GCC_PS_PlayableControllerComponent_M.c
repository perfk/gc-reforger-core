modded class PS_PlayableControllerComponent
{
	protected bool m_isSpectating = false;
	
	static PS_PlayableControllerComponent GetPlayableController()
	{
		return PS_PlayableControllerComponent.Cast(GetGame().GetPlayerController().FindComponent(PS_PlayableControllerComponent));
	}
	
	bool IsSpectating()
	{
		return m_isSpectating;
	}
	
	override void SwitchToObserver(IEntity from)
	{
		super.SwitchToObserver(from);
		m_isSpectating = true;
	}
	
	override void SwitchFromObserver()
	{
		super.SwitchFromObserver();
		m_isSpectating = false;
	}
	
	override protected void EOnPostFixedFrame(IEntity owner, float timeSlice)
	{
		RplComponent rpl = RplComponent.Cast(owner.FindComponent(RplComponent));
		if (!rpl.IsOwner())
			return;
		
		// Lets fight with phisyc engine
		if (m_eInitialEntity)
		{
			if(m_isSpectating)
				m_eInitialEntity.SetOrigin(m_eCamera.GetOrigin());
			
			//m_eInitialEntity.SetOrigin(VoNPosition);
			// Who broke camera on map?
			CameraBase cameraBase = GetGame().GetCameraManager().CurrentCamera();
			if (cameraBase)
				cameraBase.ApplyTransform(timeSlice);
		} else {
			PlayerController thisPlayerController = PlayerController.Cast(GetOwner());
			IEntity entity = thisPlayerController.GetControlledEntity();
			if (entity)
			{
				PS_LobbyVoNComponent von = PS_LobbyVoNComponent.Cast(entity.FindComponent(PS_LobbyVoNComponent));
				if (von) m_eInitialEntity = entity;
			}
		}
	}
	
	void SetVoNPosition(vector position)
	{
		VoNPosition = position;
		m_eInitialEntity.SetOrigin(position);
	}
}

modded class PS_VoNRoomsManager
{
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	override void RPC_MoveToRoom(int playerId, int roomId, vector position)
	{
		super.RPC_MoveToRoom(playerId, roomId, position);
		
		PlayerController playerController = GetGame().GetPlayerManager().GetPlayerController(playerId);
		PS_PlayableControllerComponent playableController = PS_PlayableControllerComponent.Cast(playerController.FindComponent(PS_PlayableControllerComponent));
		playableController.SetVoNPosition(position);
	}
}
