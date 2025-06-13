modded class SCR_ChimeraCharacter
{
	override void OnSpecialContactsChagned(notnull array<IEntity> contacts)
	{
		RplComponent characterRplComp = GetRplComponent();
		bool isAuthority = characterRplComp && !characterRplComp.IsProxy();

		RplComponent changedEntityRplComp;
		if (m_aContacts)
		{
			RplId changedEntityRplId;
			for (int i = m_aContacts.Count() - 1; i >= 0; i--)
			{
				if (contacts.Contains(m_aContacts[i]))
					continue;

				if (!isAuthority)
				{
					if (m_aContacts[i])
						changedEntityRplComp = SCR_EntityHelper.GetEntityRplComponent(m_aContacts[i]);

					if (changedEntityRplComp)
						changedEntityRplId = changedEntityRplComp.Id();
					else
						changedEntityRplId = RplId.Invalid();
					
					if(changedEntityRplComp)
						Rpc(Rpc_ContactChanged, changedEntityRplId, false);
				}

				RemoveOldContact(m_aContacts[i]);
			}

			if (m_aContacts.IsEmpty())
				m_aContacts = null;
		}

		SCR_SpecialCollisionHandlerComponent specialCollisionComponent;
		foreach (IEntity newContact : contacts)
		{
			if (!m_aContacts || !m_aContacts.Contains(newContact))
			{
				specialCollisionComponent = SCR_SpecialCollisionHandlerComponent.Cast(newContact.FindComponent(SCR_SpecialCollisionHandlerComponent));
				if (!specialCollisionComponent)
					continue;//only store and replicate information about important contacts

				if (!isAuthority)
				{
					changedEntityRplComp = SCR_EntityHelper.GetEntityRplComponent(newContact);
					if (changedEntityRplComp)
					{
						Print("GRAY.2222");
						Rpc(Rpc_ContactChanged, changedEntityRplComp.Id(), true);
					}
				}
				
				AddNewContact(newContact);
			}
		}
	}
}
