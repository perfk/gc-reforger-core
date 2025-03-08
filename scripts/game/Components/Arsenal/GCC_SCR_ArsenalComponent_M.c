modded class SCR_ArsenalComponent : ScriptComponent
{
	override SCR_EArsenalSaveType GetArsenalSaveType()
	{
		BaseGameMode gm = GetGame().GetGameMode();
		if (gm && PS_GameModeCoop.Cast(gm))
			return SCR_EArsenalSaveType.SAVING_DISABLED;
		return m_eArsenalSaveType;
	}
}
