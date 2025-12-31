modded class SCR_VonDisplay
{
	override void OnReceive(int playerId, bool isSenderEditor, BaseTransceiver receiver, int frequency, float quality)
	{
		if(!frequency)
			return;
		
		super.OnReceive(playerId, isSenderEditor, receiver, frequency, quality);
	}
	
	override protected bool UpdateTransmission(TransmissionData data, BaseTransceiver radioTransceiver, int frequency, bool IsReceiving)
	{
		bool result = super.UpdateTransmission(data, radioTransceiver, frequency, IsReceiving);
		
		data.m_Widgets.m_wRole.SetVisible(false);
		data.m_Widgets.m_wSquadLeaderIcon.SetVisible(false);
		
		SCR_EditorManagerEntity editorManager = SCR_EditorManagerEntity.GetInstance();
		if(!editorManager || !editorManager.IsOpened())
			data.m_Widgets.m_wName.SetVisible(false);
		
		return result;
	}
}
