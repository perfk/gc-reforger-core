modded class PS_BriefingMapMenu
{
	override void UpdateCycle()
	{
		super.UpdateCycle();
		
		Widget root = GetRootWidget();
		TextWidget timer = TextWidget.Cast(root.FindAnyWidget("FactionReadyTimer"));
		
		PS_GameModeCoop gm = PS_GameModeCoop.Cast(GetGame().GetGameMode());
		if(!gm)
			return;
		
		int minutes = gm.m_briefingTimer / 60;
		int seconds = gm.m_briefingTimer % 60;
		
		timer.SetText(string.Format("%1:%2", minutes.ToString(2), seconds.ToString(2)));
	}
}