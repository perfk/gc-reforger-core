modded class SCR_AIGroup : ChimeraAIGroup
{
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		PreventMaxLOD();
	}
}