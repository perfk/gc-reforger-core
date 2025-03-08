modded class SCR_ArsenalComponent : ScriptComponent
{
	override SCR_EArsenalSaveType GetArsenalSaveType()
	{
		return SCR_EArsenalSaveType.SAVING_DISABLED; // m_eArsenalSaveType;
	}
}
