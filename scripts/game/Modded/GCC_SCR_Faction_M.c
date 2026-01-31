modded class SCR_Faction
{
	override SCR_EntityCatalog GetFactionEntityCatalogOfType(EEntityCatalogType catalogType, bool printNotFound = false)
	{	
		return super.GetFactionEntityCatalogOfType(catalogType, printNotFound);
	}
}