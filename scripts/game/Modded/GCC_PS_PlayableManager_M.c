modded class PS_PlayableManager
{
	bool IsPlayerCompanyLeader(int thisPlayerId)
	{
 		array<PS_PlayableContainer> playables = GetPlayablesSorted();
        if (playables.Count() == 0)
            return false;

        protected RplId newestInFaction;
        foreach (PS_PlayableContainer playable : playables)
        {
            if (playable.GetFactionKey() == GetPlayerFactionKey(thisPlayerId))
                newestInFaction = playable.GetRplId();
        }
		
		Print("Perfk newestInFaction : " + newestInFaction + " - " + thisPlayerId);
		if(newestInFaction == thisPlayerId)
        	return true;
		else
			return false;
	}
}

/*
modded class PS_PlayableManager
{
    bool IsPlayerCompanyLeader(int thisPlayerId)
    {
        array<PS_PlayableContainer> playables = GetPlayablesSorted();
        if (playables.Count() == 0)
            return false;

        PS_PlayableContainer newestInFaction;
        foreach (PS_PlayableContainer playable : playables)
        {
            if (playable.GetFactionKey() == GetPlayerFactionKey(m_iCurrentPlayerId))
                newestInFaction = playable;
        }
		Print(Perfk newestInFaction : " + newestInFaction.GetRplId() + " - " + thisPlayerId);
		if(newestInFaction.GetRplId() == thisPlayerId)
        	return true;
		else
			return false;
    }
}
*/